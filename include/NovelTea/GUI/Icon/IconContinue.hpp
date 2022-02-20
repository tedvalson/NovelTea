#ifndef NOVELTEA_ICONCONTINUE_HPP
#define NOVELTEA_ICONCONTINUE_HPP

#include <vector>
#include <TweenEngine/TweenManager.h>
#include "../../TweenObjects.hpp"

namespace NovelTea
{

class IconContinue : public sf::Drawable, public TweenTransformable<sf::Transformable>
{
public:
	IconContinue();

	void reset();

	TweenText &getText();

	void show(float duration = 1.f);
	void hide(float duration = 1.f);

	void update(float delta);

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	TweenEngine::TweenManager m_tweenManager;
	TweenText m_text;
};

} // namespace NovelTea


#endif // NOVELTEA_ICONCONTINUE_HPP
