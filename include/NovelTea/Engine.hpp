#ifndef NOVELTEA_ENGINE_H
#define NOVELTEA_ENGINE_H

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/View.hpp>
#include <NovelTea/States/StateStack.hpp>
#include <queue>

namespace NovelTea
{

enum class EngineOrientation
{
	Auto,
	Portrait,
	Landscape,
};

struct EngineConfig
{
	EngineConfig();
	size_t width;
	size_t height;
	unsigned fps;
	float fontSizeMultiplier;
	float dpiMultiplier;
	sf::Color backgroundColor;
	StateID initialState;
	EngineOrientation orientation;
	std::string saveDir;
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

	std::shared_ptr<Game> getGame();

//protected:
	void initialize();

	sf::Vector2f mapPixelToCoords(const sf::Vector2i& point) const;

private:
	EngineConfig m_config;
	float m_deltaPerFrame;
	std::unique_ptr<StateStack> m_stateStack;

	sf::View m_view;
	sf::Sprite m_sprite;
	sf::RenderTexture m_renderTexture;
	float m_internalRatio;
	size_t m_width;
	size_t m_height;

	size_t m_lastTime;

	// Shared State context variables
	sj::JSON m_data;
	std::shared_ptr<Game> m_game;
};

} // namespace NovelTea

#endif // NOVELTEA_ENGINE_H
