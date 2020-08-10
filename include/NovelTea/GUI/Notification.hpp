#ifndef NOVELTEA_NOTIFICATION_HPP
#define NOVELTEA_NOTIFICATION_HPP

#include <TweenEngine/TweenManager.h>
#include <NovelTea/GUI/Button.hpp>
#include <memory>

namespace NovelTea {

class Notification: public Button {
public:
	Notification();
	~Notification();

	void animate(float duration = m_durationDefault);

	static void update(float delta);
	static void spawn(const std::string &message);
	static void setScreenSize(const sf::Vector2f &size);

	static std::vector<std::unique_ptr<Notification>> notifications;

private:
	static std::shared_ptr<sf::Texture> m_texture;
	static sf::Vector2f m_spawnPosition;
	static float m_spawnOffsetY;

	static float m_spacing;
	static float m_durationDefault;

	bool m_markForDelete;
	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_NOTIFICATION_HPP
