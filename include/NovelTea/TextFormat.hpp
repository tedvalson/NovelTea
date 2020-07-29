#ifndef NOVELTEA_TEXTFORMAT_HPP
#define NOVELTEA_TEXTFORMAT_HPP

#include <SFML/Graphics/Color.hpp>
#include <NovelTea/JsonSerializable.hpp>

namespace NovelTea
{

class TextFormat : public JsonSerializable
{
public:
	TextFormat();
	bool operator==(const TextFormat &format) const;

	json toJson() const override;
	bool fromJson(const json &j) override;

	void bold(bool val);
	bool bold() const;
	void italic(bool val);
	bool italic() const;
	void underline(bool val);
	bool underline() const;

	void size(int val);
	int size() const;
	void color(const sf::Color val);
	const sf::Color &color() const;

private:
	bool _bold      = false;
	bool _italic    = false;
	bool _underline = false;
	int  _size      = 12;

	sf::Color _color = sf::Color::Black;
};

sf::Color jsonToColor(const json &j);
json colorToJson(const sf::Color &color);

} // namespace NovelTea

#endif // NOVELTEA_TEXTFORMAT_HPP
