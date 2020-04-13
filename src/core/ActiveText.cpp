#include <NovelTea/ActiveText.hpp>
#include <NovelTea/TextBlock.hpp>
#include <NovelTea/TextFragment.hpp>

namespace NovelTea
{

ActiveText::ActiveText()
{
}

json ActiveText::toJson() const
{
	json j = json::array();
	for (auto &block : m_textBlocks)
		j.push_back(*block);
	return j;
}

bool ActiveText::fromJson(const json &j)
{
	m_textBlocks.clear();

	for (auto &jblock : j)
	{
		auto block = std::make_shared<TextBlock>();
		if (block->fromJson(jblock))
			m_textBlocks.push_back(block);
	}
	return true;
}

std::string ActiveText::toPlainText() const
{
	std::string result;
	bool processedFirstBlock = false;
	for (auto &block : blocks())
	{
		if (processedFirstBlock)
			result += " | ";
		processedFirstBlock = true;
		for (auto &frag : block->fragments())
			result += frag->getText();
	}
	return result;
}

const std::vector<std::shared_ptr<TextBlock>> &ActiveText::blocks() const
{
	return m_textBlocks;
}

void ActiveText::addBlock(std::shared_ptr<TextBlock> block, int index)
{
	if (index < 0)
		m_textBlocks.push_back(block);
	else
		m_textBlocks.insert(m_textBlocks.begin() + index, block);
}

} // namespace NovelTea
