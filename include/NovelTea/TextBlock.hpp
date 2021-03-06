#ifndef NOVELTEA_TEXTBLOCK_HPP
#define NOVELTEA_TEXTBLOCK_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <memory>
#include <vector>

namespace NovelTea
{

class TextFragment;

class TextBlock : public JsonSerializable
{
public:
	enum Alignment {
		Left      = 0,
		Center    = 1,
		Right     = 2,
		Justified = 3,
	};

	TextBlock();
	json toJson() const override;
	bool fromJson(const json &j) override;

	const std::vector<std::shared_ptr<TextFragment>> &fragments() const;
	void addFragment(std::shared_ptr<TextFragment> fragment, int index = -1);

	void setAlignment(Alignment alignment);
	Alignment getAlignment() const;

private:
	Alignment m_alignment;
	std::vector<std::shared_ptr<TextFragment>> m_textFragments;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTBLOCK_HPP
