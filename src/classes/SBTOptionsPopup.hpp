#include "SBTColorWidget.hpp"
#include <Geode/ui/Popup.hpp>

class SBTTriggerData;

class SBTOptionsPopup : public geode::Popup<SBTTriggerData*> {
protected:
    SBTTriggerData* m_triggerData;
    geode::Ref<SBTColorWidget> m_colorWidget;
    cocos2d::CCMenu* m_barSpriteMenu;
    cocos2d::CCSprite* m_barSprite;
    cocos2d::CCMenu* m_resetMenu;
    std::vector<cocos2d::ccColor4B> m_colors;
    std::vector<float> m_widths;
    int m_index;

    bool setup(SBTTriggerData*) override;
    void onClose(cocos2d::CCObject*) override;
    void closeBar();
public:
    static SBTOptionsPopup* create(SBTTriggerData* triggerData);
};
