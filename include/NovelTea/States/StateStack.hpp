#ifndef NOVELTEA_STATESTACK_HPP
#define NOVELTEA_STATESTACK_HPP

#include "State.hpp"
#include "StateIdentifiers.hpp"
#include <vector>
#include <utility>
#include <functional>
#include <map>


namespace NovelTea {

class StateStack : public ContextObject
{
public:
	enum Action {
		Push,
		Pop,
		Clear,
		ClearUnder,
	};

public:
	explicit StateStack(Context* context);

	template <typename T>
	void registerState(StateID stateID);

	void update(float delta);
	void render(sf::RenderTarget &target);
	void resize(const sf::Vector2f &size);
	void processEvent(const sf::Event& event);

	void *processData(void *data);

	void pushState(StateID stateID, bool renderAlone = false, StateCallback callback = nullptr);
	void popState();
	void clearStates();
	void clearStatesUnder();

	bool isEmpty() const;


private:
	State::Ptr createState(StateID stateID, StateCallback callback);
	void       applyPendingChanges();


private:
	struct PendingChange
	{
		explicit PendingChange(Action action, StateID stateID = StateID::None, bool renderAlone = false, StateCallback = nullptr);

		Action action;
		StateID stateID;
		bool renderAlone;
		StateCallback callback;
	};

	struct StateStackItem
	{
		StateID id;
		State::Ptr pointer;
		bool renderAlone;
		bool renderEnabled;
	};

	void updateRenderConfig();

private:
	std::vector<StateStackItem>    m_stack;
	std::vector<PendingChange> m_pendingList;

	std::map<StateID, std::function<State::Ptr(StateCallback)>> m_factories;
};


template <typename T>
void StateStack::registerState(StateID stateID)
{
	m_factories[stateID] = [this] (StateCallback callback)
	{
		return State::Ptr(new T(*this, *getContext(), callback));
	};
}

} // namespace NovelTea

#endif // NOVELTEA_STATESTACK_HPP
