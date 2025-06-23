#include <Geode/binding/GameManager.hpp>
#include <Geode/loader/Mod.hpp>

#define SBT_MODIFY \
    static void onModify(auto& self) { \
        auto mod = Mod::get(); \
        auto enabled = SmartBPMTrigger::enabled(mod); \
        auto& hooks = self.m_hooks; \
        for (auto& [name, hook] : hooks) { \
            hook->setAutoEnable(enabled); \
        } \
        if (!hooks.empty()) SmartBPMTrigger::settingListener<"enabled", bool>([hooks](bool value) { \
            for (auto& [name, hook] : hooks) { \
                (void)(value ? hook->enable().inspectErr([&name](const std::string& err) { \
                    log::error("Failed to enable {} hook: {}", name, err); \
                }) : hook->disable().inspectErr([&name](const std::string& err) { \
                    log::error("Failed to disable {} hook: {}", name, err); \
                })); \
            } \
        }, mod); \
    } \

// Thanks Prevter https://github.com/EclipseMenu/EclipseMenu/blob/v1.1.0/src/modules/config/config.hpp#L135
template <size_t N>
struct TemplateString {
    static constexpr size_t size = N;
    char value[N]{};
    constexpr TemplateString() = default;
    constexpr TemplateString(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }
    constexpr operator std::string_view() const {
        return { value, N - 1 };
    }
};

template <size_t N>
struct VariableString : public TemplateString<N + 3> {
    using Base = TemplateString<N + 3>;
    constexpr VariableString(const char (&str)[N]) {
        std::copy_n("gv_", 3, Base::value);
        std::copy_n(str, N, Base::value + 3);
    }
};

enum class GuidelineType {
    Orange,
    Yellow,
    Green,
    BPM,
    BPB
};

class SmartBPMTrigger {
private:
    template <class T>
    static T getInternal(geode::Mod* mod, std::string_view key) {
        using SettingType = typename geode::SettingTypeForValueType<T>::SettingType;
        if (std::shared_ptr<SettingType> setting = std::static_pointer_cast<SettingType>(mod->getSetting(key))) return setting->getValue();
        else return T();
    }
public:
    inline static GameManager* gameManager = nullptr;

    template <TemplateString key, class T>
    static geode::EventListener<geode::SettingChangedFilterV3>* settingListener(auto&& callback, geode::Mod* mod = geode::Mod::get()) {
        using SettingType = typename geode::SettingTypeForValueType<T>::SettingType;
        return new geode::EventListener([callback = std::move(callback)](std::shared_ptr<geode::SettingV3> setting) {
            callback(std::static_pointer_cast<SettingType>(setting)->getValue());
        }, geode::SettingChangedFilterV3(mod, std::string(key)));
    }

    template <TemplateString key, class T>
    static T get(geode::Mod* mod = geode::Mod::get()) {
        static T value = (settingListener<key, T>([](T newValue) {
            value = newValue;
        }), getInternal<T>(mod, key));
        return value;
    }

    template <TemplateString key, class T>
    static void set(T value, geode::Mod* mod = geode::Mod::get()) {
        using SettingType = typename geode::SettingTypeForValueType<T>::SettingType;
        if (std::shared_ptr<SettingType> setting = std::static_pointer_cast<SettingType>(mod->getSetting(key))) setting->setValue(value);
    }

    template <VariableString key, class T>
    static T variable() {
        if (!gameManager) return T();
        auto value = gameManager->m_valueKeeper->valueForKey(std::string(key));
        if (!value) return T();
        if constexpr (std::is_same_v<T, bool>) return value->boolValue();
        else if constexpr (std::is_integral_v<T>) return value->intValue();
    }

    static bool enabled(geode::Mod* mod = geode::Mod::get());
    static cocos2d::ccColor4B getColor(GuidelineType type, geode::Mod* mod = geode::Mod::get());
    static float getWidth(GuidelineType type, geode::Mod* mod = geode::Mod::get());
    static bool getSnap(GuidelineType type, geode::Mod* mod = geode::Mod::get());
    static void setSnap(GuidelineType type, bool value, geode::Mod* mod = geode::Mod::get());
    static bool snapDistribute(geode::Mod* mod = geode::Mod::get());
    static void setSnapDistribute(bool value, geode::Mod* mod = geode::Mod::get());
    static int spawnBPM(geode::Mod* mod = geode::Mod::get());
    static void setSpawnBPM(int value, geode::Mod* mod = geode::Mod::get());
    static std::vector<float> getGuidelines(DrawGridLayer* layer, geode::Mod* mod = geode::Mod::get());
    static ColorSelectPopup* createColorPopup(const cocos2d::ccColor4B&, float, int, ColorSelectDelegate*);
};
