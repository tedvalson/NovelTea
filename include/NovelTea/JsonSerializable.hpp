#ifndef NOVELTEA_JSONSERIALIZABLE_HPP
#define NOVELTEA_JSONSERIALIZABLE_HPP

#include <NovelTea/json.hpp>

using json = sj::JSON;

namespace NovelTea
{

class JsonSerializable
{
public:
	virtual json toJson() const = 0;
	virtual bool fromJson(const json &j) = 0;
};

} // namespace NovelTea

#endif // NOVELTEA_JSONSERIALIZABLE_HPP
