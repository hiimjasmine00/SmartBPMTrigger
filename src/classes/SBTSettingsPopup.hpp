#include "SBTColorWidget.hpp"
#include <Geode/loader/SettingV3.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>

class SBTSettingsPopup : public geode::Popup, public TextInputDelegate {
protected:
    LevelEditorLayer* m_layer;
    geode::Ref<SBTColorWidget> m_colorWidget;
    std::vector<cocos2d::CCNode*> m_otherNodes;
    geode::Ref<cocos2d::CCSprite> m_barSprite;
    cocos2d::CCSprite* m_barButtonSprite;
    geode::Color4BSettingV3* m_colorSetting;
    geode::FloatSettingV3* m_widthSetting;
    std::array<geode::Color4BSettingV3*, 5> m_guidelineColorSettings;
    std::array<geode::FloatSettingV3*, 5> m_guidelineWidthSettings;
    std::array<geode::BoolSettingV3*, 6> m_guidelineSnapSettings;
    geode::FloatSettingV3* m_spawnBPMSetting;
    Slider* m_loopBPMSlider;
    geode::TextInput* m_loopBPMInput;

    bool init(LevelEditorLayer*);
    void onBar(cocos2d::CCObject*);
    void onToggleSnap(cocos2d::CCObject*);
    void sliderChanged(cocos2d::CCObject*);
    void textChanged(CCTextInputNode*) override;
    void onGuidelineSnap(cocos2d::CCObject*);
    void onCreateLoop(cocos2d::CCObject*);
    void showPicker(cocos2d::CCSprite*, geode::Color4BSettingV3*, geode::FloatSettingV3*);
public:
    static SBTSettingsPopup* create(LevelEditorLayer* layer);

    void onClose(cocos2d::CCObject* sender) override;
};
