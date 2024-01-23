#ifndef NOVELTEA_NOTIFICATION_HPP
#define NOVELTEA_NOTIFICATION_HPP

#include <NovelTea/Subsystem.hpp>
#include <vector>
#include <memory>

namespace NovelTea {

class Notification {
public:
	Notification(Context *context, const std::string &message);

private:
	std::string m_string;
};

class NotificationManager : public Subsystem {
public:
	NotificationManager(Context *context);
	virtual ~NotificationManager();

	static std::string name() { return "Notification"; }

	virtual void spawn(const std::string &message, int durationMs = 0);
	virtual void update(float delta);

protected:
	std::vector<std::unique_ptr<Notification>> m_notifications;
};

} // namespace NovelTea

#endif // NOVELTEA_NOTIFICATION_HPP
