#ifndef NOVELTEA_SCRIPTMANAGER_HPP
#define NOVELTEA_SCRIPTMANAGER_HPP

#include <dukglue/dukglue.h>
#include <NovelTea/json.hpp>

namespace NovelTea
{

class Game;
class SaveData;
class Script;

class ScriptManager
{
	friend class Game;
public:
	void reset();

	void runScript(std::shared_ptr<Script> script);
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
	ScriptManager(Game *game, SaveData *saveData);
	~ScriptManager();
	void registerFunctions();
	void registerClasses();
	void registerGlobals();

	void runAutorunScripts();
	void checkAutorun(const sj::JSON &j);

private:
	duk_context *m_context;
	Game *m_game;
	SaveData *m_saveData;
};

} // namespace NovelTea

#endif // NOVELTEA_SCRIPTMANAGER_HPP
