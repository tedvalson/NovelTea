#include <NovelTea/Timer.hpp>
#include <NovelTea/Game.hpp>

namespace NovelTea
{

Timer::Timer(const DukValue &func)
	: m_func(func)
	, m_secondsPassed(0.f)
	, m_repeat(false)
	, m_completed(false)
	, m_duration(1000)
{
}

bool Timer::update(float delta)
{
	if (m_completed)
		return false;

	auto result = false;
	auto duration = 0.001f * m_duration;
	m_secondsPassed += delta;
	while (m_secondsPassed >= duration)
	{
		exec();
		result = true;
		if (!m_repeat)
		{
			m_completed = true;
			break;
		}
		m_secondsPassed -= duration;
	}
	return result;
}

bool Timer::isComplete() const
{
	return m_completed;
}

void Timer::kill()
{
	m_completed = true;
}

void Timer::exec()
{
	ActiveGame->getScriptManager().call<void>(m_func);
}



TimerManager::TimerManager()
{

}

void TimerManager::reset()
{
	m_timers.clear();
}

bool TimerManager::update(float delta)
{
	auto result = false;

	// Perform two separate for loops in case some
	// timers create more timers.
	for (auto timer : m_timers)
		if (timer->update(delta))
			result = true;

	for (auto it = m_timers.begin(); it != m_timers.end();)
		if ((*it)->isComplete())
			m_timers.erase(it);
		else
			++it;

	return result;
}

std::shared_ptr<Timer> TimerManager::start(int duration, const DukValue &func)
{
	auto timer = std::make_shared<Timer>(func);
	timer->setDuration(duration);
	m_timers.push_back(timer);
	return timer;
}

std::shared_ptr<Timer> TimerManager::startRepeat(int duration, const DukValue &func)
{
	auto timer = std::make_shared<Timer>(func);
	if (duration > 0)
		timer->setDuration(duration);
	timer->setRepeat(true);
	m_timers.push_back(timer);
	return timer;
}

} // namespace NovelTea
