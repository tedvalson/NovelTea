#ifndef NOVELTEA_JSONSERIALIZABLE_HPP
#define NOVELTEA_JSONSERIALIZABLE_HPP

#include <json.hpp>

using json = nlohmann::json;

namespace NovelTea
{

class JsonSerializable
{
public:
	virtual ~JsonSerializable();
	virtual json toJson() const = 0;
	virtual bool fromJson(const json &j) = 0;
};

} // namespace NovelTea

namespace nlohmann {
template<typename BasicJsonType>
inline void to_json(BasicJsonType& j, const NovelTea::JsonSerializable &obj)
{
	j = obj.toJson();
}
template<typename BasicJsonType>
inline void from_json(const BasicJsonType& j, NovelTea::JsonSerializable &obj)
{
	// TODO: check return and throw error?
	obj.fromJson(j);
}
} // namespace nlohmann

#endif // NOVELTEA_JSONSERIALIZABLE_HPP
