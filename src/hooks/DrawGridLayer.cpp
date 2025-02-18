#include "../SmartBPMTrigger.hpp"
#include <Geode/binding/AudioLineGuideGameObject.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/LevelSettingsObject.hpp>
#include <Geode/modify/DrawGridLayer.hpp>

using namespace geode::prelude;

class $modify(SBTDrawGridLayer, DrawGridLayer) {
    struct Fields {
        std::vector<CCPoint> m_bpmPoints;
        std::vector<CCPoint> m_bpbPoints;
        std::vector<float> m_guidelines;
    };

    static void onModify(ModifyBase<ModifyDerive<SBTDrawGridLayer, DrawGridLayer>>& self) {
        auto drawRes = self.getHook("DrawGridLayer::draw");
        if (drawRes.isErr()) return log::error("Failed to get DrawGridLayer::draw hook: {}", drawRes.unwrapErr());

        auto drawHook = drawRes.unwrap();
        drawHook->setAutoEnable(SmartBPMTrigger::enabled());

        listenForSettingChanges<bool>("enabled", [drawHook](bool value) {
            auto changeRes = value ? drawHook->enable() : drawHook->disable();
            if (changeRes.isErr()) log::error("Failed to {} DrawGridLayer::draw hook: {}", value ? "enable" : "disable", changeRes.unwrapErr());
        });
    }

    void draw() override {
        std::unordered_map<int, AudioLineGuideGameObject*> audioLineGuides;
        for (auto& [k, v] : m_audioLineObjects) audioLineGuides[k] = v;
        m_audioLineObjects.clear();

        DrawGridLayer::draw();

        auto f = m_fields.self();
        f->m_bpmPoints.clear();
        f->m_bpbPoints.clear();
        f->m_guidelines.clear();

        if (!SmartBPMTrigger::GAME_MANAGER) {
            if (!audioLineGuides.empty()) for (auto& [k, v] : audioLineGuides) m_audioLineObjects[k] = v;
            return;
        }

        if (SmartBPMTrigger::GAME_MANAGER->m_showSongMarkers && m_timeMarkers) for (int i = 0; i < (m_timeMarkers->count() / 2) * 2; i += 2) {
            f->m_guidelines.push_back(m_timeMarkers->stringAtIndex(i)->floatValue());
        }

        if (audioLineGuides.empty()) return;

        auto origin = m_objectLayer->convertToNodeSpace({ 0.0f, 0.0f });
        auto winSize = CCDirector::get()->getWinSize() / m_objectLayer->getScale();
        auto left = origin.x;
        auto right = winSize.width + origin.x;
        auto bottom = std::max(-3000.0f, origin.y);
        auto top = std::min(m_editorLayer->m_levelSettings->m_dynamicLevelHeight ? 30090.0f : 2490.0f, winSize.height + origin.y);
        auto hideInvisible = SmartBPMTrigger::GAME_MANAGER->m_valueKeeper->valueForKey("gv_0121")->boolValue();
        for (const auto& [_, object] : audioLineGuides) {
            if (object->m_disabled || (hideInvisible && object->m_isHide && !object->m_isSelected) || object->m_beatsPerMinute <= 0) continue;

            auto bars = 0;
            auto bpb = object->m_beatsPerBar > 0 ? object->m_beatsPerBar : 1;
            auto initialTime = timeForPos(object->getPosition(), 0, 0, false, true, false, 0);
            auto timeInterval = 60.0 / (object->m_beatsPerMinute * bpb);
            for (auto time = initialTime; time < initialTime + object->m_duration; time += timeInterval) {
                auto pos = posForTime(time).x;
                if (pos < left || pos > right) {
                    bars++;
                    continue;
                }
                f->m_guidelines.push_back(pos);
                if (bars % bpb == 0) {
                    f->m_bpmPoints.push_back({ pos, bottom });
                    f->m_bpmPoints.push_back({ pos, top });
                }
                else {
                    f->m_bpbPoints.push_back({ pos, bottom });
                    f->m_bpbPoints.push_back({ pos, top });
                }
                bars++;
            }
        }

        if (f->m_bpmPoints.empty() && f->m_bpbPoints.empty()) {
            for (auto& [k, v] : audioLineGuides) m_audioLineObjects[k] = v;
            return;
        }

        auto mod = Mod::get();
        if (!f->m_bpmPoints.empty()) {
            auto bpmColor = SmartBPMTrigger::bpmColor(mod);
            ccDrawColor4B(bpmColor.r, bpmColor.g, bpmColor.b, bpmColor.a);
            glLineWidth(SmartBPMTrigger::bpmWidth(mod));
            ccDrawLines(f->m_bpmPoints.data(), f->m_bpmPoints.size());
        }
        if (!f->m_bpbPoints.empty()) {
            auto bpbColor = SmartBPMTrigger::bpbColor(mod);
            ccDrawColor4B(bpbColor.r, bpbColor.g, bpbColor.b, bpbColor.a);
            glLineWidth(SmartBPMTrigger::bpbWidth(mod));
            ccDrawLines(f->m_bpbPoints.data(), f->m_bpbPoints.size());
        }

        for (auto& [k, v] : audioLineGuides) m_audioLineObjects[k] = v;
    }
};

std::vector<float> SmartBPMTrigger::getGuidelines(DrawGridLayer* layer) {
    return layer ? static_cast<SBTDrawGridLayer*>(layer)->m_fields->m_guidelines : std::vector<float>();
}
