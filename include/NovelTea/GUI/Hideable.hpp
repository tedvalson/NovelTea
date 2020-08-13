#ifndef NOVELTEA_HIDEABLE_HPP
#define NOVELTEA_HIDEABLE_HPP

#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/Utils.hpp>
#include <TweenEngine/TweenManager.h>

namespace NovelTea
{

using HideableCallback = std::function<void(void)>;

class Hideable : public TweenTransformable<sf::Transformable>
{
public:
	static const int ALPHA = 11;

	Hideable();

	virtual void update(float delta);

	virtual void show(float duration = 0.5f, int tweenType = ALPHA, HideableCallback callback = nullptr);
	virtual void hide(float duration = 0.5f, int tweenType = ALPHA, HideableCallback callback = nullptr);
	bool isShowing() const;
	bool isVisible() const;

	virtual void setAlpha(float alpha) = 0;
	virtual float getAlpha() const = 0;

protected:
	virtual void setValues(int tweenType, float *newValues) override;
	virtual int getValues(int tweenType, float *returnValues) override;

	bool m_isHiding;
	bool m_isShowing;
	TweenEngine::TweenManager m_tweenManager;

private:
	bool m_visible;
};

} // namespace NovelTea

#endif // NOVELTEA_HIDEABLE_HPP
