#ifndef NOVELTEA_ACTIONBUILDER_HPP
#define NOVELTEA_ACTIONBUILDER_HPP

#include <NovelTea/GUI/Hideable.hpp>
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

class ActionBuilder : public sf::Drawable, public Hideable
{
public:
	ActionBuilder();

	bool processEvent(const sf::Event& event);

	void show(float duration = 0.4f, int tweenType = ALPHA, HideableCallback callback = nullptr) override;
	void hide(float duration = 0.4f, int tweenType = ALPHA, HideableCallback callback = nullptr) override;

	void setAlpha(float alpha) override;
	float getAlpha() const override;

	void setVerb(const std::string &verbId);
	void setObject(const std::string &objectId, size_t index);
	const std::string &getVerb() const;
	const std::vector<std::string> &getObjects() const;
	std::shared_ptr<Action> getAction() const;

	void setSize(const sf::Vector2f &size);
	sf::Vector2f getSize() const;

	void setCallback(ActionBuilderCallback callback);

	void updateText();

protected:
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
	sf::Vector2f m_size;
	ActiveText m_text;

	TweenRectangleShape m_buttonCancel;
	TweenRectangleShape m_buttonConfirm;

	std::string m_verbId;
	std::vector<std::string> m_objectIds;

	ActionBuilderCallback m_callback;
};

} // namespace NovelTea

#endif // NOVELTEA_ACTIONBUILDER_HPP
