#include "SmartBPMTrigger.hpp"

using namespace geode::prelude;

CCDirector* SmartBPMTrigger::getDirector() {
    static CCDirector* instance = nullptr;
    if (!instance) instance = CCDirector::sharedDirector();
    return instance;
}

GameManager* SmartBPMTrigger::getGameManager() {
    static GameManager* instance = nullptr;
    if (!instance) instance = GameManager::sharedState();
    return instance;
}
