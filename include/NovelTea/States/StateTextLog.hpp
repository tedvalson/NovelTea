#ifndef NOVELTEA_STATETEXTLOG_HPP
#define NOVELTEA_STATETEXTLOG_HPP

#include <NovelTea/States/State.hpp>
#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/GUI/Button.hpp>
#include <NovelTea/GUI/TextLog/TextLogRenderer.hpp>

namespace NovelTea
{

class StateTextLog : public State
{
public:
	StateTextLog(StateStack& stack, Context& context, StateCallback callback);

	void show(float duration = 0.5f, int tweenType = ALPHA, HideableCallback callback = nullptr) override;
	void hide(float duration = 0.5f, int tweenType = ALPHA, HideableCallback callback = nullptr) override;

	bool processEvent(const sf::Event &event) override;
	bool update(float delta) override;
	void render(sf::RenderTarget &target) override;
	void resize(const sf::Vector2f &size) override;
	void setAlpha(float alpha) override;

private:
	TweenRectangleShape m_bg;
	Button m_buttonClose;
	sf::Vector2f m_screenSize;
	TextLogRenderer m_textLogRenderer;
};

} // namespace NovelTea

#endif // NOVELTEA_STATETEXTLOG_HPP
