#include "SmartBPMTrigger.hpp"
#include <Geode/binding/GameManager.hpp>
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

std::vector<float> SmartBPMTrigger::guidelines;
Hook* SmartBPMTrigger::drawNodeHook = nullptr;

CCDirector* director = nullptr;
CCDirector* SmartBPMTrigger::getDirector() {
    if (!director) director = CCDirector::sharedDirector();
    return director;
}

GameManager* gameManager = nullptr;
GameManager* SmartBPMTrigger::getGameManager() {
    if (!gameManager) gameManager = GameManager::sharedState();
    return gameManager;
}

CCSpriteFrameCache* spriteFrameCache = nullptr;
CCSpriteFrameCache* SmartBPMTrigger::getSpriteFrameCache() {
    if (!spriteFrameCache) spriteFrameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
    return spriteFrameCache;
}

CCTextureCache* textureCache = nullptr;
std::pair<CCTexture2D*, CCRect> SmartBPMTrigger::getSquare() {
    if (!textureCache) textureCache = CCTextureCache::sharedTextureCache();
    auto squareTexture = textureCache->addImage("square.png"_spr, false);
    auto squareSize = squareTexture ? squareTexture->m_tContentSize : CCSize { 2.0f, 2.0f };
    squareSize /= SmartBPMTrigger::getDirector()->getContentScaleFactor();
    return { squareTexture, { { 0.0f, 0.0f }, squareSize } };
}

void SmartBPMTrigger::refreshCache() {
    spriteFrameCache = nullptr;
    textureCache = nullptr;
}
