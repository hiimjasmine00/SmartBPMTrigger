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
    inline static GameManager* GAME_MANAGER = nullptr;

    template <TemplateString key, class T>
    static T get(geode::Mod* mod = geode::Mod::get()) {
        static T value = (geode::listenForSettingChanges<T>(key, [](T newValue) {
            value = newValue;
        }), getInternal<T>(mod, key));
        return value;
    }

    static bool enabled(geode::Mod* mod = geode::Mod::get());
    static cocos2d::ccColor4B bpmColor(geode::Mod* mod = geode::Mod::get());
    static float bpmWidth(geode::Mod* mod = geode::Mod::get());
    static cocos2d::ccColor4B bpbColor(geode::Mod* mod = geode::Mod::get());
    static float bpbWidth(geode::Mod* mod = geode::Mod::get());
    static std::vector<float> getGuidelines(DrawGridLayer* layer);
};
