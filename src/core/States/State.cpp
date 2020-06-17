#include <NovelTea/States/State.hpp>
#include <NovelTea/States/StateStack.hpp>


namespace NovelTea {

State::Context::Context(EngineConfig& config, ScriptManager& scriptManager, nlohmann::json& data)
: config(config)
, scriptManager(scriptManager)
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

State::Context State::getContext() const
{
	return m_context;
}

} // namespace NovelTea
