#include "../SmartBPMTrigger.hpp"
#include <Geode/modify/CCDrawNode.hpp>
#include <jasmine/hook.hpp>

using namespace geode::prelude;

class $modify(SBTDrawNode, CCDrawNode) {
    static void onModify(ModifyBase<ModifyDerive<SBTDrawNode, CCDrawNode>>& self) {
        SmartBPMTrigger::drawNodeHook = jasmine::hook::get(self.m_hooks, "cocos2d::CCDrawNode::draw", false);
    }

    void draw() {
        auto a = getOpacity() / 255.0f;
        if (a == 1.0f) return CCDrawNode::draw();

        std::vector<ccV2F_C4B_T2F> vertices(m_pBuffer, m_pBuffer + m_nBufferCount);

        auto result = false;
        for (int i = 0; i < m_nBufferCount; i++) {
            auto& oldColor = m_pBuffer[i].colors;
            ccColor4B newColor(oldColor.r * a, oldColor.g * a, oldColor.b * a, oldColor.a * a);
            if (oldColor != newColor) {
                oldColor = newColor;
                result = true;
            }
        }
        if (!m_bDirty) m_bDirty = result;

        CCDrawNode::draw();

        if (result) {
            for (int i = 0; i < m_nBufferCount; i++) {
                auto& oldColor = m_pBuffer[i].colors;
                auto& newColor = vertices[i].colors;
                if (oldColor != newColor) {
                    oldColor = newColor;
                    m_bDirty = true;
                }
            }
        }
    }
};
