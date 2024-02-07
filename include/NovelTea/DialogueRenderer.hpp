#ifndef NOVELTEA_DIALOGUERENDERER_HPP
#define NOVELTEA_DIALOGUERENDERER_HPP

#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/GUI/ScrollBar.hpp>
#include <NovelTea/GUI/Icon/IconContinue.hpp>
#include <TweenEngine/Tween.h>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/StateEventManager.hpp>
#include <NovelTea/TextTypes.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>
#include <NovelTea/json.hpp>
#include <memory>

namespace NovelTea
{

class Button;
class Dialogue;
class DialogueSegment;

using DialogueCallback = std::function<void(int)>;

class DialogueRenderer : public ContextObject, public sf::Drawable, public Scrollable
{
public:
	DialogueRenderer(Context *context);
	~DialogueRenderer();

	void reset();
	void update(float delta);
	bool processEvent(const sf::Event &event);

	void processLines();
	bool processSelection(int buttonIndex);
	void setDialogueCallback(DialogueCallback callback);

	bool continueToNext();

	void changeLine();

	void show(float duration = 1.f, int startSegmentIndex = -1);
	void hide(float duration = 1.f);

	void setScroll(float position) override;
	float getScroll() override;
	const sf::Vector2f &getScrollSize() override;
	void repositionText();

	void setSize(const sf::Vector2f &size);
	sf::Vector2f getSize() const;

	void setFontSizeMultiplier(float fontSizeMultiplier);
	float getFontSizeMultiplier() const;

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void repositionButtons(float fontSize);
	void applyChanges();

private:
	std::shared_ptr<StateEvent::DialogueEvent> m_current;
	DialogueCallback m_callback;
	bool m_isShowing;
	bool m_logCurrentIndex;
	int m_eventListenerId;

	int m_textLineIndex;
	std::vector<std::pair<std::string,std::string>> m_textLines;

	std::vector<std::shared_ptr<ActiveText>> m_buttonTexts;
	std::vector<std::shared_ptr<ActiveText>> m_buttonTextsOld;
	std::vector<std::shared_ptr<Button>> m_buttons;
	std::vector<std::shared_ptr<Button>> m_buttonsOld;
	ActiveText m_textName;
	ActiveText m_textNameOld;
	ActiveText m_text;
	ActiveText m_textOld;
	sf::Vector2f m_size;
	sf::Texture *m_buttonTexture;
	IconContinue m_iconContinue;
	TweenNinePatch m_bg;
	float m_fontSize;
	float m_fontSizeMultiplier;

	AnimationProperties m_animProps;
	AnimationProperties m_animNameProps;

	float m_padding;
	float m_scrollPos;
	sf::Vector2f m_scrollAreaSize;
	ScrollBar m_scrollBar;
	sf::View m_view;

	TweenEngine::Tween *m_fadeTween;
	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_DIALOGUERENDERER_HPP
