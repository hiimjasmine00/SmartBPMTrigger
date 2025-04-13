#include <cocos2d.h>

class SBTTriggerData : public cocos2d::CCObject {
public:
    std::vector<cocos2d::ccColor4B> m_colors;
    std::vector<float> m_widths;
    bool m_disabled;
    bool m_changed;
    int m_beats;

    static SBTTriggerData* create(const std::string&, int);

    std::string getSaveString();
};
