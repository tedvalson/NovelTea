#ifndef NOVELTEA_STATEINTRO_HPP
#define NOVELTEA_STATEINTRO_HPP

#include <NovelTea/States/State.hpp>
#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/ActiveText.hpp>
#include <TweenEngine/TweenManager.h>
#include <SFML/Graphics/RenderTexture.hpp>

namespace NovelTea
{

class StateIntro : public State
{
public:
	StateIntro(StateStack& stack, Context& context, StateCallback callback);
	bool processEvent(const sf::Event &event) override;
	bool update(float delta) override;
	void render(sf::RenderTarget &target) override;

private:
	mutable sf::RenderTexture m_renderTexture;
	ActiveText m_textBg;
	TweenSprite m_sprite;
	TweenSprite m_spriteLogo;
	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_STATEINTRO_HPP
