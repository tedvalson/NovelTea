#ifndef NOVELTEA_DIALOGUE_HPP
#define NOVELTEA_DIALOGUE_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>

namespace NovelTea
{

class Dialogue : public JsonSerializable
{
public:
	Dialogue();
	json toJson() const override;
	bool fromJson(const json &j) override;

	static constexpr auto id = "dialogue";

	ADD_ACCESSOR(std::string, Id, m_id)
	ADD_ACCESSOR(std::string, Name, m_name)

private:
	std::string m_id;
	std::string m_name;
};

} // namespace NovelTea

#endif // NOVELTEA_DIALOGUE_HPP
