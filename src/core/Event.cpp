#include <NovelTea/Event.hpp>
#include <NovelTea/Err.hpp>
#include <algorithm>

namespace NovelTea {

EventManager::EventManager(Context *context)
: Subsystem(context)
, m_listenerId(0)
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
	push<Event>(std::move(event));
}

void EventManager::push(int eventType)
{
	push(Event{eventType});
}

Event::Event(int type)
: object(nullptr)
, m_type(type)
{
	switch(type) {
	case All:
		err() << "Type 'Event::All' is invalid for Event instances." << std::endl; break;
	case EventCount:
		err() << "Type 'Event::EventCount' is invalid for Event instances." << std::endl; break;
	case TextLogged:
		textlog = new TextLogEvent; break;
	case Notification:
		notification = new NotificationEvent; break;
	}
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

Event::Event(int type, std::shared_ptr<ContextObject> obj)
: Event(type)
{
	object = obj;
}

Event::Event(Event &&event)
: number(event.number)
, text(event.text)
, object(event.object)
, ptr(event.ptr)
, m_type(event.m_type)
{
	event.m_type = -1; // Prevent deleting pointer on event's destructor
}

Event::~Event()
{
	switch(m_type) {
	case TextLogged:
		delete textlog; break;
	case Notification:
		delete notification; break;
	}
}

} // namespace NovelTea
