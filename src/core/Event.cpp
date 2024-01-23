#include <NovelTea/Event.hpp>
#include <NovelTea/Err.hpp>
#include <algorithm>

namespace NovelTea {

EventManager::EventManager(Context *context)
: Subsystem(context)
, m_listenerId(0)
{
}

EventManager::~EventManager()
{
}

void EventManager::update(float delta)
{
	// Make copy since triggered events can queue more events
	auto events = std::move(m_eventQueue);
	m_eventQueue.clear();
	for (auto &event : events) {
		trigger(event);
	}
}

int EventManager::listen(EventFunc func)
{
	return listen(Event::All, func);
}

int EventManager::listen(int type, EventFunc func)
{
	int id = m_listenerId++;
	m_listeners[type].push_back({id, func});
	return id;
}

void EventManager::remove(int listenerId)
{
	for (auto& listenerType : m_listeners) {
		auto& listeners = listenerType.second;
		auto it = std::find_if(listeners.begin(), listeners.end(), [listenerId](EventListener& x){
			return x.id == listenerId;
		});
		if (it != listeners.end()) {
			listeners.erase(it);
			return;
		}
	}
}

bool EventManager::trigger(const EventPtr &event)
{
	bool result = true;
	for (auto& listener : m_listeners[Event::All])
		result &= listener.func(event);
	if (event->type() != Event::All)
		for (auto& listener : m_listeners[event->type()])
			result &= listener.func(event);
	return result;
}

bool EventManager::trigger(Event &&event)
{
	return trigger(std::make_shared<Event>(std::move(event)));
}

bool EventManager::trigger(int eventType)
{
	return trigger({eventType});
}

void EventManager::push(Event &&event)
{
	push<Event>(event);
}

void EventManager::push(int eventType)
{
	push(Event{eventType});
}

Event::Event(int type)
: m_type(type)
{
	if (type == Event::All)
		err() << "Type 'Event::All' is invalid for Event instances." << std::endl;
	if (type == Event::EventCount)
		err() << "Type 'Event::EventCount' is invalid for Event instances." << std::endl;
}

Event::Event(int type, int num)
: Event(type)
{
	number = num;
}

Event::Event(int type, const std::string &str)
: Event(type)
{
	text = str;
}

} // namespace NovelTea
