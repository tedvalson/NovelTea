#ifndef NOVELTEA_ICONCONTINUE_HPP
#define NOVELTEA_ICONCONTINUE_HPP

#include <NovelTea/ContextObject.hpp>
#include <TweenEngine/TweenManager.h>
#include "../../TweenObjects.hpp"
#include <vector>

namespace NovelTea
{

class IconContinue : public ContextObject, public sf::Drawable, public TweenTransformable<sf::Transformable>
{
public:
	IconContinue(Context *context);

	void reset();

	TweenText &getText();

	void show(float duration = 1.f);
	void hide(float duration = 1.f);
	bool isShowing() const;

	void update(float delta);

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	TweenEngine::TweenManager m_tweenManager;
	TweenText m_text;
	bool m_isShowing;
};

} // namespace NovelTea


#endif // NOVELTEA_ICONCONTINUE_HPP
