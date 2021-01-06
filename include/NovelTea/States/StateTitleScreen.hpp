#ifndef NOVELTEA_STATETITLESCREEN_HPP
#define NOVELTEA_STATETITLESCREEN_HPP

#include <NovelTea/States/State.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/GUI/Button.hpp>
#include <TweenEngine/TweenManager.h>
#include <NovelTea/TweenObjects.hpp>

namespace NovelTea
{

class StateTitleScreen : public State
{
public:
	StateTitleScreen(StateStack& stack, Context& context, StateCallback callback);
	bool processEvent(const sf::Event &event) override;
	bool update(float delta) override;
	void render(sf::RenderTarget &target) override;
	void resize(const sf::Vector2f &size) override;

	void setAlpha(float alpha) override;

private:
	Button m_buttonStart;
	Button m_buttonSettings;
	TweenRectangleShape m_bg;
	ActiveText m_textTitle;
	ActiveText m_textAuthor;
	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_STATETITLESCREEN_HPP
