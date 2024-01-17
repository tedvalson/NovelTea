#ifndef NOVELTEA_TIMER_HPP
#define NOVELTEA_TIMER_HPP

#include <NovelTea/ContextObject.hpp>
#include <NovelTea/Utils.hpp>
#include <dukglue/dukvalue.h>
#include <memory>
#include <vector>

namespace NovelTea
{

class Timer : public ContextObject
{
public:
	Timer(Context *context, const DukValue &func);
	virtual ~Timer();

	virtual bool update(float delta);
	virtual bool isComplete() const;
	virtual void kill();

	virtual void setTimePassed(int timeMs);
	virtual int getTimePassed() const;

	ADD_ACCESSOR(bool, Repeat, m_repeat)
	ADD_ACCESSOR(int, Duration, m_duration)

protected:
	virtual void exec();

	DukValue m_func;
	float m_secondsPassed;
	bool m_repeat;
	bool m_completed;
	int m_duration;
};

class TimerManager : public ContextObject
{
public:
	TimerManager(Context *context);
	virtual ~TimerManager();

	static std::string SubsystemName;

	virtual void reset();
	virtual bool update(float delta);

	virtual std::shared_ptr<Timer> start(int duration, const DukValue &func);
	virtual std::shared_ptr<Timer> startRepeat(int duration, const DukValue &func);

protected:
	std::vector<std::shared_ptr<Timer>> m_timers;
};

} // namespace NovelTea

#endif // NOVELTEA_TIMER_HPP
