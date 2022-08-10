#ifndef NOVELTEA_NOTIFICATION_HPP
#define NOVELTEA_NOTIFICATION_HPP

#include <NovelTea/ContextObject.hpp>
#include <TweenEngine/TweenManager.h>
#include <NovelTea/GUI/Button.hpp>
#include <memory>

namespace NovelTea {

class Notification : public Button {
public:
	Notification(Context *context, const std::string &message);

	void setFontSizeMultiplier(float multiplier);
	void setScreenSize(const sf::Vector2f &size);
	void markForDelete();
	bool isMarkedForDelete() const;

private:
	sf::String m_string;
	sf::Vector2f m_screenSize;
	bool m_markForDelete;
};

class NotificationManager : public ContextObject, public sf::Drawable {
public:
	NotificationManager(Context *context);

	void spawn(const std::string &message, int durationMs = 0);
	void update(float delta);

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
	std::vector<std::unique_ptr<Notification>> m_notifications;
	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_NOTIFICATION_HPP
