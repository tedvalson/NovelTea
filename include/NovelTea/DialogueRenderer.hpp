#ifndef NOVELTEA_DIALOGUERENDERER_HPP
#define NOVELTEA_DIALOGUERENDERER_HPP

#include <NovelTea/TweenObjects.hpp>
#include <TweenEngine/TweenManager.h>
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

	bool processSelection(int buttonIndex);
	void setDialogueCallback(DialogueCallback callback);

	void changeSegment(int newSegmentIndex);

	bool isComplete() const;

	void setSize(const sf::Vector2f &size);
	sf::Vector2f getSize() const;

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	std::shared_ptr<Dialogue> m_dialogue;
	DialogueCallback m_callback;
	int m_currentSegmentIndex;
	bool m_isComplete;

	std::vector<Button> m_buttons;
	ActiveText m_text;
	ActiveText m_textOld;
	sf::Vector2f m_size;
	sf::Texture *m_buttonTexture;

	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_DIALOGUERENDERER_HPP
