#ifndef NOVELTEA_STATESETTINGS_HPP
#define NOVELTEA_STATESETTINGS_HPP

#include <NovelTea/States/State.hpp>
#include <TweenEngine/TweenManager.h>
#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/GUI/Button.hpp>

namespace NovelTea
{

class StateSettings : public State
{
public:
	StateSettings(StateStack& stack, Context& context, StateCallback callback);
	bool processEvent(const sf::Event &event) override;
	bool update(float delta) override;
	void render(sf::RenderTarget &target) override;
	void resize(const sf::Vector2f &size) override;

	void setAlpha(float alpha) override;

private:
	TweenText m_textTitle;
	Button m_buttonBack;
	Button m_buttonTextSize;
	TweenNinePatch m_bg;
	TweenRectangleShape m_overlay;
	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_STATESETTINGS_HPP
