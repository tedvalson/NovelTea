#ifndef NOVELTEA_CONTEXTOBJECT_HPP
#define NOVELTEA_CONTEXTOBJECT_HPP

namespace NovelTea {

class Context;

class ContextObject
{
public:
	ContextObject(Context* context) : m_context(context) {}
	virtual ~ContextObject() {}

	Context* getContext() const { return m_context; }

private:
	Context* m_context;
};

} // namespace NovelTea

#endif // NOVELTEA_CONTEXTOBJECT_HPP
