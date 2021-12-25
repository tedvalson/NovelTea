#ifndef NOVELTEA_STATETEXTSETTINGS_HPP
#define NOVELTEA_STATETEXTSETTINGS_HPP

#include <NovelTea/States/State.hpp>
#include <TweenEngine/TweenManager.h>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/GUI/Button.hpp>

namespace NovelTea
{

class StateTextSettings : public State
{
public:
	StateTextSettings(StateStack& stack, Context& context, StateCallback callback);
	bool processEvent(const sf::Event &event) override;
	bool update(float delta) override;
	void render(sf::RenderTarget &target) override;
	void resize(const sf::Vector2f &size) override;

	void setAlpha(float alpha) override;

	void changeSizeMultiplier(float multiplier);

private:
	float m_multiplier;
	ActiveText m_roomActiveText;

	TweenText m_textTitle;
	TweenText m_textValue;
	Button m_buttonCancel;
	Button m_buttonFinish;
	Button m_buttonSizeDec;
	Button m_buttonSizeInc;
	TweenRectangleShape m_bg;
	TweenRectangleShape m_toolbarBg;
	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_STATETEXTSETTINGS_HPP
