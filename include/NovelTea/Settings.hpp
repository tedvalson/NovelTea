#ifndef NOVELTEA_SETTINGS_HPP
#define NOVELTEA_SETTINGS_HPP

#include <NovelTea/json.hpp>
#include <NovelTea/Utils.hpp>

#define GSettings NovelTea::Settings::get()

namespace NovelTea
{

class Settings
{
public:
	static Settings &get();
	void load();
	void save() const;

	ADD_ACCESSOR(std::string, Directory, m_directory)
	ADD_ACCESSOR(float, FontSizeMultiplier, m_fontSizeMultiplier)

protected:
	Settings();

private:
	sj::JSON m_json;
	std::string m_directory;

	float m_fontSizeMultiplier;
};

} // namespace NovelTea

#endif // NOVELTEA_SETTINGS_HPP
