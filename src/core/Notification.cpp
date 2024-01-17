#include <NovelTea/Notification.hpp>
#include <NovelTea/StringUtils.hpp>
#include <cmath>

namespace NovelTea {

namespace
{
	int durationBaseDefault = 3000;
	int durationPerLetter = 30;
}

std::string NotificationManager::SubsystemName = "Notification";

Notification::Notification(Context *context, const std::string &message)
: m_string(message)
{
}

NotificationManager::NotificationManager(Context *context)
: ContextObject(context)
{
}

NotificationManager::~NotificationManager()
{
}

void NotificationManager::update(float delta)
{
}

void NotificationManager::spawn(const std::string &message, int durationMs)
{
	if (durationMs <= 0)
		durationMs = durationBaseDefault + message.size() * durationPerLetter;
	auto notification = new Notification(getContext(), message);

	// Fade out and mark notification for deletion in update()
	auto duration = 0.001f * durationMs;

	m_notifications.emplace_back(notification);
}

} // namespace NovelTea
