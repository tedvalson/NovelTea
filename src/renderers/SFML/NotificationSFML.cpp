#include <NovelTea/SFML/NotificationSFML.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/SFML/Utils.hpp>
#include <TweenEngine/Tween.h>
#include <cmath>

namespace NovelTea {

namespace
{
	float spacing = 2.f;
	int durationBaseDefault = 3000;
	int durationPerLetter = 30;
}

NotificationSFML::NotificationSFML(Context *context, const std::string &message)
: Button(context)
, m_string(message)
, m_markForDelete(false)
{
	setTexture(AssetManager<sf::Texture>::get("images/notification.9.png").get());
	setString(message);
	setCentered(true);
}

void NotificationSFML::setFontSizeMultiplier(float multiplier)
{
	getText().setCharacterSize(multiplier * 20);
	setScreenSize(m_screenSize);
}

void NotificationSFML::setScreenSize(const sf::Vector2f &size)
{
	m_screenSize = size;
	auto width = size.x;

	auto m = size.x < size.y ? 0.08f * size.x : 0.08f * size.y;
	setExtraPadding(sf::FloatRect(m * 8.f, m * 0.2f, m * 5.f, m * 10.f));

	auto &text = getText();
	setString(m_string);
	wrapText(text, width);
	setContentSize(text.getLocalBounds().width, text.getLocalBounds().height);
}

void NotificationSFML::markForDelete()
{
	m_markForDelete = true;
}

bool NotificationSFML::isMarkedForDelete() const
{
	return m_markForDelete;
}

NotificationManagerSFML::NotificationManagerSFML(Context *context)
: NotificationManager(context)
, m_fontSizeMultiplier(1.f)
, m_spawnOffsetY(0.f)
{
}


void NotificationManagerSFML::update(float delta)
{
	for (auto i = m_notifications.begin(); i != m_notifications.end();)
	{
		if ((*i)->isMarkedForDelete())
		{
			repositionItems();
			killTweens((*i).get());
			m_notifications.erase(i);
		} else {
			++i;
		}
	}

	m_tweenManager.update(delta);
}

void NotificationManagerSFML::setScreenSize(const sf::Vector2f &size)
{
	m_screenSize = size;
	m_spawnPosition.x = size.x;
	m_spawnPosition.y = 7.f;
	for (auto &notification : m_notifications)
		notification->setScreenSize(size);
	repositionItems();
}

void NotificationManagerSFML::setFontSizeMultiplier(float multiplier)
{
	if (m_fontSizeMultiplier == multiplier)
		return;
	m_fontSizeMultiplier = multiplier;
	for (auto &notification : m_notifications)
		notification->setFontSizeMultiplier(multiplier);
	repositionItems();
}

void NotificationManagerSFML::spawn(const std::string &message, int durationMs)
{
	if (durationMs <= 0)
		durationMs = durationBaseDefault + message.size() * durationPerLetter;
	auto notification = new NotificationSFML(getContext(), message);

	notification->setColor(sf::Color(255, 255, 255, 0));
	notification->setTextColor(sf::Color(60, 60, 60, 0));

	notification->setScreenSize(m_screenSize);
	notification->setFontSizeMultiplier(m_fontSizeMultiplier);

	notification->setPosition(round((m_spawnPosition.x - notification->getSize().x) / 2.f),
							  round(m_spawnPosition.y + m_spawnOffsetY));
	m_spawnOffsetY += spacing + notification->getSize().y;

	TweenEngine::Tween::from(*notification, NotificationSFML::POSITION_Y, 0.5f).target(notification->getPosition().y - 15.f).start(m_tweenManager);
	TweenEngine::Tween::to(*notification, NotificationSFML::COLOR_ALPHA, 0.5f).target(225.f).start(m_tweenManager);
	TweenEngine::Tween::to(*notification, NotificationSFML::TEXTCOLOR_ALPHA, 0.5f).target(255.f).start(m_tweenManager);

	// Fade out and mark notification for deletion in update()
	auto duration = 0.001f * durationMs;
	TweenEngine::Tween::to(*notification, NotificationSFML::COLOR_ALPHA, 0.5f).target(0.f).delay(duration).start(m_tweenManager);
	TweenEngine::Tween::to(*notification, NotificationSFML::TEXTCOLOR_ALPHA, 0.5f)
		.target(0.f)
		.setCallback(TweenEngine::TweenCallback::COMPLETE, [notification](TweenEngine::BaseTween*) {
			notification->markForDelete();
		})
	.delay(duration).start(m_tweenManager);

	m_notifications.emplace_back(notification);
}

void NotificationManagerSFML::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	for (auto &notification : m_notifications)
		target.draw(*notification, states);
}

void NotificationManagerSFML::repositionItems()
{
	auto posY = 0.f;
	for (auto &notification : m_notifications)
	{
		if (notification->isMarkedForDelete())
			continue;

		auto x = round((m_spawnPosition.x - notification->getSize().x) / 2.f);
		auto y = round(m_spawnPosition.y + posY);

		TweenEngine::Tween::to(*notification, NotificationSFML::POSITION_XY, 0.5f)
			.target(x, y)
			.start(m_tweenManager);

		posY += spacing + notification->getSize().y;
	}

	m_spawnOffsetY = posY;
}

void NotificationManagerSFML::killTweens(TweenEngine::Tweenable *notification)
{
	for (auto &object : m_tweenManager.getObjects())
	{
		auto tween = static_cast<TweenEngine::Tween*>(object);
		if (tween->getTarget() == notification)
			tween->kill();
	}
}

} // namespace NovelTea
