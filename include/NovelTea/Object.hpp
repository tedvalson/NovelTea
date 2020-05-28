#ifndef NOVELTEA_OBJECT_HPP
#define NOVELTEA_OBJECT_HPP

#include <NovelTea/JsonSerializable.hpp>

namespace NovelTea
{

class Object : public JsonSerializable
{
public:
	Object();
	~Object();
	json toJson() const override;
	bool fromJson(const json &j) override;

private:
	std::string m_name;
};

} // namespace NovelTea

#endif // NOVELTEA_OBJECT_HPP
