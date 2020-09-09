#ifndef NOVELTEA_ACTIVETEXT_HPP
#define NOVELTEA_ACTIVETEXT_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/GUI/Hideable.hpp>
#include <NovelTea/TextFormat.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <vector>
#include <memory>

namespace NovelTea
{

class TextBlock;

class ActiveText : public JsonSerializable, public sf::Drawable, public Hideable
{
public:
	struct Segment {
		TweenText text;
		std::string objectIdName;
		sf::FloatRect bounds;
	};

	ActiveText();
	json toJson() const override;
	bool fromJson(const json &j) override;

	std::string toPlainText() const;
	std::string objectFromPoint(const sf::Vector2f &point) const;

	void setText(const std::string &text, const TextFormat &format = TextFormat());

	const std::vector<std::shared_ptr<TextBlock>> &blocks() const;
	void addBlock(std::shared_ptr<TextBlock> block, int index = -1);

	void setSize(const sf::Vector2f &size);
	sf::Vector2f getSize() const;

	void setHighlightId(const std::string &id);
	void refresh();

	float getTextWidth() const;

	void setCursorStart(const sf::Vector2f &cursorPos);
	const sf::Vector2f &getCursorEnd() const;

	void setAlpha(float alpha) override;
	float getAlpha() const override;

	std::vector<Segment> &getSegments();

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void ensureUpdate() const;

private:
	std::vector<std::shared_ptr<TextBlock>> m_textBlocks;
	mutable std::vector<Segment> m_segments;
	mutable sf::Vector2f m_cursorPos;
	sf::Vector2f m_cursorStart;
	sf::Vector2f m_size;
	mutable bool m_needsUpdate = true;
	float m_alpha;

	mutable sf::RectangleShape m_debugBorder;
	mutable std::vector<sf::RectangleShape> m_debugSegmentShapes;
};

} // namespace NovelTea

#endif // NOVELTEA_ACTIVETEXT_HPP
