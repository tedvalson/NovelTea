#ifndef NOVELTEA_NOTIFICATION_SFML_HPP
#define NOVELTEA_NOTIFICATION_SFML_HPP

#include <NovelTea/Notification.hpp>
#include <TweenEngine/TweenManager.h>
#include <NovelTea/GUI/Button.hpp>
#include <memory>

namespace NovelTea {

class NotificationSFML : public Button {
public:
	NotificationSFML(Context *context, const std::string &message);

	void setFontSizeMultiplier(float multiplier);
	void setScreenSize(const sf::Vector2f &size);
	void markForDelete();
	bool isMarkedForDelete() const;

private:
	sf::String m_string;
	sf::Vector2f m_screenSize;
	bool m_markForDelete;
};

class NotificationManagerSFML : public NotificationManager, public sf::Drawable {
public:
	NotificationManagerSFML(Context *context);

	void spawn(const std::string &message, int durationMs = 0) override;
	void update(float delta) override;

	void setScreenSize(const sf::Vector2f &size);
	void setFontSizeMultiplier(float multiplier);

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void repositionItems();
	void killTweens(TweenEngine::Tweenable *notification);

private:
	sf::Vector2f m_spawnPosition;
	sf::Vector2f m_screenSize;
	float m_fontSizeMultiplier;
	float m_spawnOffsetY;
	std::vector<std::unique_ptr<NotificationSFML>> m_notifications;
	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_NOTIFICATION_SFML_HPP
