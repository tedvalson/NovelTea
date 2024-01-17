#include <NovelTea/Event.hpp>
#include <NovelTea/Err.hpp>

namespace NovelTea {

std::string EventManager::SubsystemName = "EventManager";

EventManager::EventManager(Context *context)
: ContextObject(context)
{
}

EventManager::~EventManager()
{
}

void EventManager::listen(EventFunc func)
{
	listen(Event::All, func);
}

void EventManager::listen(Event::EventType type, EventFunc func)
{
	m_listeners[type].push_back(func);
}

bool EventManager::trigger(const Event &event)
{
	bool result = true;
	for (auto listener : m_listeners[Event::All])
		result &= listener(event);
	if (event.type() != Event::All)
		for (auto listener : m_listeners[event.type()])
			result &= listener(event);
	return result;
}

Event::Event(Event::EventType type)
: m_type(type)
{
	if (type == Event::All)
		err() << "Type 'Event::All' is invalid for Event instances." << std::endl;
	if (type == Event::EventCount)
		err() << "Type 'Event::EventCount' is invalid for Event instances." << std::endl;
}

} // namespace NovelTea
