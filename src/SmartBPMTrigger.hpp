#include <cocos2d.h>
#include <Geode/GeneratedPredeclare.hpp>
#include <Geode/loader/Types.hpp>

namespace SmartBPMTrigger {
    extern std::vector<float> guidelines;
    extern geode::Hook* drawNodeHook;

    cocos2d::CCDirector* getDirector();
    GameManager* getGameManager();
    cocos2d::CCSpriteFrameCache* getSpriteFrameCache();
    std::pair<cocos2d::CCTexture2D*, cocos2d::CCRect> getSquare();
    void refreshCache();
}
