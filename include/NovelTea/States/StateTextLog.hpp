#ifndef NOVELTEA_STATETEXTLOG_HPP
#define NOVELTEA_STATETEXTLOG_HPP

#include <NovelTea/States/State.hpp>
#include <TweenEngine/TweenManager.h>
#include <NovelTea/TweenObjects.hpp>

namespace NovelTea
{

class StateTextLog : public State
{
public:
	StateTextLog(StateStack& stack, Context& context, StateCallback callback);
	bool processEvent(const sf::Event &event) override;
	bool update(float delta) override;
	void render(sf::RenderTarget &target) override;

	void setAlpha(float alpha) override;

private:
	TweenText m_text;
	TweenRectangleShape m_bg;
	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_STATETEXTLOG_HPP
