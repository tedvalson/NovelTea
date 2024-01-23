#ifndef NOVELTEA_SUBSYSTEM_HPP
#define NOVELTEA_SUBSYSTEM_HPP

#include <NovelTea/ContextObject.hpp>
#include <string>

namespace NovelTea {

class Subsystem : public ContextObject
{
public:
	Subsystem(Context* context) : ContextObject(context) {}
	virtual ~Subsystem() {}

	virtual bool initialize() { return true; }
	virtual void update(float delta) {}

private:
};

} // namespace NovelTea

#endif // NOVELTEA_SUBSYSTEM_HPP
