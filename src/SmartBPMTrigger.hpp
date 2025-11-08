#include <cocos2d.h>
#include <Geode/GeneratedPredeclare.hpp>
#include <Geode/loader/Types.hpp>

class SmartBPMTrigger {
public:
    static std::vector<float> guidelines;
    static geode::Hook* drawNodeHook;

    static cocos2d::CCDirector* getDirector();
    static GameManager* getGameManager();
    static cocos2d::CCSpriteFrameCache* getSpriteFrameCache();
    static std::pair<cocos2d::CCTexture2D*, cocos2d::CCRect> getSquare();
    static void refreshCache();
};
