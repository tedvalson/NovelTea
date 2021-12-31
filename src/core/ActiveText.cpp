#include <NovelTea/ActiveText.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/Diff.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/TextBlock.hpp>
#include <NovelTea/TextFragment.hpp>

namespace NovelTea
{

ActiveText::ActiveText()
	: m_size(1024.f, 1024.f)
	, m_lineSpacing(5.f)
	, m_alpha(255.f)
	, m_highlightFactor(1.f)
	, m_fontSizeMultiplier(1.f)
	, m_fadeAcrossPosition(1.f)
	, m_fadeLineIndex(0)
	, m_renderTexture(nullptr)
{
	auto texture = AssetManager<sf::Texture>::get("images/fade.png");
	texture->setSmooth(true);
	m_shape.setFillColor(sf::Color::Transparent);
	m_shapeFade.setTexture(texture.get(), true);
	m_shapeFade.setSize(sf::Vector2f(300.f, 40.f));
	m_shapeFade.setOrigin(0.f, 40.f);
	m_shapeFade.setRotation(90.f);
}

void ActiveText::createRenderTexture()
{
	m_shape.setSize(m_size);
	m_renderTexture = std::make_shared<sf::RenderTexture>();
	m_renderTexture->create(m_size.x, 512); // TODO: Avoid fixed size
	m_sprite.setTexture(m_renderTexture->getTexture(), true);
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
		if (!GSave->exists<Object>(idName))
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

	while ((startPos = s.find(DIFF_OPEN_TAG, searchPos)) != sf::String::InvalidPos)
	{
		auto endPos = s.find(DIFF_CLOSE_TAG, startPos);
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

std::string ActiveText::toPlainText(const std::string &newline) const
{
	std::string result;
	bool processedFirstBlock = false;
	for (auto &block : blocks())
	{
		if (processedFirstBlock)
			result += newline;
		processedFirstBlock = true;
		for (auto &frag : block->fragments())
			result += frag->getTextRaw();
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

	m_string = stripDiff(text);
	ensureUpdate();
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
	if (m_renderTexture)
		createRenderTexture();
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
			segment.text.setOutlineColor(sf::Color(255, 255, 0, 140));
			segment.text.setOutlineThickness(0.1f * segment.text.getCharacterSize());
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
	m_alpha = alpha;
	applyAlpha();
}

float ActiveText::getAlpha() const
{
	return m_alpha;
}

void ActiveText::setHighlightFactor(float highlightFactor)
{
	m_highlightFactor = highlightFactor;
	applyHighlightFactor();
}

float ActiveText::getHighlightFactor() const
{
	return m_highlightFactor;
}

void ActiveText::setFontSizeMultiplier(float fontSizeMultiplier)
{
	m_needsUpdate = true;
	m_fontSizeMultiplier = fontSizeMultiplier;
}

float ActiveText::getFontSizeMultiplier() const
{
	return m_fontSizeMultiplier;
}

void ActiveText::setFadeAcrossPosition(float position)
{
	ensureUpdate();
	m_fadeAcrossPosition = position;
	if (position == 1.f) {
		if (m_renderTexture)
			m_renderTexture = nullptr;
	} else {
		if (!m_renderTexture)
			createRenderTexture();

		auto fadeLength = getFadeAcrossLength();
		auto pos = m_fadeAcrossPosition * fadeLength;
		auto p = 0.f;

		m_fadeLineIndex = 0;
		for (auto &line : m_linePositions)
		{
			p += line.x + m_shapeFade.getSize().y;
			if (p > pos) {
				m_shape.setPosition(line.x - p + pos, line.y);
				m_shapeFade.setPosition(m_shape.getPosition());
				m_shape.move(m_shapeFade.getSize().y, 0.f);
				break;
			}
			m_fadeLineIndex++;
		}
	}
}

float ActiveText::getFadeAcrossPosition() const
{
	return m_fadeAcrossPosition;
}

float ActiveText::getFadeAcrossLength() const
{
	ensureUpdate();
	auto len = 0.f;
	for (auto &linePos : m_linePositions)
		len += linePos.x + m_shapeFade.getSize().y;
	return len;
}

std::vector<ActiveText::Segment> &ActiveText::getSegments()
{
	ensureUpdate();
	return m_segments;
}

void ActiveText::setValues(int tweenType, float *newValues)
{
	switch (tweenType) {
		case HIGHLIGHTS:
			setHighlightFactor(newValues[0]);
			break;
		case FADEACROSS:
			setFadeAcrossPosition(newValues[0]);
			break;
		default:
			Hideable::setValues(tweenType, newValues);
	}
}

int ActiveText::getValues(int tweenType, float *returnValues)
{
	switch (tweenType) {
	case HIGHLIGHTS:
		returnValues[0] = getHighlightFactor();
		return 1;
	case FADEACROSS:
		returnValues[0] = getFadeAcrossPosition();
		return 1;
	default:
		return Hideable::getValues(tweenType, returnValues);
	}
}

void ActiveText::applyAlpha() const
{
	sf::Color color;
	const float *newValues = &m_alpha; // Hack for the macro below
	for (auto &segment : m_segments) {
		SET_ALPHA(segment.text.getFillColor, segment.text.setFillColor, 255.f);
	}
}

void ActiveText::applyHighlightFactor() const
{
	for (auto &segment : m_segments)
	{
		if (!segment.objectIdName.empty())
		{
			sf::Color color;
			if (segment.objectExists)
				color = sf::Color(0, 0, m_highlightFactor * 200, m_alpha);
			else
				color = sf::Color(m_highlightFactor * 155, 0, m_highlightFactor * 255, m_alpha);
			segment.text.setFillColor(color);
		}
	}
}

void ActiveText::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	ensureUpdate();
	if (m_segments.empty())
		return;
	states.transform *= getTransform();

	if (m_renderTexture)
	{
		auto posY = m_segments[0].text.getPosition().y;
		auto lineIndex = 0;
		m_renderTexture->clear(sf::Color::Transparent);
		for (auto &segment : m_segments)
		{
			auto p = segment.text.getPosition().y;
			if (p > posY) {
				posY = p;
				lineIndex++;
			}
			if (lineIndex == m_fadeLineIndex)
				m_renderTexture->draw(segment.text, sf::BlendNone);
			else if (lineIndex > m_fadeLineIndex)
				break;
			else
				target.draw(segment.text, states);
		}
		m_renderTexture->draw(m_shape, sf::BlendMultiply);
		m_renderTexture->draw(m_shapeFade, sf::BlendMultiply);
		m_renderTexture->display();
		target.draw(m_sprite, states);
	}
	else
	{
		for (auto &segment : m_segments)
			target.draw(segment.text, states);

	}
}

void ActiveText::ensureUpdate() const
{
	if (!m_needsUpdate)
		return;

	auto padding = 6.f;
	float lineHeight = 24.f; // TODO: Don't use fixed value
	auto processedFirstBlock = false;
	m_cursorPos = m_cursorStart;
	m_segments.clear();
	m_linePositions.clear();
	m_debugSegmentShapes.clear();
	m_bounds = sf::FloatRect(0.f, m_cursorStart.y, m_cursorStart.x, m_cursorStart.y + lineHeight);

	for (auto &block : blocks())
	{
		// Keep largest char size for current line
		auto lineMaxCharacterSize = 0u;
		auto &frags = block->fragments();

		if (processedFirstBlock) // When '\n' is encountered
		{
			m_cursorPos.x = 0.f;
			m_cursorPos.y = m_bounds.height;
			if (!frags.empty() && frags[0]->getTextRaw().empty())
				m_bounds.height += lineHeight + m_lineSpacing;
		}
		else
			processedFirstBlock = true;

		for (auto &frag : frags)
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
				text.setCharacterSize(2.f * m_fontSizeMultiplier * format.size());
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

					auto string = textObjectPair.first;
					auto objectId = textObjectPair.second;
					auto objectExists = false;
					auto color = (newTextPair.first ? sf::Color(150, 0, 0) : format.color());
					if (!objectId.empty()) {
						objectExists = ActiveGame->getRoom()->containsId(objectId) ||
									  ActiveGame->getObjectList()->containsId(objectId);
					}
					color.a = m_alpha;
					text.setString(string);
					text.setFillColor(color);
					lineMaxCharacterSize = std::max(lineMaxCharacterSize, text.getCharacterSize());

					// Hack to prevent these chars from wrapping by themselves.
					const std::string specialChars = ".,!?";
					auto isSpecialChar = (string.size() == 1 && specialChars.find(string) != specialChars.npos);

					auto newX = m_cursorPos.x + text.getLocalBounds().width;
					if (newX > m_size.x && m_cursorPos.x > 0.f && !isSpecialChar)
					{
						m_linePositions.push_back(m_cursorPos);
						m_cursorPos.x = 0.f;
						m_cursorPos.y += lineMaxCharacterSize + m_lineSpacing;
						lineMaxCharacterSize = text.getCharacterSize();
					}

					text.setPosition(m_cursorPos);
					m_cursorPos.x += text.getLocalBounds().width;

					m_bounds.width = std::max(m_bounds.width, m_cursorPos.x);
					m_bounds.height = std::max(m_bounds.height, m_cursorPos.y + lineMaxCharacterSize + m_lineSpacing);
					auto bounds = sf::FloatRect(
								text.getGlobalBounds().left - padding,
								text.getGlobalBounds().top - padding,
								text.getLocalBounds().width + padding * 2,
								text.getLocalBounds().height + padding * 2);

					shape.setSize(sf::Vector2f(bounds.width, bounds.height));
					shape.setPosition(bounds.left, bounds.top);
					m_debugSegmentShapes.push_back(shape);

					m_segments.push_back({objectExists, textObjectPair.second, text, bounds});
				}
			}
		}
		m_linePositions.push_back(m_cursorPos);
	}

	applyAlpha();
	applyHighlightFactor();

	m_debugBorder.setFillColor(sf::Color::Transparent);
	m_debugBorder.setOutlineColor(sf::Color::Red);
	m_debugBorder.setOutlineThickness(2.f);
	m_debugBorder.setSize(sf::Vector2f(m_bounds.width, m_bounds.height - m_bounds.top));
	m_debugBorder.setPosition(m_bounds.left, m_bounds.top);

	m_needsUpdate = false;
}

} // namespace NovelTea
