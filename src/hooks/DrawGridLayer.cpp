#include "../SmartBPMTrigger.hpp"
#include "../classes/SBTTriggerData.hpp"
#include <Geode/binding/AudioLineGuideGameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/LevelSettingsObject.hpp>
#include <Geode/modify/DrawGridLayer.hpp>
#include <Geode/utils/ranges.hpp>

using namespace geode::prelude;

class $modify(SBTDrawGridLayer, DrawGridLayer) {
    struct Fields {
        std::vector<float> m_orangeGuidelines;
        std::vector<float> m_yellowGuidelines;
        std::vector<float> m_greenGuidelines;
        std::vector<float> m_bpmGuidelines;
        std::vector<float> m_bpbGuidelines;
    };

    SBT_MODIFY(DrawGridLayer)

    void draw() override {
        std::unordered_map<int, AudioLineGuideGameObject*> audioLineGuides;
        for (auto& [k, v] : m_audioLineObjects) audioLineGuides[k] = v;
        m_audioLineObjects.clear();

        auto timeMarkers = CCArray::create();
        if (m_timeMarkers) {
            timeMarkers->addObjectsFromArray(m_timeMarkers);
            m_timeMarkers->removeAllObjects();
        }

        DrawGridLayer::draw();

        auto f = m_fields.self();
        f->m_orangeGuidelines.clear();
        f->m_yellowGuidelines.clear();
        f->m_greenGuidelines.clear();
        f->m_bpmGuidelines.clear();
        f->m_bpbGuidelines.clear();

        auto origin = m_objectLayer->convertToNodeSpace({ 0.0f, 0.0f });
        auto winSize = CCDirector::get()->getWinSize() / m_objectLayer->getScale();
        auto left = origin.x;
        auto right = winSize.width + origin.x;
        auto bottom = std::max(-3000.0f, origin.y);
        auto top = std::min(m_editorLayer->m_levelSettings->m_dynamicLevelHeight ? 30090.0f : 2490.0f, winSize.height + origin.y);

        auto mod = Mod::get();
        auto hideInvisible = SmartBPMTrigger::variable<"0121", bool>();
        auto bpmColor = SmartBPMTrigger::getColor(GuidelineType::BPM, mod);
        auto bpmWidth = SmartBPMTrigger::getWidth(GuidelineType::BPM, mod);
        auto bpbColor = SmartBPMTrigger::getColor(GuidelineType::BPB, mod);
        auto bpbWidth = SmartBPMTrigger::getWidth(GuidelineType::BPB, mod);
        for (auto& [_, object] : audioLineGuides) {
            if (object->m_disabled || (hideInvisible && object->m_isHide && !object->m_isSelected) || object->m_beatsPerMinute <= 0) continue;

            auto beats = 0;
            auto bpb = std::max(object->m_beatsPerBar, 1);
            auto triggerData = static_cast<SBTTriggerData*>(object->getUserObject("trigger-data"_spr));
            auto disabled = triggerData && triggerData->m_disabled;
            auto initialTime = disabled ? object->getPositionX() : timeForPos(object->getPosition(), 0, 0, false, true, false, 0);
            auto speed = 1.0f;
            if (disabled) {
                switch (object->m_speed) {
                    case Speed::Slow: speed = m_slowSpeed; break;
                    case Speed::Fast: speed = m_fastSpeed; break;
                    case Speed::Faster: speed = m_fasterSpeed; break;
                    case Speed::Fastest: speed = m_fastestSpeed; break;
                    default: speed = m_normalSpeed; break;
                }
            }
            auto timeInterval = speed * 60.0f / (object->m_beatsPerMinute * bpb);
            for (auto time = initialTime; time < initialTime + object->m_duration * speed; time += timeInterval, beats++) {
                auto pos = disabled ? time : posForTime(time).x;
                auto index = beats % bpb;
                if (index == 0) f->m_bpmGuidelines.push_back(pos);
                else f->m_bpbGuidelines.push_back(pos);

                if (pos < left || pos > right || !triggerData || triggerData->m_colors.empty() || triggerData->m_widths.empty()) continue;

                ccDrawColor4B(triggerData->m_changed ? triggerData->m_colors[index] : index == 0 ? bpmColor : bpbColor);
                glLineWidth(triggerData->m_changed ? triggerData->m_widths[index] : index == 0 ? bpmWidth : bpbWidth);
                ccDrawLine({ pos, bottom }, { pos, top });
            }
        }

        for (auto& [k, v] : audioLineGuides) m_audioLineObjects[k] = v;

        if (SmartBPMTrigger::gameManager && SmartBPMTrigger::gameManager->m_showSongMarkers && timeMarkers->count() > 0) {
            for (int i = 0; i < timeMarkers->count(); i += 2) {
                auto pos = timeMarkers->stringAtIndex(i)->floatValue();
                auto type = timeMarkers->stringAtIndex(i + 1)->floatValue();

                if (type == 0.9f) f->m_yellowGuidelines.push_back(pos);
                else if (type == 1.0f) f->m_greenGuidelines.push_back(pos);
                else f->m_orangeGuidelines.push_back(pos);
            }

            if (!f->m_orangeGuidelines.empty()) {
                ccDrawColor4B(SmartBPMTrigger::getColor(GuidelineType::Orange, mod));
                glLineWidth(SmartBPMTrigger::getWidth(GuidelineType::Orange, mod));
                std::vector<CCPoint> points;
                for (auto& pos : ranges::filter(f->m_orangeGuidelines, [left, right](float pos) { return pos >= left && pos <= right; })) {
                    points.push_back({ pos, bottom });
                    points.push_back({ pos, top });
                }
                ccDrawLines(points.data(), points.size());
            }

            if (!f->m_yellowGuidelines.empty()) {
                ccDrawColor4B(SmartBPMTrigger::getColor(GuidelineType::Yellow, mod));
                glLineWidth(SmartBPMTrigger::getWidth(GuidelineType::Yellow, mod));
                std::vector<CCPoint> points;
                for (auto& pos : ranges::filter(f->m_yellowGuidelines, [left, right](float pos) { return pos >= left && pos <= right; })) {
                    points.push_back({ pos, bottom });
                    points.push_back({ pos, top });
                }
                ccDrawLines(points.data(), points.size());
            }

            if (!f->m_greenGuidelines.empty()) {
                ccDrawColor4B(SmartBPMTrigger::getColor(GuidelineType::Green, mod));
                glLineWidth(SmartBPMTrigger::getWidth(GuidelineType::Green, mod));
                std::vector<CCPoint> points;
                for (auto& pos : ranges::filter(f->m_greenGuidelines, [left, right](float pos) { return pos >= left && pos <= right; })) {
                    points.push_back({ pos, bottom });
                    points.push_back({ pos, top });
                }
                ccDrawLines(points.data(), points.size());
            }
        }

        if (m_timeMarkers) m_timeMarkers->addObjectsFromArray(timeMarkers);
        timeMarkers->release();
    }
};

std::vector<float> SmartBPMTrigger::getGuidelines(DrawGridLayer* layer, GuidelineType type) {
    std::vector<float> ret;
    if (!layer) return ret;

    auto f = static_cast<SBTDrawGridLayer*>(layer)->m_fields.self();
    if ((type & GuidelineType::Orange) != GuidelineType::None) ret.insert(ret.end(), f->m_orangeGuidelines.begin(), f->m_orangeGuidelines.end());
    if ((type & GuidelineType::Yellow) != GuidelineType::None) ret.insert(ret.end(), f->m_yellowGuidelines.begin(), f->m_yellowGuidelines.end());
    if ((type & GuidelineType::Green) != GuidelineType::None) ret.insert(ret.end(), f->m_greenGuidelines.begin(), f->m_greenGuidelines.end());
    if ((type & GuidelineType::BPM) != GuidelineType::None) ret.insert(ret.end(), f->m_bpmGuidelines.begin(), f->m_bpmGuidelines.end());
    if ((type & GuidelineType::BPB) != GuidelineType::None) ret.insert(ret.end(), f->m_bpbGuidelines.begin(), f->m_bpbGuidelines.end());
    std::ranges::sort(ret);
    return ret;
}
