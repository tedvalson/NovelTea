#ifndef NOVELTEA_TEXTINPUT_HPP
#define NOVELTEA_TEXTINPUT_HPP

#include <string>
#include <vector>
#include <memory>
#include <functional>

#define GTextInput NovelTea::TextInput::get()

namespace NovelTea
{

using GetTextInputTrigger  = std::function<void(const std::string&, int)>;
using GetTextInputCallback = std::function<void(const std::string&)>;

class TextInputInstance
{
public:
	TextInputInstance(GetTextInputCallback callback);
	void callback(const std::string &result);

private:
	GetTextInputCallback m_callback;
};

class TextInput
{
public:
	static TextInput &get();

	void run(const std::string &text, GetTextInputCallback callback);
	void callback(const std::string &result, int ref);
	bool finished();

	GetTextInputTrigger textInputTrigger;

protected:
	TextInput();
	void clean();

private:
	std::vector<std::unique_ptr<TextInputInstance>> m_list;
	int m_refCount;
};

} // namespace NovelTea

#endif // NOVELTEA_TEXTINPUT_HPP
