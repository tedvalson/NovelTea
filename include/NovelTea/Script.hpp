#ifndef NOVELTEA_SCRIPT_HPP
#define NOVELTEA_SCRIPT_HPP

#include <NovelTea/Entity.hpp>

namespace NovelTea
{

class Script : public Entity
{
public:
	Script(Context *context);
	size_t jsonSize() const override;
	json toJson() const override;
	void loadJson(const json &j) override;
	EntityType entityType() const override;

	static constexpr auto id = ID::Script;
	const std::string entityId() const override {return id;}

	ADD_ACCESSOR(bool, Autorun, m_autorun)
	ADD_ACCESSOR(std::string, Content, m_content)

private:
	bool m_autorun;
	std::string m_content;
};

} // namespace NovelTea

#endif // NOVELTEA_SCRIPT_HPP
