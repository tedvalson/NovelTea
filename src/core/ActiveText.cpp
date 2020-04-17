#include <NovelTea/ActiveText.hpp>
#include <NovelTea/TextBlock.hpp>
#include <NovelTea/TextFragment.hpp>
#include <NovelTea/ProjectData.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace NovelTea
{

ActiveText::ActiveText()
	: m_size(sf::Vector2f(300.f, 300.f))
{
}

json ActiveText::toJson() const
{
	json j = json::array();
	for (auto &block : m_textBlocks)
		j.push_back(*block);
	return j;
}

bool ActiveText::fromJson(const json &j)
{
	m_textBlocks.clear();
	m_needsUpdate = true;

	for (auto &jblock : j)
	{
		auto block = std::make_shared<TextBlock>();
		if (block->fromJson(jblock))
			m_textBlocks.push_back(block);
	}
	return true;
}

std::string ActiveText::toPlainText() const
{
	std::string result;
	bool processedFirstBlock = false;
	for (auto &block : blocks())
	{
		if (processedFirstBlock)
			result += " | ";
		processedFirstBlock = true;
		for (auto &frag : block->fragments())
			result += frag->getText();
	}
	return result;
}

const std::vector<std::shared_ptr<TextBlock>> &ActiveText::blocks() const
{
	return m_textBlocks;
}

void ActiveText::addBlock(std::shared_ptr<TextBlock> block, int index)
{
	m_needsUpdate = true;
	if (index < 0)
		m_textBlocks.push_back(block);
	else
		m_textBlocks.insert(m_textBlocks.begin() + index, block);
}

void ActiveText::setSize(const sf::Vector2f &size)
{
	m_needsUpdate = true;
	m_size = size;
}

sf::Vector2f ActiveText::getSize() const
{
	return m_size;
}

void ActiveText::setCursorStart(const sf::Vector2f &cursorPos)
{
	m_needsUpdate = true;
	m_cursorStart = cursorPos;
}

const sf::Vector2f &ActiveText::getCursorEnd() const
{
	ensureUpdate();
	return m_cursorPos;
}

void ActiveText::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	ensureUpdate();
	states.transform *= getTransform();

	for (auto &text : m_texts)
	{
		target.draw(text, states);
	}

	target.draw(m_debugBorder, states);
}

void ActiveText::ensureUpdate() const
{
	if (!m_needsUpdate)
		return;

	m_cursorPos = m_cursorStart;
	m_texts.clear();

	for (auto &block : blocks())
	{
		for (auto &frag : block->fragments())
		{
			auto font = Proj.getFont(0);
			auto format = frag->getTextFormat();
			sf::Uint32 style = sf::Text::Regular;

			if (format.bold())
				style |= sf::Text::Bold;
			if (format.italic())
				style |= sf::Text::Italic;
			if (format.underline())
				style |= sf::Text::Underlined;

			TweenText text;
			text.setFont(*font);
			text.setString(frag->getText());
			text.setPosition(m_cursorPos);
			text.setFillColor(sf::Color::Black);
			text.setStyle(style);
			text.setCharacterSize(format.size()*3);
			m_texts.push_back(text);

			m_cursorPos.x += text.getLocalBounds().width;
			if (m_cursorPos.x > m_size.x)
			{
				m_cursorPos.x = 0;
				m_cursorPos.y += text.getLocalBounds().height;
				m_cursorPos.y += 5.f * text.getLineSpacing();
			}
		}
	}

	m_debugBorder.setFillColor(sf::Color::Transparent);
	m_debugBorder.setOutlineColor(sf::Color::Red);
	m_debugBorder.setOutlineThickness(2.f);
	m_debugBorder.setSize(m_size);

	m_needsUpdate = false;
}

} // namespace NovelTea
