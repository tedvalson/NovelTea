#ifndef NOVELTEA_OBJECT_HPP
#define NOVELTEA_OBJECT_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>

namespace NovelTea
{

class Object : public JsonSerializable
{
public:
	Object();
	json toJson() const override;
	bool fromJson(const json &j) override;

	static constexpr auto id = "object";

	ADD_ACCESSOR(std::string, Name, m_name)

private:
	std::string m_name;
};

} // namespace NovelTea

#endif // NOVELTEA_OBJECT_HPP
