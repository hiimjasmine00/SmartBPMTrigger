#include <Geode/ui/Popup.hpp>

class SBTSettingsPopup : public geode::Popup<LevelEditorLayer*> {
protected:
    std::unordered_set<std::unique_ptr<geode::EventListenerProtocol>> m_listeners;

    bool setup(LevelEditorLayer*) override;
    void guidelineSnap(LevelEditorLayer*);
    void createLoop(LevelEditorLayer*);
public:
    static SBTSettingsPopup* create(LevelEditorLayer*);
};
