#ifndef NOVELTEA_SAVEDATA_HPP
#define NOVELTEA_SAVEDATA_HPP

#include <NovelTea/JsonSerializable.hpp>

#define Save NovelTea::SaveData::instance()

namespace NovelTea
{

class SaveData: public JsonSerializable
{
public:
	static SaveData &instance();

	bool isLoaded() const;

	void saveToFile(const std::string &filename = std::string());
	bool loadFromFile(const std::string &filename);
	const std::string &filename() const;

	json toJson() const override;
	bool fromJson(const json &j) override;

	const json &data() const;
	json &data();

	static void setVariables(const std::string &jsonData);
	static std::string getVariables(const std::string &jsonData);

protected:
	SaveData();
	~SaveData();

private:
	bool _loaded = false;
	std::string _filename;
	mutable json _json;
};

} // namespace NovelTea

#endif // NOVELTEA_SAVEDATA_HPP
