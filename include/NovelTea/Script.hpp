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
	const std::string entityId() const override {return id;}

	ADD_ACCESSOR(bool, Global, m_global)
	ADD_ACCESSOR(bool, Autorun, m_autorun)
	ADD_ACCESSOR(std::string, Content, m_content)

private:
	bool m_global;
	bool m_autorun;
	std::string m_content;
};

} // namespace NovelTea

#endif // NOVELTEA_SCRIPT_HPP
