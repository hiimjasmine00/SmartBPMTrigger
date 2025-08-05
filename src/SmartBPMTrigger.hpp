#include <Geode/binding/GameManager.hpp>
#include <Geode/loader/Mod.hpp>

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

class SmartBPMTrigger {
private:
    template <class T>
    static T getInternal(std::string_view key, geode::Mod* mod) {
        using SettingType = typename geode::SettingTypeForValueType<T>::SettingType;
        if (std::shared_ptr<SettingType> setting = std::static_pointer_cast<SettingType>(mod->getSetting(key))) return setting->getValue();
        else return T();
    }
public:
    static cocos2d::CCDirector* getDirector();
    static GameManager* getGameManager();

    template <class T>
    static geode::EventListener<geode::SettingChangedFilterV3>* listen(std::string_view key, auto&& callback, geode::Mod* mod = geode::Mod::get()) {
        using SettingType = typename geode::SettingTypeForValueType<T>::SettingType;
        return new geode::EventListener([callback = std::move(callback)](std::shared_ptr<geode::SettingV3> setting) {
            callback(std::static_pointer_cast<SettingType>(setting)->getValue());
        }, geode::SettingChangedFilterV3(mod, std::string(key)));
    }

    template <TemplateString key, class T>
    static T get(geode::Mod* mod = geode::Mod::get()) {
        static T value = (listen<T>(key, [](T newValue) {
            value = newValue;
        }), getInternal<T>(key, mod));
        return value;
    }

    template <TemplateString key, class T>
    static void set(T value, geode::Mod* mod = geode::Mod::get()) {
        using SettingType = typename geode::SettingTypeForValueType<T>::SettingType;
        if (std::shared_ptr<SettingType> setting = std::static_pointer_cast<SettingType>(mod->getSetting(key))) setting->setValue(value);
    }

    template <VariableString key, class T>
    static T variable() {
        auto dict = getGameManager()->m_valueKeeper;
        if (!dict) return T();
        auto value = dict->valueForKey(std::string(key));
        if (!value) return T();
        if constexpr (std::is_same_v<T, bool>) return value->boolValue();
        else if constexpr (std::is_integral_v<T>) return value->intValue();
    }

    static void modify(auto& self) {
        auto mod = geode::Mod::get();
        auto enabled = get<"enabled", bool>(mod);
        std::map<std::string, std::shared_ptr<geode::Hook>>& hooks = self.m_hooks;
        for (auto& [name, hook] : hooks) {
            hook->setAutoEnable(enabled);
        }
        if (!hooks.empty()) listen<bool>("enabled", [hooks](bool value) {
            for (auto& [name, hook] : hooks) {
                if (value) hook->enable().inspectErr([&name](const std::string& err) {
                    geode::log::error("Failed to enable {} hook: {}", name, err);
                });
                else hook->disable().inspectErr([&name](const std::string& err) {
                    geode::log::error("Failed to disable {} hook: {}", name, err);
                });
            }
        }, mod);
    }

    static std::vector<float> getGuidelines(DrawGridLayer* layer, geode::Mod* mod = geode::Mod::get());
    static ColorSelectPopup* createColorPopup(const cocos2d::ccColor4B&, float, int, ColorSelectDelegate*);
};
