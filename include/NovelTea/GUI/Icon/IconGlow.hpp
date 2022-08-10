#ifndef NOVELTEA_ICONGLOW_HPP
#define NOVELTEA_ICONGLOW_HPP

#include <NovelTea/ContextObject.hpp>
#include <TweenEngine/TweenManager.h>
#include "../../TweenObjects.hpp"
#include <vector>

namespace NovelTea
{

class IconGlow : public ContextObject, public sf::Drawable, public TweenTransformable<sf::Transformable>
{
public:
	IconGlow(Context *context);

	void reset();
	void setSpeed(float factor);

	TweenText &getText();

	void show(float duration = 1.f, float expiration = 0.f);
	void hide(float duration = 1.f);

	void update(float delta);

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	TweenEngine::TweenManager m_tweenManager;

	float m_speedFactor;
	TweenText m_text;
};

} // namespace NovelTea


#endif // NOVELTEA_ICONGLOW_HPP
