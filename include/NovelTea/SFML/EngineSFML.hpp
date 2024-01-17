#ifndef NOVELTEA_ENGINE_SFML_H
#define NOVELTEA_ENGINE_SFML_H

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/View.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/States/StateStack.hpp>
#include <queue>

namespace NovelTea
{

class EngineSFML : public Engine
{
public:
	EngineSFML(Context* context);
	virtual ~EngineSFML();
	int run();
	bool initialize();
	void resize(size_t width, size_t height);
	void render(sf::RenderTarget &target);
	void update(float deltaSeconds) override;
	void processEvent(const sf::Event &event);
	void *processData(void *data);

	sf::Vector2f mapPixelToCoords(const sf::Vector2i& point) const;

private:
	std::unique_ptr<StateStack> m_stateStack;

	sf::View m_view;
	sf::Sprite m_sprite;
	std::shared_ptr<sf::Shader> m_shader; /// Shader used for post-processing
	sf::RenderTexture m_renderTexture;
	float m_internalRatio;
	size_t m_width;
	size_t m_height;
};

} // namespace NovelTea

#endif // NOVELTEA_ENGINE_SFML_H
