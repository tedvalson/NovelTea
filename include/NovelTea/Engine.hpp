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

class Engine : public ContextObject
{
public:
	Engine(Context* context);
	int run();
	void initialize();
	bool isRunning() const;
	void resize(size_t width, size_t height);
	void render(sf::RenderTarget &target);
	void update();
	void update(float deltaSeconds);
	void processEvent(const sf::Event &event);
	void *processData(void *data);

	static size_t getSystemTimeMs();

	void setFramerateLocked(bool locked);
	bool getFramerateLocked() const;

	sf::Vector2f mapPixelToCoords(const sf::Vector2i& point) const;

private:
	bool m_framerateLocked;
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
};

} // namespace NovelTea

#endif // NOVELTEA_ENGINE_H
