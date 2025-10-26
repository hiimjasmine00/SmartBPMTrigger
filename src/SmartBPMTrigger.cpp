#include "SmartBPMTrigger.hpp"
#include <Geode/binding/GameManager.hpp>
#include <Geode/loader/Hook.hpp>
#include <Geode/loader/ModSettingsManager.hpp>
#include <ranges>

using namespace geode::prelude;

std::vector<float> SmartBPMTrigger::guidelines;
Hook* SmartBPMTrigger::drawNodeHook = nullptr;

std::unordered_map<std::string_view, SettingV3*>& SmartBPMTrigger::getSettings() {
    static std::unordered_map<std::string_view, SettingV3*> settings = [] {
        std::unordered_map<std::string_view, SettingV3*> settings;
        auto msm = ModSettingsManager::from(getMod());
        constexpr std::array keys = {
            "enabled",
            "orange-color",
            "orange-width",
            "yellow-color",
            "yellow-width",
            "green-color",
            "green-width",
            "beats-per-minute-color",
            "beats-per-minute-width",
            "beats-per-bar-color",
            "beats-per-bar-width",
            "snap-distribute",
            "snap-orange",
            "snap-yellow",
            "snap-green",
            "snap-bpm",
            "snap-bpb",
            "spawn-bpm"
        };
        for (auto key : keys) {
            if (auto setting = msm->get(key)) settings.emplace(key, setting.get());
        }
        return settings;
    }();
    return settings;
}

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
CCTextureCache* SmartBPMTrigger::getTextureCache() {
    if (!textureCache) textureCache = CCTextureCache::sharedTextureCache();
    return textureCache;
}

void SmartBPMTrigger::refreshCache() {
    spriteFrameCache = nullptr;
    textureCache = nullptr;
}

void SmartBPMTrigger::modify(std::map<std::string, std::shared_ptr<Hook>>& hooks) {
    if (hooks.empty()) return;

    auto enabled = get<bool>("enabled");
    log::info("Smart BPM Trigger is {}", enabled ? "enabled" : "disabled");
    for (auto& hook : std::views::values(hooks)) {
        hook->setAutoEnable(enabled);
    }

    new EventListener([hooks](std::shared_ptr<SettingV3> setting) {
        auto enabled = std::static_pointer_cast<BoolSettingV3>(std::move(setting))->getValue();
        for (auto& [name, hook] : hooks) {
            if (auto err = hook->toggle(enabled).err()) {
                log::error("Failed to toggle {} hook: {}", name, *err);
            }
        }
    }, SettingChangedFilterV3(GEODE_MOD_ID, "enabled"));
}
