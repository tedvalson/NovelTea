#ifndef NOVELTEA_SCRIPTMANAGER_HPP
#define NOVELTEA_SCRIPTMANAGER_HPP

#include <dukglue/dukglue.h>

#define Script NovelTea::ScriptManager::instance()

namespace NovelTea
{

class ScriptManager
{
public:
	static ScriptManager &instance();

	inline void run(const std::string& script)
	{
		run<void>(script);
	}

	template <typename T>
	inline T run(const std::string &script)
	{
		return dukglue_peval<T>(m_context, script.c_str());
	}

	inline void runInClosure(const std::string& script)
	{
		runInClosure<void>(script);
	}

	template <typename T>
	inline T runInClosure(const std::string &script)
	{
		return run<T>("(function(){" + script + "})();");
	}

protected:
	ScriptManager();
	~ScriptManager();

	void registerFunctions();
	void registerClasses();

private:
	duk_context *m_context;
};

} // namespace NovelTea

#endif // NOVELTEA_SCRIPTMANAGER_HPP
