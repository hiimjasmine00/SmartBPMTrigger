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
    static T getInternal(geode::Mod* mod, std::string_view key) {
        using SettingType = typename geode::SettingTypeForValueType<T>::SettingType;
        if (std::shared_ptr<SettingType> setting = std::static_pointer_cast<SettingType>(mod->getSetting(key))) {
            return setting->getValue();
        }
        return T();
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

    template <VariableString key, class T>
    static T variable() {
        if (!gameManager) return T();
        auto value = gameManager->m_valueKeeper->valueForKey(std::string(key));
        if (!value) return T();
        if constexpr (std::is_same_v<T, bool>) return value->boolValue();
        else if constexpr (std::is_integral_v<T>) return value->intValue();
    }

    static bool enabled(geode::Mod* mod = geode::Mod::get());
    static cocos2d::ccColor4B orangeColor(geode::Mod* mod = geode::Mod::get());
    static float orangeWidth(geode::Mod* mod = geode::Mod::get());
    static cocos2d::ccColor4B yellowColor(geode::Mod* mod = geode::Mod::get());
    static float yellowWidth(geode::Mod* mod = geode::Mod::get());
    static cocos2d::ccColor4B greenColor(geode::Mod* mod = geode::Mod::get());
    static float greenWidth(geode::Mod* mod = geode::Mod::get());
    static cocos2d::ccColor4B bpmColor(geode::Mod* mod = geode::Mod::get());
    static float bpmWidth(geode::Mod* mod = geode::Mod::get());
    static cocos2d::ccColor4B bpbColor(geode::Mod* mod = geode::Mod::get());
    static float bpbWidth(geode::Mod* mod = geode::Mod::get());
    static std::vector<float> getGuidelines(DrawGridLayer* layer);
    static std::vector<float>& getOrangeGuidelines(DrawGridLayer* layer);
    static std::vector<float>& getYellowGuidelines(DrawGridLayer* layer);
    static std::vector<float>& getGreenGuidelines(DrawGridLayer* layer);
    static std::vector<float>& getBPMGuidelines(DrawGridLayer* layer);
    static std::vector<float>& getBPBGuidelines(DrawGridLayer* layer);
    static ColorSelectPopup* createColorPopup(const cocos2d::ccColor4B&, float, int, ColorSelectDelegate*);
};
