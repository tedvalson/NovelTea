#include <NovelTea/StringUtils.hpp>

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

std::string join(const std::vector<std::string> &words, const std::string &delimiter)
{
	if (words.empty())
		return "";
	std::string result;
	for (auto &word : words)
		result += delimiter + word;
	return result.substr(delimiter.size());
}

std::string replace(const std::string &text, const std::string &textToReplace, const std::string &replacement)
{
	// TODO: Optimize
	auto v = split(text, textToReplace);
	return join(v, replacement);
}

} // namespace NovelTea
