#ifndef NOVELTEA_DIFF_HPP
#define NOVELTEA_DIFF_HPP

#include <string>

#define DIFF_OPEN_TAG "^<"
#define DIFF_CLOSE_TAG ">^"

namespace NovelTea
{

std::string diff(const std::string &oldString, const std::string &newString);
std::string stripDiff(const std::string &diffString);

} // namespace NovelTea

#endif // NOVELTEA_DIFF_HPP
