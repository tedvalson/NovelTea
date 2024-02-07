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

enum class TextLogType;

struct Event
{
	enum EventType {
		All,             // Special type used for event listener
		TimerCompleted,  // Timer executed its callback
		GameLoaded,
		GameSaving,      // Game save prep, load SaveData with whatever you want
		GameSaved,
		Notification,
		TextLogged,

		EventCount,
		CustomEvent = 1000, // For users to extend with new event types
	};

	struct NotificationEvent {
		std::string text;
		int durationMs;
	};

	struct TextLogEvent {
		std::string text;
		TextLogType type;
	};

	int intVal;
	float floatVal;
	std::string text;
	std::shared_ptr<ContextObject> object;
	sj::JSON json;

	union {
		NotificationEvent *notification;
		TextLogEvent *textlog;
		void *ptr;
	};

	Event(int type);
	Event(int type, int value);
	Event(int type, float value);
	Event(int type, const std::string &text);
	Event(int type, const sj::JSON &json);
	Event(int type, std::shared_ptr<ContextObject> object);
	Event(int type, void *pointer);
	Event(Event&& event);
	virtual ~Event();

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

	static std::string name() { return "EventManager"; }

	void update(float delta) override;

	int listen(EventFunc func);
	int listen(int type, EventFunc func);
	void remove(int listenerId);
	bool trigger(const EventPtr &event);
	bool trigger(Event &&event);
	bool trigger(int eventType);
	void push(const EventPtr &event);
	void push(Event &&event);
	void push(int eventType);

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
