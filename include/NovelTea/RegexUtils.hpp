#ifndef NOVELTEA_REGEXUTILS_HPP
#define NOVELTEA_REGEXUTILS_HPP

#include <regex>

namespace NovelTea
{

using RegexIterator = std::sregex_iterator;
using RegexCallback = std::function<void(const RegexIterator&)>;
using RegexStringCallback = std::function<std::string(const RegexIterator&)>;

void processRegex(const std::string& s, const std::regex& re, RegexCallback callback);
std::string replaceRegex(const std::string& s, const std::regex& re, RegexStringCallback callback);

} // namespace NovelTea

#endif // NOVELTEA_REGEXUTILS_HPP
