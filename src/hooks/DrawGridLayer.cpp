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

    static void onModify(ModifyBase<ModifyDerive<SBTDrawGridLayer, DrawGridLayer>>& self) {
        (void)self.getHook("DrawGridLayer::draw").map([](Hook* hook) {
            auto mod = Mod::get();
            hook->setAutoEnable(SmartBPMTrigger::enabled(mod));

            SmartBPMTrigger::settingListener<"enabled", bool>([hook](bool value) {
                (void)(value ? hook->enable().mapErr([](const std::string& err) {
                    return log::error("Failed to enable DrawGridLayer::draw hook: {}", err), err;
                }) : hook->disable().mapErr([](const std::string& err) {
                    return log::error("Failed to disable DrawGridLayer::draw hook: {}", err), err;
                }));
            }, mod);

            return hook;
        }).mapErr([](const std::string& err) {
            return log::error("Failed to get DrawGridLayer::draw hook: {}", err), err;
        });
    }

    void draw() override {
        std::unordered_map<int, AudioLineGuideGameObject*> audioLineGuides;
        for (auto& [k, v] : m_audioLineObjects) audioLineGuides[k] = v;
        m_audioLineObjects.clear();

        std::vector<std::pair<float, float>> timeMarkers;
        for (int i = 0; i < m_timeMarkers->count(); i += 2) {
            timeMarkers.push_back({
                m_timeMarkers->stringAtIndex(i)->floatValue(),
                m_timeMarkers->stringAtIndex(i + 1)->floatValue()
            });
        }
        m_timeMarkers->removeAllObjects();

        DrawGridLayer::draw();

        auto f = m_fields.self();
        f->m_bpmGuidelines.clear();
        f->m_bpbGuidelines.clear();

        auto origin = m_objectLayer->convertToNodeSpace({ 0.0f, 0.0f });
        auto winSize = CCDirector::get()->getWinSize() / m_objectLayer->getScale();
        auto left = origin.x;
        auto right = winSize.width + origin.x;
        auto bottom = std::max(-3000.0f, origin.y);
        auto top = std::min(m_editorLayer->m_levelSettings->m_dynamicLevelHeight ? 30090.0f : 2490.0f, winSize.height + origin.y);

        auto hideInvisible = SmartBPMTrigger::variable<"0121", bool>();
        for (auto& [_, object] : audioLineGuides) {
            if (object->m_disabled || (hideInvisible && object->m_isHide && !object->m_isSelected) || object->m_beatsPerMinute <= 0) continue;

            auto beats = 0;
            auto bpb = object->m_beatsPerBar > 0 ? object->m_beatsPerBar : 1;
            auto initialTime = timeForPos(object->getPosition(), 0, 0, false, true, false, 0);
            auto timeInterval = 60.0 / (object->m_beatsPerMinute * bpb);
            auto triggerData = static_cast<SBTTriggerData*>(object->getUserObject("trigger-data"_spr));
            for (auto time = initialTime; time < initialTime + object->m_duration; time += timeInterval, beats++) {
                auto pos = posForTime(time).x;
                if (beats % bpb == 0) f->m_bpmGuidelines.push_back(pos);
                else f->m_bpbGuidelines.push_back(pos);

                if (pos < left || pos > right || !triggerData || triggerData->m_colors.empty() || triggerData->m_widths.empty()) continue;

                auto index = beats % bpb;
                auto& [r, g, b, a] = triggerData->m_colors[index];
                ccDrawColor4B(r, g, b, a);
                glLineWidth(triggerData->m_widths[index]);
                ccDrawLine({ pos, bottom }, { pos, top });
            }
        }

        for (auto& [k, v] : audioLineGuides) m_audioLineObjects[k] = v;

        if (SmartBPMTrigger::gameManager && SmartBPMTrigger::gameManager->m_showSongMarkers) {
            auto mod = Mod::get();
            for (auto& [pos, type] : timeMarkers) {
                if (type == 0.8f) f->m_orangeGuidelines.push_back(pos);
                else if (type == 0.9f) f->m_yellowGuidelines.push_back(pos);
                else if (type == 1.0f) f->m_greenGuidelines.push_back(pos);
            }

            for (auto& pos : ranges::filter(f->m_orangeGuidelines, [left, right](float pos) { return pos >= left && pos <= right; })) {
                auto [r, g, b, a] = SmartBPMTrigger::orangeColor(mod);
                ccDrawColor4B(r, g, b, a);
                glLineWidth(SmartBPMTrigger::orangeWidth(mod));
                ccDrawLine({ pos, bottom }, { pos, top });
            }

            for (auto& pos : ranges::filter(f->m_yellowGuidelines, [left, right](float pos) { return pos >= left && pos <= right; })) {
                auto [r, g, b, a] = SmartBPMTrigger::yellowColor(mod);
                ccDrawColor4B(r, g, b, a);
                glLineWidth(SmartBPMTrigger::yellowWidth(mod));
                ccDrawLine({ pos, bottom }, { pos, top });
            }

            for (auto& pos : ranges::filter(f->m_greenGuidelines, [left, right](float pos) { return pos >= left && pos <= right; })) {
                auto [r, g, b, a] = SmartBPMTrigger::greenColor(mod);
                ccDrawColor4B(r, g, b, a);
                glLineWidth(SmartBPMTrigger::greenWidth(mod));
                ccDrawLine({ pos, bottom }, { pos, top });
            }
        }

        for (auto& [pos, type] : timeMarkers) {
            m_timeMarkers->addObject(CCString::create(fmt::format("{:.2f}", pos)));
            m_timeMarkers->addObject(CCString::create(fmt::format("{:.2f}", type)));
        }
    }
};

#define DEFINE_GET_GUIDELINES(name, member) \
    std::vector<float>& SmartBPMTrigger::name(DrawGridLayer* layer) { \
        static std::vector<float> empty; \
        return layer ? static_cast<SBTDrawGridLayer*>(layer)->m_fields->member : empty; \
    }

DEFINE_GET_GUIDELINES(getOrangeGuidelines, m_orangeGuidelines)
DEFINE_GET_GUIDELINES(getYellowGuidelines, m_yellowGuidelines)
DEFINE_GET_GUIDELINES(getGreenGuidelines, m_greenGuidelines)
DEFINE_GET_GUIDELINES(getBPMGuidelines, m_bpmGuidelines)
DEFINE_GET_GUIDELINES(getBPBGuidelines, m_bpbGuidelines)
