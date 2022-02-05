#ifndef NOVELTEA_STATEPROFILEMANAGER_HPP
#define NOVELTEA_STATEPROFILEMANAGER_HPP

#include <NovelTea/States/State.hpp>
#include <NovelTea/GUI/ScrollBar.hpp>
#include <NovelTea/GUI/Scrollable.hpp>
#include <TweenEngine/TweenManager.h>
#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/GUI/Button.hpp>

namespace NovelTea
{

class StateProfileManager : public State, public Scrollable
{
public:
	StateProfileManager(StateStack& stack, Context& context, StateCallback callback);
	bool processEvent(const sf::Event &event) override;
	bool update(float delta) override;
	void render(sf::RenderTarget &target) override;
	void resize(const sf::Vector2f &size) override;

	void setScroll(float position) override;
	float getScroll() override;
	const sf::Vector2f &getScrollSize() override;

	void repositionItems();
	void setActiveProfile(int index);

	void add(const std::string &text);
	void remove();
	void refresh();

	void setAlpha(float alpha) override;

private:
	TweenText m_textTitle;
	Button m_buttonBack;
	Button m_buttonAdd;
	Button m_buttonRemove;

	float m_scrollPos;
	float m_buttonHeight;
	float m_buttonSpacing;
	sf::FloatRect m_buttonRect;
	sf::Vector2f m_scrollAreaSize;
	ScrollBar m_scrollBar;
	sf::View m_view;

	std::vector<std::unique_ptr<Button>> m_profileButtons;
	bool m_cancelButtonPress;

	TweenNinePatch m_bg;
	TweenRectangleShape m_overlay;
	TweenEngine::TweenManager m_tweenManager;
};

} // namespace NovelTea

#endif // NOVELTEA_STATEPROFILEMANAGER_HPP
