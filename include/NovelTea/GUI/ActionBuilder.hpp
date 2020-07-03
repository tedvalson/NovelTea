#ifndef NOVELTEA_ACTIONBUILDER_HPP
#define NOVELTEA_ACTIONBUILDER_HPP

#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/Utils.hpp>
#include <TweenEngine/TweenManager.h>
#include <SFML/Window/Event.hpp>
#include <vector>

namespace NovelTea
{

class Action;
class Verb;
class Object;

using ActionBuilderCallback = std::function<void(bool)>;

class ActionBuilder : public sf::Drawable, public TweenTransformable<sf::Transformable>
{
public:
	static const int ALPHA = 11;

	ActionBuilder();
	~ActionBuilder();

	void update(float delta);
	bool processEvent(const sf::Event& event);

	void show();
	void hide();
	bool isVisible() const;

	void setVerb(const std::string &verbId);
	void setObject(const std::string &objectId, size_t index);
	std::string getVerb() const;
	std::vector<std::string> getObjects() const;
	std::shared_ptr<Action> getAction() const;

	void setSize(const sf::Vector2f &size);
	sf::Vector2f getSize() const;

	void setCallback(ActionBuilderCallback callback);

	void updateText();

protected:
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	void setValues(int tweenType, float *newValues) override;
	int getValues(int tweenType, float *returnValues) override;

private:
	sf::Vector2f m_size;
	ActiveText m_text;
	TweenEngine::TweenManager m_tweenManager;

	bool m_visible;
	bool m_isHiding;
	bool m_isShowing;

	TweenRectangleShape m_buttonCancel;
	TweenRectangleShape m_buttonConfirm;

	std::string m_verbId;
	std::vector<std::string> m_objectIds;

	ActionBuilderCallback m_callback;
};

} // namespace NovelTea

#endif // NOVELTEA_ACTIONBUILDER_HPP
