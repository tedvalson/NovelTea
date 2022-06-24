#ifndef NOVELTEA_DIFF_HPP
#define NOVELTEA_DIFF_HPP

#include <string>

#define DIFF_OPEN_TAG "[d]"
#define DIFF_CLOSE_TAG "[/d]"
#define DIFF_OPEN_TAG_LEN 3
#define DIFF_CLOSE_TAG_LEN 4

namespace NovelTea
{

std::string diff(const std::string &oldString, const std::string &newString);
std::string stripDiff(const std::string &diffString);

} // namespace NovelTea

#endif // NOVELTEA_DIFF_HPP
