#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/cocos.hpp>

class SBTColorWidget : public cocos2d::CCLayer, public cocos2d::extension::ColorPickerDelegate {
protected:
    cocos2d::ccColor4B m_color;
    float m_width;
    std::function<void(const cocos2d::ccColor4B&, float)> m_callback;
    cocos2d::extension::CCControlColourPicker* m_colorPicker;
    geode::Ref<cocos2d::CCArray> m_opacityNodes;
    cocos2d::CCLabelBMFont* m_redLabel;
    cocos2d::CCLabelBMFont* m_greenLabel;
    cocos2d::CCLabelBMFont* m_blueLabel;
    cocos2d::CCLabelBMFont* m_alphaLabel;
    cocos2d::CCLabelBMFont* m_widthLabel;
    geode::TextInput* m_redInput;
    geode::TextInput* m_greenInput;
    geode::TextInput* m_blueInput;
    geode::TextInput* m_alphaInput;
    geode::TextInput* m_widthInput;
    Slider* m_redSlider;
    Slider* m_greenSlider;
    Slider* m_blueSlider;
    Slider* m_alphaSlider;
    Slider* m_widthSlider;
    cocos2d::CCDrawNode* m_redNode;
    cocos2d::CCDrawNode* m_greenNode;
    cocos2d::CCDrawNode* m_blueNode;
    cocos2d::CCDrawNode* m_alphaNode;
    cocos2d::CCSprite* m_widthSprite;
    bool m_disableDelegate;

    bool init(const cocos2d::ccColor4B&, float, std::function<void(const cocos2d::ccColor4B&, float)>);
    void updatePicker();
    void updateRed(bool, bool, bool);
    void updateGreen(bool, bool, bool);
    void updateBlue(bool, bool, bool);
    void updateAlpha(bool, bool, bool);
    void updateWidth(bool, bool, bool);
    void updateNodes(bool, bool, bool, bool);
public:
    static SBTColorWidget* create(const cocos2d::ccColor4B& color, float width, std::function<void(const cocos2d::ccColor4B&, float)> callback);

    const cocos2d::ccColor4B& getColor() const {
        return m_color;
    }
    float getWidth() const {
        return m_width;
    }

    void colorValueChanged(cocos2d::ccColor3B color) override;
    void setValues(const cocos2d::ccColor4B& color, float width);
    void prepareActions(bool show);

    void onEnter() override;
    void onExit() override;
};
