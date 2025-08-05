#include "../SmartBPMTrigger.hpp"
#include "../classes/SBTTriggerData.hpp"
#ifndef GEODE_IS_WINDOWS
#include <dlfcn.h>
#endif
#include <Geode/binding/AudioLineGuideGameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/LevelSettingsObject.hpp>
#include <Geode/modify/DrawGridLayer.hpp>
#include <ranges>

using namespace geode::prelude;

#ifdef GEODE_IS_WINDOWS
HMODULE getGoodGrid() {
    static HMODULE instance = nullptr;
    if (!instance) instance = GetModuleHandleA("alphalaneous.good_grid.dll");
    return instance;
}
#else
void* getGoodGrid() {
    static void* instance = nullptr;
    if (!instance) instance = dlopen("alphalaneous.good_grid" GEODE_PLATFORM_EXTENSION, RTLD_LAZY);
    return instance;
}
#endif

class LineColor {
    ccColor4B m_colorA;
    ccColor4B m_colorB;
    bool m_hasColorB = false;

public:
    LineColor() = default;
    LineColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : m_colorA(r, g, b, a) {}
    LineColor(const ccColor4B& colorA) : m_colorA(colorA) {}
    LineColor(const ccColor4B& colorA, const ccColor4B& colorB) : m_colorA(colorA), m_colorB(colorB), m_hasColorB(true) {}

    ccColor4B getColorA() const {
        return m_colorA;
    }
    ccColor4B getColorB() const {
        return m_hasColorB ? m_colorB : m_colorA;
    }
};

class DrawGridAPI {
public:
    static DrawGridAPI* get() {
        auto goodGrid = getGoodGrid();
        return goodGrid ? reinterpret_cast<DrawGridAPI*(*)()>(
            #ifdef GEODE_IS_WINDOWS
            GetProcAddress(goodGrid, "?get@DrawGridAPI@@SAAEAV1@XZ")
            #else
            dlsym(goodGrid, "_ZN11DrawGridAPI3getEv")
            #endif
        )() : nullptr;
    }

    void drawLine(const ccVertex2F& start, const ccVertex2F& end, const LineColor& color, float width, bool blend = false) {
        auto goodGrid = getGoodGrid();
        if (goodGrid) reinterpret_cast<void(*)(DrawGridAPI*, const ccVertex2F&, const ccVertex2F&, const LineColor&, float, bool)>(
            #ifdef GEODE_IS_WINDOWS
            GetProcAddress(goodGrid, "?drawLine@DrawGridAPI@@QEAAXAEBU_ccVertex2F@cocos2d@@0AEBVLineColor@@M_N@Z")
            #else
            dlsym(goodGrid, "_ZN11DrawGridAPI8drawLineERKN7cocos2d11_ccVertex2FES3_RK9LineColorfb")
            #endif
        )(this, start, end, color, width, blend);
    }
};

class $modify(SBTDrawGridLayer, DrawGridLayer) {
    struct Fields {
        std::vector<float> m_orangeGuidelines;
        std::vector<float> m_yellowGuidelines;
        std::vector<float> m_greenGuidelines;
        std::vector<float> m_bpmGuidelines;
        std::vector<float> m_bpbGuidelines;
    };

    static void onModify(ModifyBase<ModifyDerive<SBTDrawGridLayer, DrawGridLayer>>& self) {
        (void)self.setHookPriorityBeforePre("DrawGridLayer::draw", "alphalaneous.good_grid");
        SmartBPMTrigger::modify(self);
    }

    void draw() override {
        std::unordered_map<int, AudioLineGuideGameObject*> audioLineGuides;
        for (auto& pair : m_audioLineObjects) audioLineGuides.insert(pair);
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
        auto winSize = SmartBPMTrigger::getDirector()->getWinSize() / m_objectLayer->getScale();
        auto left = origin.x;
        auto right = winSize.width + origin.x;
        auto bottom = std::max(-3000.0f, origin.y);
        auto top = std::min(m_editorLayer->m_levelSettings->m_dynamicLevelHeight ? 30090.0f : 2490.0f, winSize.height + origin.y);

        auto drawGridAPI = DrawGridAPI::get();
        auto mod = Mod::get();
        auto hideInvisible = SmartBPMTrigger::variable<"0121", bool>();
        auto bpmColor = SmartBPMTrigger::get<"beats-per-minute-color", ccColor4B>(mod);
        auto bpmWidth = SmartBPMTrigger::get<"beats-per-minute-width", float>(mod);
        auto bpbColor = SmartBPMTrigger::get<"beats-per-bar-color", ccColor4B>(mod);
        auto bpbWidth = SmartBPMTrigger::get<"beats-per-bar-width", float>(mod);
        for (auto object : std::views::values(audioLineGuides)) {
            if (object->m_disabled || (hideInvisible && object->m_isHide && !object->m_isSelected) || object->m_beatsPerMinute <= 0) continue;

            auto beats = 0;
            auto bpb = std::max(object->m_beatsPerBar, 1);
            auto triggerData = static_cast<SBTTriggerData*>(object->getUserObject("trigger-data"_spr));
            auto disabled = triggerData && triggerData->m_disabled;
            auto& objectPosition = object->getPosition();
            auto initialTime = disabled ? objectPosition.x : timeForPos(objectPosition, 0, 0, false, false, false, 0);
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

                if (drawGridAPI) {
                    drawGridAPI->drawLine(
                        { pos, bottom },
                        { pos, top },
                        triggerData->m_changed ? triggerData->m_colors[index] : index == 0 ? bpmColor : bpbColor,
                        triggerData->m_changed ? triggerData->m_widths[index] : index == 0 ? bpmWidth : bpbWidth
                    );
                }
                else {
                    ccDrawColor4B(triggerData->m_changed ? triggerData->m_colors[index] : index == 0 ? bpmColor : bpbColor);
                    glLineWidth(triggerData->m_changed ? triggerData->m_widths[index] : index == 0 ? bpmWidth : bpbWidth);
                    ccDrawLine({ pos, bottom }, { pos, top });
                }
            }
        }

