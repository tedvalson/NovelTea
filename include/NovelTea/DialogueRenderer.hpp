#ifndef NOVELTEA_DIALOGUERENDERER_HPP
#define NOVELTEA_DIALOGUERENDERER_HPP

#include <NovelTea/TweenObjects.hpp>
#include <TweenEngine/Tween.h>
#include <NovelTea/ActiveText.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>
#include <NovelTea/json.hpp>
#include <memory>

namespace NovelTea
{

class Button;
class Dialogue;
class ActiveText;

using DialogueCallback = std::function<void(int)>;

class DialogueRenderer : public sf::Drawable, public TweenTransformable<sf::Transformable>
{
public:
	DialogueRenderer();

	void setDialogue(const std::shared_ptr<Dialogue> &dialogue);
	void reset();
	void update(float delta);
	bool processEvent(const sf::Event &event);

	void processLines();
	bool processSelection(int buttonIndex);
	void setDialogueCallback(DialogueCallback callback);

	void changeSegment(int newSegmentIndex);
	void changeLine(int newLineIndex);

	bool isComplete() const;
	void show(float duration = 1.f);
	void hide(float duration = 1.f);

	void setSize(const sf::Vector2f &size);
	sf::Vector2f getSize() const;

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	std::shared_ptr<Dialogue> m_dialogue;
	DialogueCallback m_callback;
	int m_currentSegmentIndex;
	int m_nextForcedSegmentIndex;
	bool m_isComplete;

	int m_textLineIndex;
	std::vector<std::pair<std::string,std::string>> m_textLines;

	std::vector<std::shared_ptr<Button>> m_buttons;
	std::vector<std::shared_ptr<Button>> m_buttonsOld;
	ActiveText m_textName;
	ActiveText m_textNameOld;
	ActiveText m_text;
	ActiveText m_textOld;
	sf::Vector2f m_size;
	sf::Texture *m_buttonTexture;
	TweenNinePatch m_bg;
	float m_middleY;

	TweenEngine::Tween *m_fadeTween;
	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_DIALOGUERENDERER_HPP
