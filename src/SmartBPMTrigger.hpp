#include <Geode/loader/SettingV3.hpp>

// Thanks Prevter https://github.com/EclipseMenu/EclipseMenu/blob/v1.3.2/src/modules/config/config.hpp#L137
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
public:
    inline static std::vector<float> guidelines;
    inline static std::unordered_map<std::string_view, std::shared_ptr<geode::SettingV3>> settings;
    inline static geode::Hook* drawNodeHook;

    static cocos2d::CCDirector* getDirector();
    static GameManager* getGameManager();
    static cocos2d::CCSpriteFrameCache* getSpriteFrameCache();
    static cocos2d::CCTextureCache* getTextureCache();
    static void refreshCache();

    template <class T>
    static geode::SettingTypeForValueType<T>::SettingType* getSetting(std::string_view key) {
        return static_cast<geode::SettingTypeForValueType<T>::SettingType*>(settings[key].get());
    }

    template <class T>
    static T get(std::string_view key) {
        return getSetting<T>(key)->getValue();
    }

    static void modify(std::map<std::string, std::shared_ptr<geode::Hook>>& hooks);
};
