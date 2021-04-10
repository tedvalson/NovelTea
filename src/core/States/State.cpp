#include <NovelTea/States/State.hpp>
#include <NovelTea/States/StateStack.hpp>


namespace NovelTea {

State::Context::Context(EngineConfig& config, Game& game, sj::JSON& data)
: config(config)
, game(game)
, data(data)
{
}

State::State(StateStack& stack, Context& context, StateCallback callback)
: m_stack(&stack)
, m_context(context)
, m_callback(callback)
{
}

State::~State()
{
}

void *State::processData(void *)
{
	return nullptr;
}

void State::requestStackPush(StateID stateID, bool renderAlone, StateCallback callback)
{
	m_stack->pushState(stateID, renderAlone, callback);
}

void State::requestStackPop()
{
	m_stack->popState();
}

void State::requestStackClear()
{
	m_stack->clearStates();
}

void State::requestStackClearUnder()
{
	m_stack->clearStatesUnder();
}

bool State::runCallback(void *data)
{
	if (m_callback)
		return m_callback(data);
	return false;
}

void State::close(float duration, StateID stateId)
{
	hide(duration, ALPHA, [this, stateId](){
		requestStackPop();
		if (stateId != StateID::None)
			requestStackPush(stateId);
	});
}

void State::setAlpha(float alpha)
{
	m_alpha = alpha;
}

float State::getAlpha() const
{
	return m_alpha;
}

State::Context State::getContext() const
{
	return m_context;
}

StateStack &State::getStack() const
{
	return *m_stack;
}

} // namespace NovelTea
