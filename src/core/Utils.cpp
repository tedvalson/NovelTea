#include <NovelTea/Utils.hpp>

namespace NovelTea
{

std::vector<std::string> split(const std::string &text, const std::string &delimiter)
{
	std::vector<std::string> result;
	size_t pos = 0;
	size_t p = 0;

	while (pos != text.npos)
	{
		pos = text.find(delimiter.c_str(), p, delimiter.size());
		result.push_back(text.substr(p, pos - p));
		p = pos + delimiter.size();
	}
	return result;
}

bool wrapText(sf::Text &text, float width)
{
	if (text.getLocalBounds().width <= width)
		return false;

	auto s = text.getString().toAnsiString();
	auto words = split(s, " ");
	auto processedWidth = 0.f;
	int pos = 0;
	std::string out;
	for (auto &word : words)
	{
		auto p = text.findCharacterPos(pos + word.size());
		if (p.x - processedWidth > width)
		{
			out += "\n" + word + " ";
			pos += word.size() + 1;
			processedWidth += p.x - processedWidth;
		} else {
			out += word + " ";
			pos += word.size() + 1;
		}
	}

	text.setString(out);
	return true;
}

} // namespace NovelTea
