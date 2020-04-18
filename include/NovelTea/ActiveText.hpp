#ifndef NOVELTEA_ACTIVETEXT_HPP
#define NOVELTEA_ACTIVETEXT_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/TweenObjects.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <vector>

namespace NovelTea
{

class TextBlock;

class ActiveText : public JsonSerializable, public sf::Drawable, public TweenTransformable<sf::Transformable>
{
public:
	ActiveText();
	json toJson() const override;
	bool fromJson(const json &j) override;

	std::string toPlainText() const;

	const std::vector<std::shared_ptr<TextBlock>> &blocks() const;
	void addBlock(std::shared_ptr<TextBlock> block, int index = -1);

	void setSize(const sf::Vector2f &size);
	sf::Vector2f getSize() const;

	void setCursorStart(const sf::Vector2f &cursorPos);
	const sf::Vector2f &getCursorEnd() const;

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void ensureUpdate() const;

private:
	std::vector<std::shared_ptr<TextBlock>> m_textBlocks;
	mutable std::vector<TweenText> m_texts;
	mutable sf::Vector2f m_cursorPos;
	sf::Vector2f m_cursorStart;
	sf::Vector2f m_size;
	mutable bool m_needsUpdate = true;
	mutable sf::RectangleShape m_debugBorder;
};

} // namespace NovelTea

#endif // NOVELTEA_ACTIVETEXT_HPP
