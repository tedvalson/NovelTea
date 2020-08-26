#include <NovelTea/GUI/Hideable.hpp>
#include <TweenEngine/Tween.h>

namespace NovelTea
{

Hideable::Hideable()
: m_isHiding(false)
, m_isShowing(false)
, m_visible(false)
{
}

void Hideable::update(float delta)
{
	m_tweenManager.update(delta);
}

void Hideable::show(float duration, int tweenType, HideableCallback callback)
{
	if (!m_isShowing && !m_visible)
	{
		m_isShowing = true;
		m_isHiding = false;
		m_visible = true;
		m_tweenManager.killAll();
		TweenEngine::Tween::to(*this, tweenType, duration)
			.target(255.f)
			.setCallback(TweenEngine::TweenCallback::COMPLETE, [this, callback](TweenEngine::BaseTween*){
				m_isShowing = false;
				if (callback)
					callback();
			}).start(m_tweenManager);
	}
}

void Hideable::hide(float duration, int tweenType, HideableCallback callback)
{
	if (!m_isHiding && m_visible)
	{
		m_isHiding = true;
		m_isShowing = false;
		m_tweenManager.killAll();
		TweenEngine::Tween::to(*this, tweenType, duration)
			.target(0.f)
			.setCallback(TweenEngine::TweenCallback::COMPLETE, [this, callback](TweenEngine::BaseTween*){
				m_visible = false;
				m_isHiding = false;
				if (callback)
					callback();
			}).start(m_tweenManager);
	}
}

bool Hideable::isShowing() const
{
	return m_isShowing;
}

bool Hideable::isVisible() const
{
	return m_visible;
}

void Hideable::setValues(int tweenType, float *newValues)
{
	switch (tweenType) {
		case ALPHA: {
			setAlpha(newValues[0]);
			break;
		}
		default:
			TweenTransformable::setValues(tweenType, newValues);
	}
}

int Hideable::getValues(int tweenType, float *returnValues)
{
	switch (tweenType) {
	case ALPHA:
			returnValues[0] = getAlpha();
		return 1;
	default:
		return TweenTransformable::getValues(tweenType, returnValues);
	}
}

} // namespace NovelTea
