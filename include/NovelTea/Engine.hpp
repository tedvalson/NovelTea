#ifndef NOVELTEA_ENGINE_H
#define NOVELTEA_ENGINE_H

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/View.hpp>
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
	void update();
	void update(float deltaSeconds);
	void processEvent(const sf::Event &event);
	void *processData(void *data);

	static unsigned int getSystemTimeMs();

//protected:
	void initialize();

	sf::Vector2f mapPixelToCoords(const sf::Vector2i& point) const;

private:
	EngineConfig _config;
	float _deltaPerFrame;
	StateStack _stateStack;

	sf::View _view;
	sf::RectangleShape _bg;
	float _internalRatio;
	unsigned int _width;
	unsigned int _height;

	unsigned int _lastTime;

	// Shared State context variables
	std::vector<char*> _data;
	sf::String _text;
};

} // namespace NovelTea

#endif // NOVELTEA_ENGINE_H
