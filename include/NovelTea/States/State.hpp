#ifndef NOVELTEA_STATE_HPP
#define NOVELTEA_STATE_HPP

#include "StateIdentifiers.hpp"
#include <NovelTea/ContextObject.hpp>
#include <SFML/Window/Event.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/json.hpp>
#include <NovelTea/GUI/Hideable.hpp>
#include <functional>
#include <memory>
#include <vector>

namespace sf
{
	class RenderTarget;
}

namespace NovelTea {

struct EngineConfig;
class StateStack;

typedef std::function<bool(void*)> StateCallback;

class State : public ContextObject, public Hideable
{
public:
	typedef std::unique_ptr<State> Ptr;

	State(StateStack& stack, Context& context, StateCallback callback);
	virtual ~State();

	virtual void render(sf::RenderTarget &target) = 0;
	virtual void resize(const sf::Vector2f &size) = 0;
	virtual bool processEvent(const sf::Event& event) = 0;

	virtual void setAlpha(float alpha) override;
	float getAlpha() const override;

	virtual void *processData(void *data);

	void requestStackPush(StateID stateID, bool renderAlone = false, StateCallback callback = nullptr);
	void requestStackPop();
	void requestStackClear();
	void requestStackClearUnder();
	bool runCallback(void *data);
	void close(float duration = 0.5f, StateID stateId = StateID::None);

	StateStack &getStack() const;

private:
	float        m_alpha;
	StateStack*  m_stack;
	StateCallback m_callback;
};

} // namespace NovelTea

#endif // NOVELTEA_STATE_HPP
