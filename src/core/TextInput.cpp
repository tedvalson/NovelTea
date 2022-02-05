#include <NovelTea/TextInput.hpp>
#include <SFML/System/Err.hpp>

namespace NovelTea
{

TextInput::TextInput()
: textInputTrigger(nullptr)
, m_refCount(0)
{
}

TextInput &TextInput::get()
{
	static TextInput obj;
	return obj;
}

void TextInput::clean()
{
	if (m_refCount == 0)
		m_list.clear();
}

void TextInput::run(const std::string &text, GetTextInputCallback callback)
{
	clean();
	if (textInputTrigger)
	{
		auto ref = m_list.size();
		m_list.emplace_back(new TextInputInstance(callback));
		m_refCount++;
		textInputTrigger(text, ref);
	}
	else if (callback)
		callback("NULL");
}

void TextInput::callback(const std::string &result, int ref)
{
	m_list[ref]->callback(result);
	m_refCount--;
	clean();
}

bool TextInput::finished()
{
	return m_list.empty();
}

TextInputInstance::TextInputInstance(GetTextInputCallback callback)
: m_callback(callback)
{

}

void TextInputInstance::callback(const std::string &result)
{
	if (m_callback)
		m_callback(result);
}

} // namespace NovelTea
