#include "SmartBPMTrigger.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    SmartBPMTrigger::gameManager = GameManager::get();
}

bool SmartBPMTrigger::enabled(Mod* mod) {
    return get<"enabled", bool>(mod);
}

ccColor4B SmartBPMTrigger::getColor(GuidelineType type, Mod* mod) {
    switch (type) {
        case GuidelineType::Orange: return get<"orange-color", ccColor4B>(mod);
        case GuidelineType::Yellow: return get<"yellow-color", ccColor4B>(mod);
        case GuidelineType::Green: return get<"green-color", ccColor4B>(mod);
        case GuidelineType::BPM: return get<"beats-per-minute-color", ccColor4B>(mod);
        case GuidelineType::BPB: return get<"beats-per-bar-color", ccColor4B>(mod);
    }
}

float SmartBPMTrigger::getWidth(GuidelineType type, Mod* mod) {
    switch (type) {
        case GuidelineType::Orange: return get<"orange-width", float>(mod);
        case GuidelineType::Yellow: return get<"yellow-width", float>(mod);
        case GuidelineType::Green: return get<"green-width", float>(mod);
        case GuidelineType::BPM: return get<"beats-per-minute-width", float>(mod);
        case GuidelineType::BPB: return get<"beats-per-bar-width", float>(mod);
    }
}

bool SmartBPMTrigger::getSnap(GuidelineType type, Mod* mod) {
    switch (type) {
        case GuidelineType::Orange: return get<"snap-orange", bool>(mod);
        case GuidelineType::Yellow: return get<"snap-yellow", bool>(mod);
        case GuidelineType::Green: return get<"snap-green", bool>(mod);
        case GuidelineType::BPM: return get<"snap-bpm", bool>(mod);
        case GuidelineType::BPB: return get<"snap-bpb", bool>(mod);
    }
}

void SmartBPMTrigger::setSnap(GuidelineType type, bool value, Mod* mod) {
    switch (type) {
        case GuidelineType::Orange: return set<"snap-orange", bool>(value, mod);
        case GuidelineType::Yellow: return set<"snap-yellow", bool>(value, mod);
        case GuidelineType::Green: return set<"snap-green", bool>(value, mod);
        case GuidelineType::BPM: return set<"snap-bpm", bool>(value, mod);
        case GuidelineType::BPB: return set<"snap-bpb", bool>(value, mod);
    }
}

bool SmartBPMTrigger::snapDistribute(Mod* mod) {
    return get<"snap-distribute", bool>(mod);
}

void SmartBPMTrigger::setSnapDistribute(bool value, Mod* mod) {
    set<"snap-distribute", bool>(value, mod);
}

int SmartBPMTrigger::spawnBPM(Mod* mod) {
    return get<"spawn-bpm", int>(mod);
}

void SmartBPMTrigger::setSpawnBPM(int value, Mod* mod) {
    set<"spawn-bpm", int>(value, mod);
}
