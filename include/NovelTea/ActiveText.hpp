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

	static const int HIGHLIGHTS = 12;

	struct Segment {
		bool objectExists;
		std::string objectIdName;
		TweenText text;
		sf::FloatRect bounds;
	};

	ActiveText();
	json toJson() const override;
	bool fromJson(const json &j) override;

	std::string toPlainText() const;
	std::string objectFromPoint(const sf::Vector2f &point) const;

	void setText(const std::string &text, const TextFormat &format = TextFormat());
	std::string getText() const;

	const std::vector<std::shared_ptr<TextBlock>> &blocks() const;
	void addBlock(std::shared_ptr<TextBlock> block, int index = -1);

	void setSize(const sf::Vector2f &size);
	sf::Vector2f getSize() const;

	void setHighlightId(const std::string &id);
	void refresh();

	float getTextWidth() const;

	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

	void setLineSpacing(float lineSpacing);
	float getLineSpacing() const;

	void setCursorStart(const sf::Vector2f &cursorPos);
	const sf::Vector2f &getCursorEnd() const;

	void setAlpha(float alpha) override;
	float getAlpha() const override;

	void setHighlightFactor(float highlightFactor);
	float getHighlightFactor() const;

	std::vector<Segment> &getSegments();

protected:
	virtual void setValues(int tweenType, float *newValues) override;
	virtual int getValues(int tweenType, float *returnValues) override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void ensureUpdate() const;

private:
	std::vector<std::shared_ptr<TextBlock>> m_textBlocks;
	mutable std::vector<Segment> m_segments;
	mutable sf::Vector2f m_cursorPos;
	mutable sf::FloatRect m_bounds;
	sf::Vector2f m_cursorStart;
	sf::Vector2f m_size;
	std::string m_string;
	mutable bool m_needsUpdate = true;
	float m_lineSpacing;
	float m_alpha;
	float m_highlightFactor;

	mutable sf::RectangleShape m_debugBorder;
	mutable std::vector<sf::RectangleShape> m_debugSegmentShapes;
};

} // namespace NovelTea

#endif // NOVELTEA_ACTIVETEXT_HPP
