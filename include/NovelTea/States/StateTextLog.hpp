#ifndef NOVELTEA_STATETEXTLOG_HPP
#define NOVELTEA_STATETEXTLOG_HPP

#include <NovelTea/States/State.hpp>
#include <TweenEngine/TweenManager.h>
#include <NovelTea/TweenObjects.hpp>
#include <NovelTea/GUI/ScrollBar.hpp>

namespace NovelTea
{

class ActiveText;

class StateTextLog : public State, public Scrollable
{
public:
	StateTextLog(StateStack& stack, Context& context, StateCallback callback);
	bool processEvent(const sf::Event &event) override;
	bool update(float delta) override;
	void render(sf::RenderTarget &target) override;
	void resize(const sf::Vector2f &size) override;

	void setAlpha(float alpha) override;

	void setScroll(float position) override;
	float getScroll() override;
	const sf::Vector2f &getScrollSize() override;
	
	void refreshItems();
	void repositionItems();

private:
	TweenText m_text;
	TweenRectangleShape m_bg;
	TweenEngine::TweenManager m_tweenManager;
	sf::View m_textView;
	sf::Vector2f m_screenSize;
	
	std::vector<std::shared_ptr<ActiveText>> m_texts;

	float m_scrollPos;
	sf::Vector2f m_scrollAreaSize;
	ScrollBar m_scrollbar;
};

} // namespace NovelTea

#endif // NOVELTEA_STATETEXTLOG_HPP
