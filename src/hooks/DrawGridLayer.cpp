#include "../SmartBPMTrigger.hpp"
#include "../classes/SBTTriggerData.hpp"
#include <alphalaneous.good_grid/include/DrawGridAPI.hpp>
#include <Geode/binding/AudioLineGuideGameObject.hpp>
#include <alphalaneous.good_grid/include/DrawLayers.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/LevelSettingsObject.hpp>
#include <Geode/modify/DrawGridLayer.hpp>

using namespace geode::prelude;

static std::unordered_map<int, bool> audioLineObjects;

class $modify(SBTDrawGridLayer, DrawGridLayer) {
    static void onModify(ModifyBase<ModifyDerive<SBTDrawGridLayer, DrawGridLayer>>& self) {
        SmartBPMTrigger::modify(self.m_hooks);
    }

    void draw() override {
        SmartBPMTrigger::guidelines.clear();
        DrawGridLayer::draw();
        audioLineObjects.clear();
    }
};

$on_mod(Loaded) {
    auto& api = DrawGridAPI::get();

    if (auto res = api.getNode<Guidelines>("guidelines")) {
        auto& guidelines = *res;
        auto orangeColor = SmartBPMTrigger::getSetting<ccColor4B>("orange-color");
        auto orangeWidth = SmartBPMTrigger::getSetting<float>("orange-width");
        auto orangeSnap = SmartBPMTrigger::getSetting<bool>("snap-orange");
        auto yellowColor = SmartBPMTrigger::getSetting<ccColor4B>("yellow-color");
        auto yellowWidth = SmartBPMTrigger::getSetting<float>("yellow-width");
        auto yellowSnap = SmartBPMTrigger::getSetting<bool>("snap-yellow");
        auto greenColor = SmartBPMTrigger::getSetting<ccColor4B>("green-color");
        auto greenWidth = SmartBPMTrigger::getSetting<float>("green-width");
        auto greenSnap = SmartBPMTrigger::getSetting<bool>("snap-green");
        guidelines.setPropertiesForValue([
            orangeColor, orangeWidth, orangeSnap,
            yellowColor, yellowWidth, yellowSnap,
            greenColor, greenWidth, greenSnap
        ](LineColor& color, float& value, float& width) {
            auto colorA = color.getColorA();
            auto yellow = colorA == ccColor4B { 255, 255, 0, 255 };
            auto green = colorA == ccColor4B { 127, 255, 0, 255 };
            if (auto colorSetting = yellow ? yellowColor : green ? greenColor : orangeColor) {
                color = colorSetting->getValue();
            }
            if (auto widthSetting = yellow ? yellowWidth : green ? greenWidth : orangeWidth) {
                width = widthSetting->getValue();
            }
            if (auto snapSetting = yellow ? yellowSnap : green ? greenSnap : orangeSnap) {
                if (snapSetting->getValue()) SmartBPMTrigger::guidelines.push_back(value);
            }
        });
    }

    if (auto res = api.getNode<BPMTriggers>("bpm-triggers")) {
        auto& triggers = *res;
        auto bpmColor = SmartBPMTrigger::getSetting<ccColor4B>("beats-per-minute-color");
        auto bpmWidth = SmartBPMTrigger::getSetting<float>("beats-per-minute-width");
        auto bpmSnap = SmartBPMTrigger::getSetting<bool>("snap-bpm");
        auto bpbColor = SmartBPMTrigger::getSetting<ccColor4B>("beats-per-bar-color");
        auto bpbWidth = SmartBPMTrigger::getSetting<float>("beats-per-bar-width");
        auto bpbSnap = SmartBPMTrigger::getSetting<bool>("snap-bpb");
        triggers.setPropertiesForBeats([
            bpmColor, bpmWidth, bpmSnap,
            bpbColor, bpbWidth, bpbSnap
        ](LineColor& lineColor, AudioLineGuideGameObject* object, float&, int, int bpb, float& lineWidth) {
            if (auto found = audioLineObjects.find(object->m_uniqueID); found != audioLineObjects.end()) {
                if (!found->second) {
                    lineColor = { 0, 0, 0, 0 };
                    lineWidth = 0.0f;
                }
                return;
            }

            auto triggerData = static_cast<SBTTriggerData*>(object->getUserObject("trigger-data"_spr));
            auto disabled = triggerData && triggerData->m_disabled;
            audioLineObjects.emplace(object->m_uniqueID, disabled);
            if (disabled) return;

            lineColor = { 0, 0, 0, 0 };
            lineWidth = 0.0f;

            auto editorLayer = SmartBPMTrigger::getGameManager()->m_levelEditorLayer;
            if (!editorLayer) return;

            auto objectLayer = editorLayer->m_objectLayer;
            auto cameraPos = -objectLayer->getPosition() / objectLayer->getScale();
            auto& api = DrawGridAPI::get();
            auto [width, height] = api.getWorldViewSize() / 2.0f;
            auto [minX, minY] = api.getGridBoundsOrigin();
            auto [maxX, maxY] = api.getGridBoundsSize();
            auto pad = PADDING;
            auto left = std::max(cameraPos.x - width - pad, minX);
            auto right = std::min(cameraPos.x + width + pad, maxX);
            auto top = std::max(cameraPos.y - height - pad, minY);
            auto bottom = std::min(cameraPos.y + height + pad, (editorLayer->m_levelSettings->m_dynamicLevelHeight ? maxY : MAX_HEIGHT));

            auto drawGridLayer = editorLayer->m_drawGridLayer;
            auto initialTime = drawGridLayer->timeForPos(object->getPosition(), 0, 0, false, false, false, 0);
            auto finalTime = initialTime + object->m_duration;
            auto timeInterval = 60.0f / (object->m_beatsPerMinute * bpb);

            auto beats = 0;
            for (auto time = initialTime; time < finalTime; time += timeInterval, beats++) {
                auto pos = drawGridLayer->posForTime(time).x;
                auto index = beats % bpb;
                if (auto snapSetting = index == 0 ? bpmSnap : bpbSnap) {
                    if (snapSetting->getValue()) SmartBPMTrigger::guidelines.push_back(pos);
                }

                if (pos < left || pos > right) continue;

                ccColor4B color = { 0, 0, 0, 0 };
                if (triggerData && triggerData->m_changed && index < triggerData->m_colors.size()) {
                    color = triggerData->m_colors[index];
                }
                else if (auto colorSetting = index == 0 ? bpmColor : bpbColor) {
                    color = colorSetting->getValue();
                }

                auto width = 0.0f;
                if (triggerData && triggerData->m_changed && index < triggerData->m_widths.size()) {
                    width = triggerData->m_widths[index];
                }
                else if (auto widthSetting = index == 0 ? bpmWidth : bpbWidth) {
                    width = widthSetting->getValue();
                }

                api.drawLine({ pos, top }, { pos, bottom }, color, width);
            }
        });
    }
}
