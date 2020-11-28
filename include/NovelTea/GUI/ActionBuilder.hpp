#ifndef NOVELTEA_ACTIONBUILDER_HPP
#define NOVELTEA_ACTIONBUILDER_HPP

#include <NovelTea/GUI/Hideable.hpp>
#include <NovelTea/GUI/Button.hpp>
#include <NovelTea/Utils.hpp>
#include <TweenEngine/TweenManager.h>
#include <SFML/Window/Event.hpp>
#include <vector>

namespace NovelTea
{

class Action;
class ActiveText;
class Verb;
class Object;

using ActionBuilderCallback = std::function<void(bool)>;

class ActionBuilder : public sf::Drawable, public Hideable
{
public:
	ActionBuilder();

	void update(float delta) override;
	bool processEvent(const sf::Event& event);

	void show(float duration = 0.4f, int tweenType = ALPHA, HideableCallback callback = nullptr) override;
	void hide(float duration = 0.4f, int tweenType = ALPHA, HideableCallback callback = nullptr) override;

	void setAlpha(float alpha) override;
	float getAlpha() const override;

	void setVerb(const std::string &verbId);
	void setObject(const std::string &objectId, size_t index);
	void setObject(const std::string &objectId);
	const std::string &getVerb() const;
	const std::vector<std::string> &getObjects() const;
	std::shared_ptr<Action> getAction() const;

	void setSize(const sf::Vector2f &size);
	sf::Vector2f getSize() const;

	void setSelectedIndex(size_t selectedIndex);
	size_t getSelectedIndex() const;
	void selectNextEmptyIndex();

	void setCallback(ActionBuilderCallback callback);

	void updateText();

protected:
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
	sf::Vector2f m_size;
	std::vector<std::unique_ptr<ActiveText>> m_texts;
	std::vector<std::unique_ptr<TweenRectangleShape>> m_emptyRects;
	TextFormat m_textFormat;

	sf::Color m_emptyRectColor;
	float m_emptyRectAlpha;

	TweenEngine::TweenManager m_tweenManager;
	size_t m_selectedIndex;

	Button m_buttonCancel;
	float m_buttonWidth;

	std::string m_verbId;
	std::vector<std::string> m_objectIds;

	ActionBuilderCallback m_callback;
};

} // namespace NovelTea

#endif // NOVELTEA_ACTIONBUILDER_HPP
