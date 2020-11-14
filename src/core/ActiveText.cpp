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
	: m_size(sf::Vector2f(9999.f, 9999.f))
	, m_lineSpacing(5.f)
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

std::vector<std::pair<bool, std::string>> getNewTextPairs(const sf::String &s)
{
	std::vector<std::pair<bool, std::string>> v;

	size_t searchPos = 0,
		   processedPos = 0,
		   startPos;

	while ((startPos = s.find("^[", searchPos)) != sf::String::InvalidPos)
	{
		auto endPos = s.find("]^", startPos);
		if (endPos == sf::String::InvalidPos)
			break;
		auto text = s.substring(startPos + 2, endPos - startPos - 2);
		if (startPos != processedPos)
			v.emplace_back(false, s.substring(processedPos, startPos - processedPos));
		v.emplace_back(true, text);
		processedPos = searchPos = endPos + 2;
	}

	// Push remaining unprocessed string
	if (processedPos < s.getSize())
		v.emplace_back(false, s.substring(processedPos));
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
		if (!segment.objectIdName.empty())
		{
			auto bounds = getTransform().transformRect(segment.bounds);
			if (bounds.contains(point))
				return segment.objectIdName;
		}
	}
	return std::string();
}

void ActiveText::setText(const std::string &text, const TextFormat &format)
{
	if (m_string == text)
		return;
	m_textBlocks.clear();

	auto lines = split(text);
	for (auto &line : lines)
	{
		auto block = std::make_shared<TextBlock>();
		auto fragment = std::make_shared<TextFragment>();

		fragment->setText(line);
		fragment->setTextFormat(format);
		block->addFragment(fragment);
		addBlock(block);
	}

	m_string = text;
	setAlpha(m_alpha);
}

std::string ActiveText::getText() const
{
	return m_string;
}

const std::vector<std::shared_ptr<TextBlock>> &ActiveText::blocks() const
{
	return m_textBlocks;
}

void ActiveText::addBlock(std::shared_ptr<TextBlock> block, int index)
{
	m_needsUpdate = true;
	m_string.clear();
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

sf::FloatRect ActiveText::getLocalBounds() const
{
	ensureUpdate();
	return m_bounds;
}

sf::FloatRect ActiveText::getGlobalBounds() const
{
	ensureUpdate();
	return getTransform().transformRect(m_bounds);
}

void ActiveText::setLineSpacing(float lineSpacing)
{
	m_needsUpdate = true;
	m_lineSpacing = lineSpacing;
}

float ActiveText::getLineSpacing() const
{
	return m_lineSpacing;
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
	ensureUpdate();
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

	auto padding = 6.f;
	auto processedFirstBlock = false;
	m_cursorPos = m_cursorStart;
	m_segments.clear();
	m_debugSegmentShapes.clear();
	m_bounds = sf::FloatRect(0.f, m_cursorStart.y, m_cursorStart.x, m_cursorStart.y);

	for (auto &block : blocks())
	{
		if (processedFirstBlock)
		{
			m_cursorPos.x = 0.f;
			m_bounds.height += 12.f*2 + m_lineSpacing;
			m_cursorPos.y = m_bounds.height - 12.f + m_lineSpacing;
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

			sf::RectangleShape shape;
			shape.setFillColor(sf::Color(0, 0, 0, 30));

			auto newTextPairs = getNewTextPairs(frag->getText());
			for (auto &newTextPair : newTextPairs)
			{

				auto textObjectPairs = getTextObjectPairs(newTextPair.second);

				TweenText text;
				text.setFont(*font);
				text.setCharacterSize(format.size()*2); // TODO: standardize using pt size
				text.setStyle(style);

				text.setString(" ");
				auto spaceWidth = text.getLocalBounds().width;

				for (auto &textObjectPair : textObjectPairs)
				{
					// Don't start line with a space
					if (textObjectPair.first.empty())
					{
						if (m_cursorPos.x > 0)
							m_cursorPos.x += spaceWidth;
						continue;
					}

					auto color = textObjectPair.second.empty() ? (newTextPair.first ? sf::Color::Red : sf::Color::Black) : sf::Color::Blue;
					color.a = m_alpha;
					text.setString(textObjectPair.first);
					text.setFillColor(color);

					auto newX = m_cursorPos.x + text.getLocalBounds().width;

					if (newX > m_size.x && m_cursorPos.x > 0.f)
					{
						m_cursorPos.x = 0.f;
						if (m_bounds.height == m_bounds.top)
							m_cursorPos.y += text.getCharacterSize() + m_lineSpacing;
						else
							m_cursorPos.y = m_bounds.height - 12.f + m_lineSpacing;
					}

					text.setPosition(m_cursorPos);
					m_cursorPos.x += text.getLocalBounds().width;

					m_bounds.width = std::max(m_bounds.width, m_cursorPos.x);
					m_bounds.height = std::max(m_bounds.height, m_cursorPos.y + text.getLocalBounds().height + padding*2);
					auto bounds = sf::FloatRect(
								text.getGlobalBounds().left - padding,
								text.getGlobalBounds().top - padding,
								text.getLocalBounds().width + padding * 2,
								text.getLocalBounds().height + padding * 2);

					shape.setSize(sf::Vector2f(bounds.width, bounds.height));
					shape.setPosition(bounds.left, bounds.top);
					m_debugSegmentShapes.push_back(shape);

					m_segments.push_back({text, textObjectPair.second, bounds});
				}
			}
		}
	}

	m_debugBorder.setFillColor(sf::Color::Transparent);
	m_debugBorder.setOutlineColor(sf::Color::Red);
	m_debugBorder.setOutlineThickness(2.f);
	m_debugBorder.setSize(sf::Vector2f(m_bounds.width, m_bounds.height - m_bounds.top));
	m_debugBorder.setPosition(m_bounds.left, m_bounds.top);

	m_needsUpdate = false;
}

} // namespace NovelTea
