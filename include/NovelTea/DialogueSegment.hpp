#ifndef NOVELTEA_DIALOGUESEGMENT_HPP
#define NOVELTEA_DIALOGUESEGMENT_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>
#include <memory>
#include <vector>

namespace NovelTea
{

class DialogueSegment : public JsonSerializable
{
public:
	enum Type {
		Invalid   =-1,
		Root      = 0,
		Text      = 1,
		Link      = 2,
	};

	DialogueSegment();

	json toJson() const override;
	bool fromJson(const json &j) override;

	void appendChild(int id);
	ADD_ACCESSOR(Type, Type, m_type)
	ADD_ACCESSOR(std::string, Text, m_text)
	ADD_ACCESSOR(std::vector<int>, ChildrenIds, m_childrenIds)

	static std::shared_ptr<DialogueSegment> createSegment(const json &j);

private:
	int m_id;
//	int m_parentId;
	int m_linkId;
	Type m_type;
	std::string m_text;
	std::vector<int> m_childrenIds;
};

} // namespace NovelTea

#endif // NOVELTEA_DIALOGUESEGMENT_HPP
