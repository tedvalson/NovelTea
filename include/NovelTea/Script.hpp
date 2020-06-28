#ifndef NOVELTEA_SCRIPT_HPP
#define NOVELTEA_SCRIPT_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>

namespace NovelTea
{

class Script : public JsonSerializable
{
public:
	Script();
	json toJson() const override;
	bool fromJson(const json &j) override;

	static constexpr auto id = "script";

	ADD_ACCESSOR(std::string, Name, m_name)

private:
	std::string m_name;
};

} // namespace NovelTea

#endif // NOVELTEA_SCRIPT_HPP
