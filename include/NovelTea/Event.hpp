#ifndef NOVELTEA_EVENT_HPP
#define NOVELTEA_EVENT_HPP

#include <NovelTea/Subsystem.hpp>
#include <NovelTea/json.hpp>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <memory>

namespace NovelTea
{

struct Event
{
	enum EventType {
		All,             // Special type used for event listener
		TimerCompleted,  // Timer executed its callback

		EventCount,      // Special type
	};

	std::string text;

	union {
		int number;
	};

	Event(int type);
	Event(int type, int number);
	Event(int type, const std::string &text);
	virtual ~Event() = default;

	int type() const { return m_type; }

private:
	int m_type;
};

using EventPtr = std::shared_ptr<Event>;
using EventFunc = std::function<bool(const EventPtr &event)>;

struct EventListener {
	int id;
	EventFunc func;
};

class EventManager : public Subsystem
{
public:
	EventManager(Context* context);
	virtual ~EventManager();

	static std::string name() { return "EventManager"; }

	virtual void update(float delta) override;

	virtual int listen(EventFunc func);
	virtual int listen(int type, EventFunc func);
	virtual void remove(int listenerId);
	virtual bool trigger(const EventPtr &event);
	virtual bool trigger(Event &&event);
	virtual bool trigger(int eventType);
	virtual void push(Event &&event);
	virtual void push(int eventType);

	template <class T, class = typename std::enable_if<!std::is_enum<T>::value>::type>
	void push(T&&v) {
		m_eventQueue.emplace_back(new typename std::remove_reference<T>::type{std::move(v)});
	}

	template <typename T, typename... Args>
	void push(Args&&... args) {
		m_eventQueue.emplace_back(new T(std::forward<Args>(args)...));
	}

private:
	int m_listenerId;
	std::vector<EventPtr> m_eventQueue;
	std::map<int, std::vector<EventListener>> m_listeners;
};

} // namespace NovelTea

#endif // NOVELTEA_EVENT_HPP
