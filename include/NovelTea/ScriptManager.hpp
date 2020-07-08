#ifndef NOVELTEA_SCRIPTMANAGER_HPP
#define NOVELTEA_SCRIPTMANAGER_HPP

#include <dukglue/dukglue.h>

namespace NovelTea
{

class ScriptManager
{
public:
	ScriptManager();
	~ScriptManager();
	void reset();

	void runScriptId(const std::string &scriptId);

	inline void run(const std::string &script)
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

	template <typename... Args>
	void call(const std::string &script, const std::string &funcName, Args&&... args)
	{
		call<void>(script, funcName, args...);
	}

	template <typename T, typename... Args>
	T call(const std::string &script, const std::string &funcName, Args&&... args)
	{
		auto s = script+";"+funcName+";";
		auto fn = dukglue_peval<DukValue>(m_context, s.c_str());
		return dukglue_pcall<T>(m_context, fn, std::forward<Args>(args)...);
	}

protected:
	void registerFunctions();
	void registerClasses();
	void registerGlobals();

private:
	duk_context *m_context;
};

} // namespace NovelTea

#endif // NOVELTEA_SCRIPTMANAGER_HPP