        for (auto& pair : audioLineGuides) m_audioLineObjects.insert(pair);

        auto timeMarkerCount = timeMarkers->count();
        if (SmartBPMTrigger::getGameManager()->m_showSongMarkers && timeMarkerCount > 0) {
            std::vector<CCPoint> orangePoints;
            std::vector<CCPoint> yellowPoints;
            std::vector<CCPoint> greenPoints;

            auto orangeColor = SmartBPMTrigger::get<"orange-color", ccColor4B>(mod);
            auto yellowColor = SmartBPMTrigger::get<"yellow-color", ccColor4B>(mod);
            auto greenColor = SmartBPMTrigger::get<"green-color", ccColor4B>(mod);
            auto orangeWidth = SmartBPMTrigger::get<"orange-width", float>(mod);
            auto yellowWidth = SmartBPMTrigger::get<"yellow-width", float>(mod);
            auto greenWidth = SmartBPMTrigger::get<"green-width", float>(mod);

            for (int i = 0; i < timeMarkerCount - 1; i += 2) {
                auto pos = static_cast<CCString*>(timeMarkers->objectAtIndex(i))->floatValue();
                auto type = static_cast<CCString*>(timeMarkers->objectAtIndex(i + 1))->floatValue();

                auto& guidelines = type == 0.9f ? f->m_yellowGuidelines : type == 1.0f ? f->m_greenGuidelines : f->m_orangeGuidelines;
                guidelines.push_back(pos);

                if (pos < left || pos > right) continue;

                if (drawGridAPI) {
                    drawGridAPI->drawLine(
                        { pos, bottom },
                        { pos, top },
                        type == 0.9f ? yellowColor : type == 1.0f ? greenColor : orangeColor,
                        type == 0.9f ? yellowWidth : type == 1.0f ? greenWidth : orangeWidth
                    );
                }
                else {
                    auto& points = type == 0.9f ? yellowPoints : type == 1.0f ? greenPoints : orangePoints;
                    points.emplace_back(pos, bottom);
                    points.emplace_back(pos, top);
                }
            }

            if (!orangePoints.empty()) {
                ccDrawColor4B(orangeColor);
                glLineWidth(orangeWidth);
                ccDrawLines(orangePoints.data(), orangePoints.size());
            }

            if (!yellowPoints.empty()) {
                ccDrawColor4B(yellowColor);
                glLineWidth(yellowWidth);
                ccDrawLines(yellowPoints.data(), yellowPoints.size());
            }

            if (!greenPoints.empty()) {
                ccDrawColor4B(greenColor);
                glLineWidth(greenWidth);
                ccDrawLines(greenPoints.data(), greenPoints.size());
            }
        }

        if (m_timeMarkers) m_timeMarkers->addObjectsFromArray(timeMarkers);
        timeMarkers->release();

        glLineWidth(1.0f);
    }
};

std::vector<float> SmartBPMTrigger::getGuidelines(DrawGridLayer* layer, Mod* mod) {
    std::vector<float> ret;
    if (!layer) return ret;

    auto f = static_cast<SBTDrawGridLayer*>(layer)->m_fields.self();
    if (get<"snap-orange", bool>(mod)) ret.insert(ret.end(), f->m_orangeGuidelines.begin(), f->m_orangeGuidelines.end());
    if (get<"snap-yellow", bool>(mod)) ret.insert(ret.end(), f->m_yellowGuidelines.begin(), f->m_yellowGuidelines.end());
    if (get<"snap-green", bool>(mod)) ret.insert(ret.end(), f->m_greenGuidelines.begin(), f->m_greenGuidelines.end());
    if (get<"snap-bpm", bool>(mod)) ret.insert(ret.end(), f->m_bpmGuidelines.begin(), f->m_bpmGuidelines.end());
    if (get<"snap-bpb", bool>(mod)) ret.insert(ret.end(), f->m_bpbGuidelines.begin(), f->m_bpbGuidelines.end());
    std::ranges::sort(ret);
    return ret;
}
