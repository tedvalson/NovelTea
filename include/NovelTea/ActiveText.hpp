#ifndef NOVELTEA_ACTIVETEXT_HPP
#define NOVELTEA_ACTIVETEXT_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <vector>

namespace NovelTea
{

class TextBlock;

class ActiveText : public JsonSerializable
{
public:
	ActiveText();
	virtual json toJson() const;
	virtual bool fromJson(const json &j);

	std::string toPlainText() const;

	const std::vector<std::shared_ptr<TextBlock>> &blocks() const;
	void addBlock(std::shared_ptr<TextBlock> block, int index = -1);

private:
	std::vector<std::shared_ptr<TextBlock>> m_textBlocks;
};

} // namespace NovelTea

#endif // NOVELTEA_ACTIVETEXT_HPP
