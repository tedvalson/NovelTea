#include <NovelTea/GUI/VerbList.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/Engine.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <TweenEngine/Tween.h>
#include <iostream>

namespace NovelTea
{

VerbList::VerbList()
: m_scrollPos(0.f)
, m_margin(10.f)
, m_size(320.f, 400.f)
, m_visible(false)
, m_isHiding(false)
, m_isShowing(false)
, m_selectCallback(nullptr)
, m_showHideCallback(nullptr)
{
	sf::FloatRect viewport(0.f, 0.f, 0.5f, 1.f);
	m_view.reset(sf::FloatRect(0, 0, 500, 700));
	m_view.setViewport(viewport);

	m_scrollBar.setColor(sf::Color(0, 0, 0, 40));
	m_scrollBar.setAutoHide(false);
	m_scrollBar.attachObject(this);

	m_bg.setFillColor(sf::Color(230, 230, 230, 0));

	std::vector<std::string> verbs;
	setVerbs(verbs);

	TweenEngine::Tween::set(*this, ALPHA)
		.target(0.f)
		.start(m_tweenManager);

}

void VerbList::update(float delta)
{
	m_scrollBar.update(delta);
	m_tweenManager.update(delta);
}

bool VerbList::processEvent(const sf::Event &event)
{
	if (m_scrollBar.processEvent(event))
		return false;
	if (m_isHiding)
		return true;

	if (event.type == sf::Event::MouseButtonReleased)
	{
		auto p = getInverseTransform().transformPoint(event.mouseButton.x, event.mouseButton.y);
		for (auto &verb : m_verbs)
		{
			if (verb.text.getGlobalBounds().contains(p))
			{
				if (m_selectCallback)
					m_selectCallback(verb.verbId);
				return false;
			}
		}
	}

	return true;
}

void VerbList::show()
{
	if (!m_isShowing)
	{
		m_visible = true;
		m_isShowing = true;
		TweenEngine::Tween::to(*this, ALPHA, 0.4f)
			.target(255.f)
			.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*){
				m_isShowing = false;
				if (m_showHideCallback)
					m_showHideCallback(true);
			}).start(m_tweenManager);
	}
}

void VerbList::hide()
{
	if (!m_isHiding)
	{
		m_isHiding = true;
		TweenEngine::Tween::to(*this, ALPHA, 0.4f)
			.target(0.f)
			.setCallback(TweenEngine::TweenCallback::COMPLETE, [this](TweenEngine::BaseTween*){
				m_visible = false;
				m_isHiding = false;
				if (m_showHideCallback)
					m_showHideCallback(false);
			}).start(m_tweenManager);
	}
}

bool VerbList::isVisible() const
{
	return m_visible;
}

void VerbList::setVerbs(const std::vector<std::string> &verbs)
{
	m_verbs.clear();

	for (auto &item : Save.data()[Verb::id].ObjectRange())
		addVerbOption(item.first);
	for (auto &item : ProjData[Verb::id].ObjectRange())
	{
		auto verbId = item.first;
		if (!Save.data()[Verb::id].hasKey(verbId))
			addVerbOption(verbId);
	}

	float maxWidth = 0.f;
	float posY = m_margin + 42.f * m_verbs.size();
	for (auto &verb : m_verbs)
		maxWidth = std::max(maxWidth, verb.text.getLocalBounds().width);

	m_bounds = sf::FloatRect(0.f, 0.f, maxWidth + m_margin*2, posY + m_margin*2);
	m_scrollBar.setSize(sf::Vector2u(2, posY + m_margin*2));
	m_scrollBar.setScrollAreaSize(sf::Vector2u(320, posY*2));
	m_scrollBar.setPosition(m_bounds.width + 4.f, 0.f);
	m_size.y = posY;

	m_bg.setSize(sf::Vector2f(m_bounds.width, m_bounds.height));
	repositionItems();
}

void VerbList::setVerbs(const std::string &objectId)
{
	std::vector<std::string> v;
	setVerbs(v);
}

void VerbList::setSelectCallback(VerbSelectCallback callback)
{
	m_selectCallback = callback;
}

void VerbList::setShowHideCallback(VerbShowHideCallback callback)
{
	m_showHideCallback = callback;
}

void VerbList::setScroll(float position)
{
	m_scrollPos = round(position);
	repositionItems();
}

float VerbList::getScroll()
{
	return m_scrollPos;
}

const sf::Vector2f &VerbList::getScrollSize()
{
	return m_size;
}

sf::FloatRect VerbList::getLocalBounds() const
{
	return m_bounds;
}

sf::FloatRect VerbList::getGlobalBounds() const
{
	return getTransform().transformRect(m_bounds);
}

void VerbList::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	auto transform = getTransform();
	states.transform *= transform;
	target.draw(m_bg, states);
	target.draw(m_scrollBar, states);

	auto view = target.getView();
	if (m_targetSize != target.getSize() || m_lastTransform != transform)
	{
		m_lastTransform = transform;
		m_targetSize = target.getSize();
		auto bounds = getGlobalBounds();
		auto vp = target.getViewport(view);
		auto ratio = view.getSize().x / vp.width;

		sf::FloatRect viewport;
		viewport.left = (bounds.left / ratio + vp.left) / m_targetSize.x;
		viewport.top = (bounds.top / ratio + vp.top) / m_targetSize.y;
		viewport.width = bounds.width / ratio / m_targetSize.x;
		viewport.height = bounds.height / ratio / m_targetSize.y;

		m_view.reset(bounds);
		m_view.setViewport(viewport);
	}

	target.setView(m_view);

	for (auto &verb : m_verbs)
	{
		target.draw(verb.text, states);
	}

	target.setView(view);
}

void VerbList::setValues(int tweenType, float *newValues)
{
	switch (tweenType) {
		case ALPHA: {
			sf::Color color;
			SET_ALPHA(m_bg.getFillColor, m_bg.setFillColor, 230.f);
			SET_ALPHA(m_scrollBar.getColor, m_scrollBar.setColor, 40.f);
			for (auto &verb : m_verbs) {
				SET_ALPHA(verb.text.getFillColor, verb.text.setFillColor, 255.f);
			}
			break;
		}
		default:
			TweenTransformable::setValues(tweenType, newValues);
	}
}

int VerbList::getValues(int tweenType, float *returnValues)
{
	switch (tweenType) {
		case ALPHA:
			returnValues[0] = m_bg.getFillColor().a / 200.f * 255.f;
			return 1;
		default:
			return TweenTransformable::getValues(tweenType, returnValues);
	}
}

void VerbList::repositionItems()
{
	float posY = m_margin + m_scrollPos;
	for (auto &verb : m_verbs)
	{
		verb.text.setPosition(m_margin, posY);
		posY += 42.f;
	}
//	m_size.y = posY - m_scrollPos;
	updateScrollSize();
}

void VerbList::addVerbOption(const std::string &verbId)
{
	VerbOption option;
	option.verbId = verbId;
	option.text.setCharacterSize(40);
	option.text.setFillColor(sf::Color::Black);
	option.text.setFont(*Proj.getFont(0));

	auto verb = Save.get<Verb>(verbId);
	option.text.setString(verb->getName());
	m_verbs.push_back(option);
}

} // namespace NovelTea
