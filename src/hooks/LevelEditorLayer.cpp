#include <Geode/binding/AudioLineGuideGameObject.hpp>
#include <Geode/binding/DrawGridLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

using namespace geode::prelude;

// This hook fixes a bug where the object is not removed from the grid layer when deleted.
// In the vanilla game, it is removed by the unique ID, which is not the same when the object is deleted by the "Undo" or "Redo" buttons.
// This bug is very dangerous, because it can cause a crash when the game tries to access the object that has been deleted.

class $modify(SBTLevelEditorLayer, LevelEditorLayer) {
    void removeSpecial(GameObject* object) {
        LevelEditorLayer::removeSpecial(object);

        if (object->m_objectID == 3642) {
            auto& audioLineObjects = m_drawGridLayer->m_audioLineObjects;
            for (auto it = audioLineObjects.begin(); it != audioLineObjects.end(); ++it) {
                if (it->second == object) {
                    audioLineObjects.erase(it);
                    break;
                }
            }
        }
    }
};
