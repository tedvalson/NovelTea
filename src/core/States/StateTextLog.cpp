#include <NovelTea/States/StateTextLog.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/TextLog.hpp>
#include <TweenEngine/Tween.h>
#include <SFML/System/Err.hpp>

namespace NovelTea
{

StateTextLog::StateTextLog(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
{
	m_buttonClose.getText().setFont(*Proj.getFont(1));
	m_buttonClose.setString(L"\uf00d");
	m_buttonClose.setColor(sf::Color(230, 0 , 0));
	m_buttonClose.setTextColor(sf::Color::White);
	m_buttonClose.onClick([this](){
		hide(1.f, ALPHA, [this](){
			requestStackPop();
		});
	});

	m_bg.setFillColor(sf::Color(180, 180, 180, 0));

	hide(0.f);
	show(1.f);
}

void StateTextLog::render(sf::RenderTarget &target)
{
	target.draw(m_bg);
	target.draw(m_textLogRenderer);
	target.draw(m_buttonClose);
}

void StateTextLog::resize(const sf::Vector2f &size)
{
	auto w = size.x;
	auto h = size.y;
	auto wi = std::min(w, h);
	auto portrait = (w < h);
	auto margin = 0.1f * wi;
	auto fontSizeMultiplier = getContext().config.fontSizeMultiplier * getContext().config.dpiMultiplier;
	m_screenSize = size;

	auto buttonHeight = wi * 0.09f;
	m_buttonClose.getText().setCharacterSize(buttonHeight * 0.9f);
	m_buttonClose.setSize(buttonHeight, buttonHeight);
	m_buttonClose.setPosition(size.x - m_buttonClose.getSize().x * 1.3f, m_buttonClose.getSize().y * 0.3f);

	m_bg.setSize(size);

	m_textLogRenderer.setFontSizeMultiplier(fontSizeMultiplier);
	m_textLogRenderer.setPosition({20.f, 20.f});
	m_textLogRenderer.setSize(size * 0.5f);
}

void StateTextLog::setAlpha(float alpha)
{
	sf::Color color;
	float *newValues = &alpha;
	SET_ALPHA(m_bg.getFillColor, m_bg.setFillColor, 245.f);
	m_textLogRenderer.setAlpha(alpha);
	m_buttonClose.setAlpha(alpha);
	State::setAlpha(alpha);
}

bool StateTextLog::processEvent(const sf::Event &event)
{
	if (m_buttonClose.processEvent(event))
		return false;
	m_textLogRenderer.processEvent(event);
	return false;
}

bool StateTextLog::update(float delta)
{
	m_textLogRenderer.update(delta);
	State::update(delta);
	return false;
}

} // namespace NovelTea
