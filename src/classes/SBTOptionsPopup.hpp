#include "SBTColorWidget.hpp"
#include <Geode/binding/FLAlertLayerProtocol.hpp>
#include <Geode/ui/Popup.hpp>

class SBTTriggerData;

class SBTOptionsPopup : public geode::Popup, public FLAlertLayerProtocol {
protected:
    SBTTriggerData* m_triggerData;
    geode::Ref<SBTColorWidget> m_colorWidget;
    cocos2d::CCMenu* m_barSpriteMenu;
    cocos2d::CCSprite* m_barSprite;
    cocos2d::CCMenu* m_resetMenu;
    std::vector<cocos2d::ccColor4B> m_colors;
    std::vector<cocos2d::ccColor4B> m_queuedColors;
    std::vector<float> m_widths;
    std::vector<float> m_queuedWidths;
    cocos2d::ccColor4B m_queuedColor;
    float m_queuedWidth;
    int m_index;

    bool init(SBTTriggerData*);
    void onBar(cocos2d::CCObject*);
    void onSoftReset(cocos2d::CCObject*);
    void onHardReset(cocos2d::CCObject*);
    void onSave(cocos2d::CCObject*);
    void onReset(cocos2d::CCObject*);
    void onClose(cocos2d::CCObject*) override;
    void FLAlert_Clicked(FLAlertLayer*, bool) override;
    void closeBar();
public:
    static SBTOptionsPopup* create(SBTTriggerData* triggerData);
};
