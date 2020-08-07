#ifndef NOVELTEA_NOTIFICATION_HPP
#define NOVELTEA_NOTIFICATION_HPP

#include <TweenEngine/TweenManager.h>
#include <NovelTea/GUI/Button.hpp>
#include <memory>

#define NOTIFICATION_SPACING 0.f
#define NOTIFICATION_DURATION 5.f

namespace NovelTea {

class Notification: public Button {
public:
	Notification();
	~Notification();

	void animate();

	static void update(float delta);
	static void spawn(const std::string &message);
	static void setScreenSize(const sf::Vector2f &size);

	static std::vector<std::unique_ptr<Notification>> notifications;

private:
	static std::shared_ptr<sf::Texture> m_texture;
	static sf::Vector2f m_spawnPosition;

	bool m_markForDelete;
	TweenEngine::TweenManager m_tweenManager;
	float m_destinationY;
};

} // namespace NovelTea

#endif // NOVELTEA_NOTIFICATION_HPP
