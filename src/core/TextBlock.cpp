#include <NovelTea/TextBlock.hpp>
#include <NovelTea/TextFragment.hpp>

namespace NovelTea
{

TextBlock::TextBlock()
	: m_alignment(Alignment::Left)
{
}

json TextBlock::toJson() const
{
	json jfrags = sj::Array();
	for (auto &fragment : m_textFragments)
		jfrags.append(fragment->toJson());

	return sj::Array(
		static_cast<int>(m_alignment),
		jfrags
	);
}

bool TextBlock::fromJson(const json &j)
{
	m_alignment = static_cast<Alignment>(j[0].ToInt());
	for (auto &jfrag : j[1].ArrayRange())
	{
		auto fragment = std::make_shared<TextFragment>();
		if (fragment->fromJson(jfrag))
			m_textFragments.push_back(fragment);
	}
	return true;
}

const std::vector<std::shared_ptr<TextFragment>> &TextBlock::fragments() const
{
	return m_textFragments;
}

void TextBlock::addFragment(std::shared_ptr<TextFragment> fragment, int index)
{
	if (index < 0)
		m_textFragments.push_back(fragment);
	else
		m_textFragments.insert(m_textFragments.begin() + index, fragment);
}

void TextBlock::setAlignment(TextBlock::Alignment alignment)
{
	m_alignment = alignment;
}

TextBlock::Alignment TextBlock::getAlignment() const
{
	return m_alignment;
}

} // namespace NovelTea
