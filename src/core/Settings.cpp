#include <NovelTea/Settings.hpp>
#include <SFML/System/Err.hpp>
#include <fstream>

namespace
{
	const auto fileName = "/settings.conf";
	const auto propFontSizeMultiplier = "sizeFactor";
}

namespace NovelTea
{

Settings::Settings()
: m_fontSizeMultiplier(1.f)
{
}

Settings &Settings::get()
{
	static Settings obj;
	return obj;
}

void Settings::load()
{
	try
	{
		std::ifstream file(m_directory + fileName);
		if (!file.is_open())
			return;

		std::string s;
		file.seekg(0, std::ios_base::end);
		s.resize(file.tellg());
		file.seekg(0);
		file.read(&s[0], s.size());

		auto j = sj::JSON::Load(s);
		m_fontSizeMultiplier = j[propFontSizeMultiplier].ToFloat();
	}
	catch (std::exception &e)
	{
		sf::err() << "Failed to load game settings." << std::endl;
		sf::err() << e.what() << std::endl;
	}
}

void Settings::save() const
{
	std::ofstream file(m_directory + fileName);
	auto j = sj::JSON({
		propFontSizeMultiplier, m_fontSizeMultiplier,
	});
	file << j;
}

} // namespace NovelTea
