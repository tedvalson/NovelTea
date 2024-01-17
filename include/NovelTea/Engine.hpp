#ifndef NOVELTEA_ENGINE_H
#define NOVELTEA_ENGINE_H

#include <NovelTea/ContextObject.hpp>
#include <NovelTea/json.hpp>
#include <NovelTea/Event.hpp>
#include <NovelTea/Utils.hpp>
#include <queue>

namespace NovelTea
{

class Engine : public ContextObject
{
public:
	Engine(Context* context);
	virtual ~Engine();
	int run();
	bool initialize();
	virtual bool isRunning() const;
	virtual void update();
	virtual void update(float deltaSeconds);

	void listen(EventFunc func);
	void listen(Event::EventType type, EventFunc func);

	static int64_t getSystemTimeMs();

	ADD_ACCESSOR(bool, FramerateLocked, m_framerateLocked)

protected:
	bool m_framerateLocked;
	float m_deltaPerFrame;
	int64_t m_lastTime;
};

} // namespace NovelTea

#endif // NOVELTEA_ENGINE_H
