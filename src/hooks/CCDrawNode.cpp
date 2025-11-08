#include "../SmartBPMTrigger.hpp"
#include <Geode/modify/CCDrawNode.hpp>
#include <jasmine/hook.hpp>

using namespace geode::prelude;

class $modify(SBTDrawNode, CCDrawNode) {
    static void onModify(ModifyBase<ModifyDerive<SBTDrawNode, CCDrawNode>>& self) {
        SmartBPMTrigger::drawNodeHook = jasmine::hook::get(self.m_hooks, "cocos2d::CCDrawNode::draw", false);
    }

    bool preRender() {
        auto a = getOpacity() / 255.0f;
        auto ret = false;
        if (a != 1.0f) {
            for (int i = 0; i < m_nBufferCount; i++) {
                auto& oldColor = m_pBuffer[i].colors;
                ccColor4B newColor(oldColor.r * a, oldColor.g * a, oldColor.b * a, oldColor.a * a);
                if (oldColor != newColor) {
                    oldColor = newColor;
                    ret = true;
                }
            }
            if (!m_bDirty) m_bDirty = ret;
        }
        return ret;
    }

    void postRender(const std::vector<ccV2F_C4B_T2F>& vertices) {
        for (int i = 0; i < m_nBufferCount; i++) {
            auto& oldColor = m_pBuffer[i].colors;
            auto& newColor = vertices[i].colors;
            if (oldColor != newColor) {
                oldColor = newColor;
                m_bDirty = true;
            }
        }
    }

    void draw() {
        std::vector<ccV2F_C4B_T2F> vertices(m_pBuffer, m_pBuffer + m_nBufferCount);
        auto result = preRender();
        CCDrawNode::draw();
        if (result) postRender(vertices);
    }
};
