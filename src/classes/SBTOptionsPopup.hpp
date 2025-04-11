#include "SBTTriggerData.hpp"
#include <Geode/binding/ColorSelectDelegate.hpp>
#include <Geode/ui/Popup.hpp>

class SBTOptionsPopup : public geode::Popup<SBTTriggerData*>, ColorSelectDelegate {
protected:
    std::vector<cocos2d::ccColor4B> m_colors;
    std::vector<float> m_widths;

    bool setup(SBTTriggerData*) override;
public:
    static SBTOptionsPopup* create(SBTTriggerData*);

    void colorSelectClosed(CCNode*) override;
};
