#include "SBTColorWidget.hpp"
#include <Geode/loader/SettingV3.hpp>
#include <Geode/ui/Popup.hpp>

class SBTSettingsPopup : public geode::Popup {
protected:
    geode::Ref<SBTColorWidget> m_colorWidget;
    std::vector<cocos2d::CCNode*> m_otherNodes;
    geode::Ref<cocos2d::CCSprite> m_barSprite;
    cocos2d::CCSprite* m_barButtonSprite;
    geode::Color4BSettingV3* m_colorSetting;
    geode::FloatSettingV3* m_widthSetting;

    bool init(LevelEditorLayer*);
    void guidelineSnap(LevelEditorLayer*);
    void createLoop(LevelEditorLayer*);
    void showPicker(cocos2d::CCSprite*, geode::Color4BSettingV3*, geode::FloatSettingV3*);
public:
    static SBTSettingsPopup* create(LevelEditorLayer* layer);

    void onClose(cocos2d::CCObject* sender) override;
};
