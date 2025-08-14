#include "../SmartBPMTrigger.hpp"
#include <Geode/modify/GameManager.hpp>

using namespace geode::prelude;

class $modify(SBTGameManager, GameManager) {
    void reloadAllStep2() {
        GameManager::reloadAllStep2();
        if (m_reloadTextures) SmartBPMTrigger::refreshCache();
    }
};
