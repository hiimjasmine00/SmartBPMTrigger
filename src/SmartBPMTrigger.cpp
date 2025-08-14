#include "SmartBPMTrigger.hpp"
#include <Geode/binding/GameManager.hpp>
#include <Geode/loader/Mod.hpp>
#include <ranges>

using namespace geode::prelude;

void addSetting(std::string_view name, Mod* mod) {
    SmartBPMTrigger::settings.emplace(name, mod->getSetting(name));
}

$execute {
    auto mod = Mod::get();
    addSetting("enabled", mod);
    addSetting("orange-color", mod);
    addSetting("orange-width", mod);
    addSetting("yellow-color", mod);
    addSetting("yellow-width", mod);
    addSetting("green-color", mod);
    addSetting("green-width", mod);
    addSetting("beats-per-minute-color", mod);
    addSetting("beats-per-minute-width", mod);
    addSetting("beats-per-bar-color", mod);
    addSetting("beats-per-bar-width", mod);
    addSetting("snap-distribute", mod);
    addSetting("snap-orange", mod);
    addSetting("snap-yellow", mod);
    addSetting("snap-green", mod);
    addSetting("snap-bpm", mod);
    addSetting("snap-bpb", mod);
    addSetting("spawn-bpm", mod);
}

static CCDirector* director = nullptr;
CCDirector* SmartBPMTrigger::getDirector() {
    if (!director) director = CCDirector::sharedDirector();
    return director;
}

static GameManager* gameManager = nullptr;
GameManager* SmartBPMTrigger::getGameManager() {
    if (!gameManager) gameManager = GameManager::sharedState();
    return gameManager;
}

static CCSpriteFrameCache* spriteFrameCache = nullptr;
CCSpriteFrameCache* SmartBPMTrigger::getSpriteFrameCache() {
    if (!spriteFrameCache) spriteFrameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
    return spriteFrameCache;
}

static CCTextureCache* textureCache = nullptr;
CCTextureCache* SmartBPMTrigger::getTextureCache() {
    if (!textureCache) textureCache = CCTextureCache::sharedTextureCache();
    return textureCache;
}

void SmartBPMTrigger::refreshCache() {
    spriteFrameCache = nullptr;
    textureCache = nullptr;
}

void SmartBPMTrigger::modify(std::map<std::string, std::shared_ptr<Hook>>& hooks) {
    // This gets run before $execute (When the settings map is populated)
    auto mod = Mod::get();
    auto enabled = static_cast<BoolSettingV3*>(mod->getSetting("enabled").get())->getValue();
    for (auto& hook : std::views::values(hooks)) {
        hook->setAutoEnable(enabled);
    }
    if (!hooks.empty()) {
        new EventListener([hooks = hooks](std::shared_ptr<SettingV3> setting) {
            for (auto& [name, hook] : hooks) {
                if (static_cast<BoolSettingV3*>(setting.get())->getValue()) {
                    hook->enable().inspectErr([&name](const std::string& err) {
                        log::error("Failed to enable {} hook: {}", name, err);
                    });
                } else {
                    hook->disable().inspectErr([&name](const std::string& err) {
                        log::error("Failed to disable {} hook: {}", name, err);
                    });
                }
            }
        }, SettingChangedFilterV3(mod, "enabled"));
    }
}
