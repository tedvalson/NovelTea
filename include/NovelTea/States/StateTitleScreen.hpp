#ifndef NOVELTEA_STATETITLESCREEN_HPP
#define NOVELTEA_STATETITLESCREEN_HPP

#include <NovelTea/States/State.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/GUI/Button.hpp>
#include <TweenEngine/TweenManager.h>
#include <NovelTea/TweenObjects.hpp>

namespace NovelTea
{

class StateTitleScreen : public State, public TweenEngine::Tweenable
{
public:
	static const int ALPHA = 1;

	StateTitleScreen(StateStack& stack, Context& context, StateCallback callback);
	bool processEvent(const sf::Event &event) override;
	bool update(float delta) override;
	void render(sf::RenderTarget &target) override;

	void setAlpha(float alpha);
	virtual int getValues(int tweenType, float *returnValues) override;
	virtual void setValues(int tweenType, float *newValues) override;

private:
	float m_alpha;
	Button m_buttonStart;
	Button m_buttonSettings;
	TweenRectangleShape m_bg;
	ActiveText m_textTitle;
	ActiveText m_textAuthor;
	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_STATETITLESCREEN_HPP
