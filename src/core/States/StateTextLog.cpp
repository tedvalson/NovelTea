#include <NovelTea/States/StateTextLog.hpp>
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/Engine.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/TextLog.hpp>
#include <TweenEngine/Tween.h>

namespace NovelTea
{

StateTextLog::StateTextLog(StateStack& stack, Context& context, StateCallback callback)
: State(stack, context, callback)
{
	m_text.setFont(*Proj.getFont(0));
	m_text.setString("Text log");

	m_bg.setFillColor(sf::Color(180, 180, 180, 0));

	m_scrollbar.setColor(sf::Color(100, 100, 100, 140));
	m_scrollbar.setAutoHide(false);
	m_scrollbar.attachObject(this);

	refreshItems();

	setAlpha(0.f);
	TweenEngine::Tween::to(*this, ALPHA, 1.f)
		.target(240.f)
		.start(m_tweenManager);
}

void StateTextLog::render(sf::RenderTarget &target)
{
	target.draw(m_bg);
	target.draw(m_text);

	auto view = target.getView();
	target.setView(m_textView);
	for (auto &text : m_texts)
		target.draw(*text);
	target.setView(view);

	target.draw(m_scrollbar);
}

void StateTextLog::resize(const sf::Vector2f &size)
{
	auto margin = size.y / 10.f;
	m_screenSize = size;

	m_bg.setSize(size);

	m_scrollbar.setScrollAreaSize(sf::Vector2f(size.x, size.y - margin*2));
	m_scrollbar.setSize(sf::Vector2f(4, size.y - 6));
	m_scrollbar.setPosition(size.x - 4.f, 4.f);

	m_textView.reset(sf::FloatRect(0.f, 0.f, size.x, size.y - margin * 2));
	m_textView.setViewport(sf::FloatRect(0.f, margin/size.y, 1.f, 1.f-2.f*margin/size.y));

	refreshItems();
}

void StateTextLog::setAlpha(float alpha)
{
	sf::Color color;
	float *newValues = &alpha;
	SET_ALPHA(m_text.getFillColor, m_text.setFillColor, 245.f);
	SET_ALPHA(m_bg.getFillColor, m_bg.setFillColor, 245.f);
	for (auto &text : m_texts)
		text->setAlpha(alpha);
	State::setAlpha(alpha);
}

void StateTextLog::setScroll(float position)
{
	m_scrollPos = round(position);
	repositionItems();
}

float StateTextLog::getScroll()
{
	return m_scrollPos;
}

const sf::Vector2f &StateTextLog::getScrollSize()
{
	return m_scrollAreaSize;
}

void StateTextLog::refreshItems()
{
	TextFormat format;
	format.size(getContext().config.fontSizeMultiplier * 12);

	m_texts.clear();
	m_scrollAreaSize.y = 0.f;

	for (auto &logItem : GGame.getTextLog()->items())
	{
		format.bold(logItem.type == TextLogType::DialogueOption);

		// Subtract amount from width because of scrollbar
		auto text = new ActiveText;
		text->setSize(sf::Vector2f(m_screenSize.x - 5, m_screenSize.y));
		text->setText(logItem.text, format);
		m_texts.emplace_back(text);

		m_scrollAreaSize.y += text->getLocalBounds().height;
	}

	updateScrollbar();
	repositionItems();
	m_scrollbar.setScroll(-m_scrollAreaSize.y);
}

void StateTextLog::repositionItems()
{
	auto posY = 0.f + m_scrollPos;
	for (auto &text : m_texts)
	{
		text->setPosition(0.f, posY);
		posY += text->getLocalBounds().height;
	}
}

bool StateTextLog::processEvent(const sf::Event &event)
{
	if (m_scrollbar.processEvent(event))
		return false;

	if (event.type == sf::Event::MouseButtonReleased)
	{
		if (m_tweenManager.getRunningTweensCount() > 0)
			return false;
		TweenEngine::Tween::to(*this, ALPHA, 2.f)
			.target(0.f)
			.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*){
				requestStackPop();
			})
			.start(m_tweenManager);
	}
	return false;
}

bool StateTextLog::update(float delta)
{
	m_scrollbar.update(delta);
	m_tweenManager.update(delta);
	return false;
}

} // namespace NovelTea
