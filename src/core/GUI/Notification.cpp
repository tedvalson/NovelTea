#include <TweenEngine/Tween.h>
#include <cmath>
#include <NovelTea/GUI/Notification.hpp>
#include <NovelTea/AssetManager.hpp>

namespace NovelTea {

// Static members
std::shared_ptr<sf::Texture> Notification::m_texture = nullptr;
sf::Vector2f Notification::m_spawnPosition;
std::vector<std::unique_ptr<Notification>> Notification::notifications;
float Notification::m_spawnOffsetY = 0.f;
float Notification::m_durationDefault = 5.f;
float Notification::m_spacing = 4.f;


Notification::Notification()
: m_markForDelete(false)
{
	if (!m_texture)
		m_texture = AssetManager<sf::Texture>::get("images/notification.9.png");
	setTexture(m_texture.get());
	getText().setCharacterSize(22);
}


Notification::~Notification()
{
	m_tweenManager.killAll();
}


void Notification::update(float delta)
{
	for (auto i = notifications.begin(); i != notifications.end();)
	{
		if ((*i)->m_markForDelete)
		{
			float offsetY = (*i)->getSize().y + m_spacing;
			m_spawnOffsetY += offsetY;
			for (auto j = i+1; j != notifications.end(); j++)
			{
				auto notification = j->get();
				TweenEngine::Tween::to(*notification, POSITION_Y, 0.3f)
					.target(notification->getPosition().y + offsetY)
					.start(notification->m_tweenManager);
			}
			notifications.erase(i);
		} else {
			++i;
		}
	}
	for (auto& notification : notifications)
		notification->m_tweenManager.update(delta);
}


void Notification::spawn(const std::string &message)
{
	auto notification = new Notification;
	notification->setString(message);
	notification->setPosition(round((m_spawnPosition.x - notification->getSize().x) / 2.f),
							  round(m_spawnPosition.y - notification->getSize().y + m_spawnOffsetY));
	m_spawnOffsetY -= m_spacing + notification->getSize().y;
	notification->animate();
	notifications.emplace_back(notification);
}

void Notification::setScreenSize(const sf::Vector2f &size)
{
	m_spawnPosition = size;
}


void Notification::animate(float duration)
{
	setColor(sf::Color(255, 255, 255, 0));
	setTextColor(sf::Color(60, 60, 60, 0));
	TweenEngine::Tween::from(*this, POSITION_Y, 0.5f).target(getPosition().y - 15.f).start(m_tweenManager);
	TweenEngine::Tween::to(*this, COLOR_ALPHA, 0.5f).target(225.f).start(m_tweenManager);
	TweenEngine::Tween::to(*this, TEXTCOLOR_ALPHA, 0.5f).target(255.f).start(m_tweenManager);

	// Fade out and mark notification for deletion in update()
	TweenEngine::Tween::to(*this, COLOR_ALPHA, 0.5f).target(0.f).delay(duration).start(m_tweenManager);
	TweenEngine::Tween::to(*this, TEXTCOLOR_ALPHA, 0.5f)
		.target(0.f)
		.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*) {
			m_markForDelete = true;
		})
		.delay(duration).start(m_tweenManager);
}

} // namespace NovelTea
