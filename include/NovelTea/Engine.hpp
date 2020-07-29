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
	EngineConfig m_config;
	float m_deltaPerFrame;
	StateStack m_stateStack;

	sf::View m_view;
	sf::RectangleShape m_bg;
	float m_internalRatio;
	size_t m_width;
	size_t m_height;

	size_t m_lastTime;

	// Shared State context variables
	sj::JSON m_data;
	ScriptManager m_scriptManager;
};

} // namespace NovelTea

#endif // NOVELTEA_ENGINE_H
