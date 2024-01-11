#ifndef NOVELTEA_STRINGUTILS_HPP
#define NOVELTEA_STRINGUTILS_HPP

#include <SFML/Graphics/Text.hpp>
#include <vector>
#include <string>
#include <cstring>

namespace NovelTea
{

std::vector<std::string> split(const std::string &text, const std::string &delimiter = "\n");
std::string join(const std::vector<std::string> &words, const std::string &delimiter = "\n");
std::string replace(const std::string &text, const std::string &textToReplace, const std::string &replacement = "");
std::string trimLeft(const std::string &text);
bool wrapText(sf::Text &text, float width);

inline bool IsDigit(char c)
{
	return ('0' <= c && c <= '9');
}

inline bool IsAlpha(char c)
{
	static const char alpha[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	return (std::strchr(alpha, c) != nullptr);
}

inline bool IsAlNum(char c)
{
	return IsAlpha(c) || IsDigit(c);
}

inline bool IsSpace(char c)
{
	return std::isspace(static_cast<unsigned char>(c)) != 0;
}

} // namespace NovelTea

#endif // NOVELTEA_STRINGUTILS_HPP
