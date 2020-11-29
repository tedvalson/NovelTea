#ifndef NOVELTEA_TIMER_HPP
#define NOVELTEA_TIMER_HPP

#include <NovelTea/Utils.hpp>
#include <dukglue/dukvalue.h>
#include <memory>
#include <vector>

namespace NovelTea
{

class Timer
{
public:
	Timer(const DukValue &func);

	bool update(float delta);
	bool isComplete() const;
	void kill();

	void setTimePassed(int timeMs);
	int getTimePassed() const;

	ADD_ACCESSOR(bool, Repeat, m_repeat)
	ADD_ACCESSOR(int, Duration, m_duration)

protected:
	void exec();

private:
	DukValue m_func;
	float m_secondsPassed;
	bool m_repeat;
	bool m_completed;
	int m_duration;
};

class TimerManager
{
public:
	TimerManager();
	void reset();
	bool update(float delta);

	std::shared_ptr<Timer> start(int duration, const DukValue &func);
	std::shared_ptr<Timer> startRepeat(int duration, const DukValue &func);

private:
	std::vector<std::shared_ptr<Timer>> m_timers;
};

} // namespace NovelTea

#endif // NOVELTEA_TIMER_HPP
