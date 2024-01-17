#include <NovelTea/StringUtils.hpp>
#include <NovelTea/Err.hpp>
#ifdef ANDROID
#include <SFML/System/FileInputStream.hpp>
#endif
#include <fstream>

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

std::string trimLeft(const std::string &text)
{
	auto result = text;
	int count = 0;
	for (auto& c : text) {
		if (!IsSpace(c))
			break;
		++count;
	}
	if (count > 0)
		result.erase(0, count);
	return result;
}

std::string getFileContents(const std::string &filename)
{
	std::string contents;
#ifdef ANDROID
	sf::FileInputStream file;
	if (file.open(filename))
	{
		contents.resize(file.getSize());
		file.read(&contents[0], contents.size());
	}
#else
	std::ifstream ifs(filename, std::ios::binary);
	if (!ifs.is_open()) {
		err() << "Failed to read file '" << filename << "'" << std::endl;
		return contents;
	}
	ifs.seekg(0, std::ios::end);
	contents.reserve(ifs.tellg());
	ifs.seekg(0, std::ios::beg);
	contents.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
#endif
	return contents;
}

} // namespace NovelTea
