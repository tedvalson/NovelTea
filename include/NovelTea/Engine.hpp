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
	size_t width = 1024;
	size_t height = 720;
	unsigned short fps = 30;
	StateID initialState;
};

class Engine
{
public:
	Engine(EngineConfig config);
	void run();
	bool isRunning() const;
	void resize(size_t width, size_t height);
	void render(sf::RenderTarget &target);
	void update();
	void update(float deltaSeconds);
	void processEvent(const sf::Event &event);
	void *processData(void *data);

	static size_t getSystemTimeMs();

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
	size_t _width;
	size_t _height;

	size_t _lastTime;

	// Shared State context variables
	std::vector<char*> _data;
	sf::String _text;
};

} // namespace NovelTea

#endif // NOVELTEA_ENGINE_H
