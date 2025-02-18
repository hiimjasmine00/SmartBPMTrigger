#include "SmartBPMTrigger.hpp"
#include <Geode/binding/GameManager.hpp>

using namespace geode::prelude;

$on_mod(Loaded) {
    SmartBPMTrigger::GAME_MANAGER = GameManager::get();
}

bool SmartBPMTrigger::enabled(Mod* mod) {
    return get<"enabled", bool>(mod);
}

ccColor4B SmartBPMTrigger::bpmColor(Mod* mod) {
    return get<"beats-per-minute-color", ccColor4B>(mod);
}

float SmartBPMTrigger::bpmWidth(Mod* mod) {
    return get<"beats-per-minute-width", float>(mod);
}

ccColor4B SmartBPMTrigger::bpbColor(Mod* mod) {
    return get<"beats-per-bar-color", ccColor4B>(mod);
}

float SmartBPMTrigger::bpbWidth(Mod* mod) {
    return get<"beats-per-bar-width", float>(mod);
}
