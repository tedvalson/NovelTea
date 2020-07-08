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

	ADD_ACCESSOR(std::string, Id, m_id)
	ADD_ACCESSOR(std::string, Content, m_content)

private:
	std::string m_id;
	std::string m_content;
};

} // namespace NovelTea

#endif // NOVELTEA_SCRIPT_HPP
