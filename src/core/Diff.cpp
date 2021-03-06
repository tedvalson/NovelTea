#include <NovelTea/Diff.hpp>
#include <dtl/dtl.hpp>

namespace NovelTea
{

std::string stripEmptyDiff(const std::string &diffString)
{
	auto result = diffString;
	auto pos = 0;
	while ((pos = result.find("^[]^")) != result.npos)
		result.erase(pos, 4);
	while ((pos = result.find("]^^[")) != result.npos)
		result.erase(pos, 4);
	return result;
}

bool isSnapChar(char c)
{
	return (c == ' ' || c == '\n' || c == '\t' || c == ',');
}

std::string snapDiffToWord(const std::string &diffString)
{
	auto result = stripEmptyDiff(diffString);
	size_t startPos = 0;
	while ((startPos = result.find("^[", startPos)) != result.npos)
	{
		if (startPos == 0 ||
				isSnapChar(result[startPos-1]) ||
				isSnapChar(result[startPos+2]))
		{
			startPos++;
			continue;
		}

		for (int i = startPos-1; i >= 0; --i) {
			if (isSnapChar(result[i])) {
				result.erase(startPos, 2);
				result.insert(i, "^[");
				break;
			}
		}

		startPos++;
	}

	startPos = 0;
	while ((startPos = result.find("]^", startPos)) != result.npos)
	{
		if (startPos == result.size() - 2 ||
				isSnapChar(result[startPos-1]) ||
				isSnapChar(result[startPos+2]))
		{
			startPos++;
			continue;
		}

		for (int i = startPos+3; i < result.size(); ++i) {
			if (isSnapChar(result[i])) {
				result.insert(i, "]^");
				result.erase(startPos, 2);
				break;
			}
		}

		startPos++;
	}

	return stripEmptyDiff(result);
}

std::string snapDiffToObject(const std::string &diffString)
{
	auto result = diffString;
	std::vector<std::pair<int,int>> objectBounds;
	auto pos = 0;
	while ((pos = diffString.find("[[", pos)) != diffString.npos)
	{
		auto endPos = diffString.find("]]", pos);
		if (endPos != diffString.npos)
			objectBounds.emplace_back(pos, endPos);
		++pos;
	}
	for (auto &bounds : objectBounds)
	{
		pos = 0;
		while ((pos = result.find("^[", pos)) != diffString.npos)
		{
			if (pos > bounds.first && pos < bounds.second)
			{
				result.erase(pos, 2);
				result.insert(bounds.first, "^[");
			}
			++pos;
		}
		pos = 0;
		while ((pos = result.find("]^", pos)) != diffString.npos)
		{
			if (pos > bounds.first && pos < bounds.second)
			{
				result.insert(bounds.second + 2, "]^");
				result.erase(pos, 2);
			}
			++pos;
		}
	}

	return result;
}

std::string removeNestedDiff(const std::string &diffString)
{
	if (diffString.size() <= 2)
		return diffString;

	auto result = diffString;
	int count = 0;
	for (int i = 0; i < result.size()-2;)
	{
		auto s = result.substr(i, 2);
		if (s == "^[") {
			count++;
			if (count > 1)
				result.erase(i, 2);
			else
				i++;
		}
		else if (s == "]^") {
			count--;
			if (count > 0)
				result.erase(i, 2);
			else
				i++;
		}
		else
			i++;
	}
	return result;
}

std::string diff(const std::string &oldString, const std::string &newString)
{
	if (oldString.empty() || newString.empty() || oldString == newString)
		return newString;

	std::string result;
	dtl::Diff<char, std::string> diff(oldString, newString);
	diff.compose();

	auto startNew = false;
	auto ses = diff.getSes().getSequence();
	for (auto s : ses)
	{
		if (s.second.type == dtl::SES_ADD) {
			if (!startNew) {
				startNew = true;
				result += "^[";
			}
		} else if (startNew) {
			startNew = false;
			result += "]^";
		}
		
		if (s.second.type != dtl::SES_DELETE) {
			if (startNew && s.first == '\n')
				result += "]^\n^[";
			else
				result += s.first;
		}
	}
	if (startNew)
		result += "]^";

	return removeNestedDiff(snapDiffToObject(snapDiffToWord(result)));
}

std::string stripDiff(const std::string &diffString)
{
	auto result = diffString;
	auto pos = 0;
	while ((pos = result.find("^[", pos)) != result.npos)
		result.erase(pos, 2);
	pos = 0;
	while ((pos = result.find("]^", pos)) != result.npos)
		result.erase(pos, 2);
	return result;
}

} // namespace NovelTea
