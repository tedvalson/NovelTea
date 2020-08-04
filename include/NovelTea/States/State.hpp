#ifndef NOVELTEA_STATE_HPP
#define NOVELTEA_STATE_HPP

#include "StateIdentifiers.hpp"
#include <SFML/Window/Event.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/json.hpp>
#include <functional>
#include <memory>
#include <vector>

#define GGame getContext().game
#define ScriptMan getContext().game.getScriptManager()

namespace sf
{
	class RenderTarget;
}

namespace NovelTea {

struct EngineConfig;
class StateStack;

typedef std::function<bool(void*)> StateCallback;

class State
{
public:
	typedef std::unique_ptr<State> Ptr;

	struct Context
	{
		Context(EngineConfig& config, Game& game, sj::JSON& data);
		EngineConfig& config;
		Game& game;
		sj::JSON& data;
	};

	State(StateStack& stack, Context& context, StateCallback callback);
	virtual ~State();

	virtual void render(sf::RenderTarget &target) = 0;
	virtual bool update(float delta) = 0;
	virtual bool processEvent(const sf::Event& event) = 0;

	virtual void *processData(void *data);

	void requestStackPush(StateID stateID, bool renderAlone = false, StateCallback callback = nullptr);
	void requestStackPop();
	void requestStackClear();
	void requestStackClearUnder();
	bool runCallback(void *data);

	Context getContext() const;

private:
	StateStack*  m_stack;
	Context      m_context;
	StateCallback m_callback;
};

} // namespace NovelTea

#endif // NOVELTEA_STATE_HPP
