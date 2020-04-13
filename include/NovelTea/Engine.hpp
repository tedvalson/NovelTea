#ifndef NOVELTEA_ENGINE_H
#define NOVELTEA_ENGINE_H

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <NovelTea/States/StateStack.hpp>
#include <queue>

namespace NovelTea
{

struct EngineConfig
{
	unsigned int width = 1024;
	unsigned int height = 720;
	unsigned short fps = 30;
	StateID initialState;
};

class Engine
{
public:
	Engine(EngineConfig config);
	void run();
	bool isRunning() const;
	void resize(unsigned int width, unsigned int height);
	void render(sf::RenderTarget &target);
	void update(float deltaSeconds);
	void processEvent(const sf::Event &event);
	void *processData(void *data);

//protected:
	void initialize();

private:
	EngineConfig _config;
	float _deltaPerFrame;
	StateStack _stateStack;

	// Shared State context variables
	std::vector<char*> _data;
	sf::String _text;
};

} // namespace NovelTea

#endif // NOVELTEA_ENGINE_H
