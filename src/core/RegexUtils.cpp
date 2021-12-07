#include <NovelTea/RegexUtils.hpp>

namespace NovelTea
{

void processRegex(const std::string& s, const std::regex& re, RegexCallback callback)
{
	auto iter = std::sregex_iterator{s.begin(), s.end(), re};
	while (iter != std::sregex_iterator{})
		callback(iter++);
}

std::string replaceRegex(const std::string& s, const std::regex& re, RegexStringCallback callback)
{
	std::string result;
	auto prev = RegexIterator{};

	processRegex(s, re, [&](const RegexIterator& it){
		result += it->prefix();
		result += callback(it);
		prev = it;
	});

	if (prev == RegexIterator{})
		result = s;
	else
		result += prev->suffix();

	return result;
}

} // namespace NovelTea
