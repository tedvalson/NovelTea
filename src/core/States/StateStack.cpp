#include <NovelTea/States/StateStack.hpp>
#include <cassert>


namespace NovelTea {

StateStack::StateStack(State::Context context)
: m_stack()
, m_pendingList()
, m_context(context)
, m_factories()
{
}

void StateStack::update(float delta)
{
	// Iterate from top to bottom, stop as soon as update() returns false
	for (auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
	{
		if (!itr->pointer->update(delta))
			break;
	}

	applyPendingChanges();
}

void StateStack::render(sf::RenderTarget &target)
{
	// Draw all active states from bottom to top
	for(const StateStackItem& state : m_stack)
		if (state.renderEnabled)
			state.pointer->render(target);
}

void StateStack::processEvent(const sf::Event& event)
{
	// Iterate from top to bottom, stop as soon as handleEvent() returns false
	for (auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
	{
		if (!itr->pointer->processEvent(event))
			break;
	}

	applyPendingChanges();
}

void *StateStack::processData(void *data)
{
	void *ret;
	for (auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
	{
		if (ret = itr->pointer->processData(data))
			return ret;
	}
	return nullptr;
}

void StateStack::pushState(StateID stateID, bool renderAlone, StateCallback callback)
{
	m_pendingList.push_back(PendingChange(Push, stateID, renderAlone, callback));
}

void StateStack::popState()
{
	m_pendingList.push_back(PendingChange(Pop));
}

void StateStack::clearStates()
{
	m_pendingList.push_back(PendingChange(Clear));
}

void StateStack::clearStatesUnder()
{
	m_pendingList.push_back(PendingChange(ClearUnder));
}

bool StateStack::isEmpty() const
{
	return m_stack.empty();
}

State::Ptr StateStack::createState(StateID stateID, StateCallback callback)
{
	auto found = m_factories.find(stateID);
	assert(found != m_factories.end());

	return found->second(callback);
}

void StateStack::applyPendingChanges()
{
	for(const PendingChange& change : m_pendingList)
	{
		switch (change.action)
		{
			case Push:
				m_stack.push_back({change.stateID, createState(change.stateID, change.callback), change.renderAlone, true});
				updateRenderConfig();
				break;

			case Pop:
				m_stack.pop_back();
				updateRenderConfig();
				break;

			case Clear:
				m_stack.clear();
				break;

			case ClearUnder:
				if (m_stack.size() > 1)
					m_stack.erase(m_stack.begin(), m_stack.end() - 1);
				break;
		}
	}

	m_pendingList.clear();
}

void StateStack::updateRenderConfig()
{
	bool renderEnabled = true;
	for (auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
	{
		itr->renderEnabled = renderEnabled;
		if (itr->renderAlone)
			renderEnabled = false;
	}
}


	StateStack::PendingChange::PendingChange(Action action, StateID stateID, bool renderAlone, StateCallback callback)
: action(action)
, stateID(stateID)
, renderAlone(renderAlone)
, callback(callback)
{
}

} // namespace NovelTea
