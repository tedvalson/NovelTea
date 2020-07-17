#ifndef NOVELTEA_DIALOGUE_HPP
#define NOVELTEA_DIALOGUE_HPP

#include <NovelTea/Entity.hpp>

namespace NovelTea
{

class Dialogue : public Entity
{
public:
	Dialogue();
	size_t jsonSize() const override;
	json toJson() const override;
	void loadJson(const json &j) override;

	static constexpr auto id = "dialogue";
	const std::string entityId() const override {return id;}

	ADD_ACCESSOR(std::string, Name, m_name)

private:
	std::string m_name;
};

} // namespace NovelTea

#endif // NOVELTEA_DIALOGUE_HPP
