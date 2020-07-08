#ifndef NOVELTEA_SCRIPT_HPP
#define NOVELTEA_SCRIPT_HPP

#include <NovelTea/Entity.hpp>

namespace NovelTea
{

class Script : public Entity
{
public:
	Script();
	size_t jsonSize() const override;
	json toJson() const override;
	void loadJson(const json &j) override;

	static constexpr auto id = "script";

	ADD_ACCESSOR(std::string, Content, m_content)

private:
	std::string m_content;
};

} // namespace NovelTea

#endif // NOVELTEA_SCRIPT_HPP
