#include <Geode/loader/SettingV3.hpp>

class SmartBPMTrigger {
public:
    static std::vector<float> guidelines;
    static geode::Hook* drawNodeHook;

    static cocos2d::CCDirector* getDirector();
    static GameManager* getGameManager();
    static cocos2d::CCSpriteFrameCache* getSpriteFrameCache();
    static cocos2d::CCTextureCache* getTextureCache();
    static void refreshCache();

    static std::unordered_map<std::string_view, geode::SettingV3*>& getSettings();

    template <class T>
    static geode::SettingTypeForValueType<T>::SettingType* getSetting(std::string_view key) {
        auto& settings = getSettings();
        if (auto it = settings.find(key); it != settings.end()) {
            return static_cast<geode::SettingTypeForValueType<T>::SettingType*>(it->second);
        }
        return nullptr;
    }

    template <class T>
    static T get(std::string_view key) {
        if (auto setting = getSetting<T>(key)) {
            return setting->getValue();
        }
        return T();
    }

    static void modify(std::map<std::string, std::shared_ptr<geode::Hook>>& hooks);
};
