#ifndef NOVELTEA_OBJECT_HPP
#define NOVELTEA_OBJECT_HPP

#include <NovelTea/Entity.hpp>

namespace NovelTea
{

class Object : public Entity
{
public:
	Object();
	size_t jsonSize() const override;
	json toJson() const override;
	void loadJson(const json &j) override;

	static constexpr auto id = "object";
	const std::string entityId() const override {return id;}

	ADD_ACCESSOR(std::string, Name, m_name)

private:
	std::string m_name;
};

} // namespace NovelTea

#endif // NOVELTEA_OBJECT_HPP
