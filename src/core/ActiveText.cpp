#include <NovelTea/ActiveText.hpp>
#include <NovelTea/TextBlock.hpp>
#include <NovelTea/TextFragment.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Object.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace NovelTea
{

ActiveText::ActiveText()
	: m_size(sf::Vector2f(400.f, 400.f))
	, m_alpha(255.f)
{
}

void splitAndAppend(const std::string &text, const std::string &idName, std::vector<std::pair<std::string, std::string>> &pairs)
{
	std::string buffer;
	for (auto &c : text)
	{
		if (c == ' ')
		{
			if (!buffer.empty())
				pairs.emplace_back(buffer, idName);
			pairs.emplace_back("", "");
			buffer.clear();
		}
		else
			buffer += c;
	}

	if (!buffer.empty())
		pairs.emplace_back(buffer, idName);
}

std::vector<std::pair<std::string, std::string>> getTextObjectPairs(const sf::String &s)
{
	std::vector<std::pair<std::string, std::string>> v;

	size_t searchPos = 0,
		   processedPos = 0,
		   startPos;

	while ((startPos = s.find("[[", searchPos)) != sf::String::InvalidPos)
	{
		auto endPos = s.find("]]", startPos);
		auto midPos = s.find("|", startPos);
		if (endPos == sf::String::InvalidPos)
			break;

		// if there is no mid char "|" in between braces, skip it
		if (midPos == sf::String::InvalidPos || endPos < midPos)
		{
			searchPos = endPos + 2;
			continue;
		}

		auto idName = s.substring(midPos + 1, endPos - midPos - 1);
		auto text = s.substring(startPos + 2, midPos - startPos - 2);
		if (!GSave.exists<Object>(idName))
			idName.clear();
		if (startPos != processedPos)
			splitAndAppend(s.substring(processedPos, startPos - processedPos).toAnsiString(), "", v);
		splitAndAppend(text, idName, v);
		processedPos = searchPos = endPos + 2;
	}

	// Push remaining unprocessed string
	if (processedPos < s.getSize())
		splitAndAppend(s.substring(processedPos), "", v);

	return v;
}

json ActiveText::toJson() const
{
	json j = sj::Array();
	for (auto &block : m_textBlocks)
		j.append(block->toJson());
	return j;
}

bool ActiveText::fromJson(const json &j)
{
	m_textBlocks.clear();
	m_needsUpdate = true;

	for (auto &jblock : j.ArrayRange())
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

std::string ActiveText::objectFromPoint(const sf::Vector2f &point) const
{
	for (auto &segment : m_segments)
	{
		if (!segment.objectIdName.empty() && segment.bounds.contains(point))
			return segment.objectIdName;
	}
	return std::string();
}

void ActiveText::setText(const std::string &text)
{
	auto block = std::make_shared<TextBlock>();
	auto fragment = std::make_shared<TextFragment>();

	fragment->setText(text);
	block->addFragment(fragment);

	m_textBlocks.clear();
	addBlock(block);

	setAlpha(m_alpha);
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

void ActiveText::setHighlightId(const std::string &id)
{
	for (auto &segment : m_segments)
	{
		if (id.empty() || id != segment.objectIdName)
		{
			segment.text.setOutlineThickness(0.f);
		}
		else
		{
			segment.text.setOutlineColor(sf::Color::Yellow);
			segment.text.setOutlineThickness(3.f);
		}
	}
}

void ActiveText::refresh()
{
	m_needsUpdate = true;
}

float ActiveText::getTextWidth() const
{
	auto width = 0.f;
	for (auto &segment : m_segments)
		width += segment.text.getLocalBounds().width;
	return width;
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

void ActiveText::setAlpha(float alpha)
{
	sf::Color color;
	m_alpha = alpha;
	float *newValues = &alpha; // Hack for the macro below
	for (auto &segment : m_segments) {
		SET_ALPHA(segment.text.getFillColor, segment.text.setFillColor, 255.f);
	}
}

float ActiveText::getAlpha() const
{
	return m_alpha;
}

std::vector<ActiveText::Segment> &ActiveText::getSegments()
{
	return m_segments;
}

void ActiveText::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	ensureUpdate();
	states.transform *= getTransform();

	for (auto &segment : m_segments)
		target.draw(segment.text, states);

}

void ActiveText::ensureUpdate() const
{
	if (!m_needsUpdate)
		return;

	auto processedFirstBlock = false;
	m_cursorPos = m_cursorStart;
	m_segments.clear();
	m_debugSegmentShapes.clear();

	for (auto &block : blocks())
	{
		if (processedFirstBlock)
		{
			m_cursorPos.x = 0.f;
			m_cursorPos.y += 30.f; // TODO: don't used fixed line height
		}
		else
			processedFirstBlock = true;

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

			auto textObjectPairs = getTextObjectPairs(frag->getText());


			sf::RectangleShape shape;
			shape.setFillColor(sf::Color(0, 0, 0, 50));

			TweenText text;
			text.setFont(*font);
			text.setCharacterSize(format.size()*2);
			text.setStyle(style);

			text.setString(" ");
			auto spaceWidth = text.getLocalBounds().width;

			for (auto &p : textObjectPairs)
			{
				// Don't start line with a space
				if (p.first.empty())
				{
					if (m_cursorPos.x > 0)
						m_cursorPos.x += spaceWidth;
					continue;
				}

				auto color = p.second.empty() ? sf::Color::Black : sf::Color::Blue;
				color.a = m_alpha;
				text.setString(p.first);
				text.setFillColor(color);

				auto newX = m_cursorPos.x + text.getLocalBounds().width;

				if (newX > m_size.x && m_cursorPos.x > 0.f)
				{
					m_cursorPos.x = 0.f;
					m_cursorPos.y += text.getCharacterSize();//text.getLocalBounds().height;
				}

				text.setPosition(m_cursorPos);
				m_cursorPos.x += text.getLocalBounds().width;
				float padding = 6.f;
				auto bounds = sf::FloatRect(
							text.getGlobalBounds().left - padding,
							text.getGlobalBounds().top - padding,
							text.getLocalBounds().width + padding * 2,
							text.getLocalBounds().height + padding * 2);

				shape.setSize(sf::Vector2f(bounds.width, bounds.height));
				shape.setPosition(bounds.left, bounds.top);
				m_debugSegmentShapes.push_back(shape);

				bounds = getTransform().transformRect(bounds);
				m_segments.push_back({text, p.second, bounds});
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
