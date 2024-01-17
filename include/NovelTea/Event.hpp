#ifndef NOVELTEA_EVENT_HPP
#define NOVELTEA_EVENT_HPP

#include <NovelTea/ContextObject.hpp>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace NovelTea
{

struct Event
{
	struct TouchEvent
	{
		int x;
	};

	enum EventType
	{
		All,
		Touch,

		EventCount,
	};

	union
	{
		TouchEvent touch;
	};

	Event(EventType type);

	EventType type() const { return m_type; }

private:
	EventType m_type;
};

using EventFunc = std::function<bool(const Event &event)>;

class EventManager : public ContextObject
{
public:
	EventManager(Context* context);
	virtual ~EventManager();

	static std::string SubsystemName;
	
	virtual void listen(EventFunc func);
	virtual void listen(Event::EventType type, EventFunc func);
	virtual bool trigger(const Event &event);
	
private:
	std::map<Event::EventType, std::vector<EventFunc>> m_listeners;
};

} // namespace NovelTea

#endif // NOVELTEA_EVENT_HPP
