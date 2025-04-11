#include "SmartBPMTrigger.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    SmartBPMTrigger::gameManager = GameManager::get();
}

bool SmartBPMTrigger::enabled(Mod* mod) {
    return get<"enabled", bool>(mod);
}

ccColor4B SmartBPMTrigger::orangeColor(Mod* mod) {
    return get<"orange-color", ccColor4B>(mod);
}

float SmartBPMTrigger::orangeWidth(Mod* mod) {
    return get<"orange-width", float>(mod);
}

ccColor4B SmartBPMTrigger::yellowColor(Mod* mod) {
    return get<"yellow-color", ccColor4B>(mod);
}

float SmartBPMTrigger::yellowWidth(Mod* mod) {
    return get<"yellow-width", float>(mod);
}

ccColor4B SmartBPMTrigger::greenColor(Mod* mod) {
    return get<"green-color", ccColor4B>(mod);
}

float SmartBPMTrigger::greenWidth(Mod* mod) {
    return get<"green-width", float>(mod);
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

std::vector<float> SmartBPMTrigger::getGuidelines(DrawGridLayer* layer) {
    std::vector<float> guidelines;
    auto orangeGuidelines = getOrangeGuidelines(layer);
    auto yellowGuidelines = getYellowGuidelines(layer);
    auto greenGuidelines = getGreenGuidelines(layer);
    auto bpmGuidelines = getBPMGuidelines(layer);
    auto bpbGuidelines = getBPBGuidelines(layer);
    guidelines.insert(guidelines.end(), orangeGuidelines.begin(), orangeGuidelines.end());
    guidelines.insert(guidelines.end(), yellowGuidelines.begin(), yellowGuidelines.end());
    guidelines.insert(guidelines.end(), greenGuidelines.begin(), greenGuidelines.end());
    guidelines.insert(guidelines.end(), bpmGuidelines.begin(), bpmGuidelines.end());
    guidelines.insert(guidelines.end(), bpbGuidelines.begin(), bpbGuidelines.end());
    std::ranges::sort(guidelines);
    return guidelines;
}
