#ifndef NOVELTEA_SCRIPTMANAGER_HPP
#define NOVELTEA_SCRIPTMANAGER_HPP

#include <NovelTea/Subsystem.hpp>
#include <NovelTea/Err.hpp>
#include <dukglue/dukglue.h>
#include <NovelTea/json.hpp>
#include <random>

namespace NovelTea
{

class Entity;
class Game;
class Script;

const std::string strUseStrict = "\n\"use strict\";\n";

class ScriptManager : public Subsystem
{
public:
	ScriptManager(Context* context);
	~ScriptManager();

	static std::string name() { return "ScriptManager"; }

	virtual void reset();

	virtual DukValue runScript(std::shared_ptr<Script> script);
	virtual DukValue runScriptId(const std::string &scriptId);

	virtual bool runActionScript(const std::string &verbId, const std::vector<std::string> &objectIds, const std::string &script);
	virtual bool runActionScript(const std::string &verbId, const std::string &verbIdOrig, const std::vector<std::string> &objectIds);
	virtual bool runActionScript(const std::string &verbId, const std::vector<std::string> &objectIds);

	void setActiveEntity(std::shared_ptr<Entity> entity);

	virtual std::string evalExpressions(const std::string &s);
	virtual void getTextInput(const std::string &message, const DukValue &func);

	virtual void randSeed(int seed);
	virtual double randGen();

	inline void run(const std::string &script)
	{
		run<void>(script);
	}

	template <typename T>
	inline T run(const std::string &script)
	{
		try
		{
			auto s = strUseStrict + script;
			return dukglue_peval<T>(m_context, s.c_str());
		}
		catch (std::exception &e)
		{
			err() << "ScriptManager::run() " << e.what() << std::endl;
			throw e;
		}
	}

	inline void runInClosure(const std::string& script)
	{
		runInClosure<void>(script);
	}

	template <typename T>
	inline T runInClosure(const std::string &script)
	{
		return run<T>("(function(){\n" + script + "\n})();");
	}

	template <typename T>
	T call(const DukValue &func)
	{
		try
		{
			return dukglue_pcall<T>(m_context, func);
		}
		catch (std::exception &e)
		{
			err() << e.what() << std::endl;
			throw e;
		}
	}

	template <typename... Args>
	void call(const std::string &script, const std::string &funcName, Args&&... args)
	{
		call<void>(script, funcName, args...);
	}

	template <typename T, typename... Args>
	T call(const std::string &script, const std::string &funcName, Args&&... args)
	{
		try
		{
			auto s = strUseStrict + "\n" + script + ";\n" + funcName + ";";
			auto fn = dukglue_peval<DukValue>(m_context, s.c_str());
			return dukglue_pcall<T>(m_context, fn, std::forward<Args>(args)...);
		}
		catch (std::exception &e)
		{
			err() << e.what() << std::endl;
			throw e;
		}
	}

protected:
	void registerFunctions();
	void registerClasses();
	void registerGlobals();

	void runAutorunScripts();
	void checkAutorun(const sj::JSON &j);

private:
	duk_context *m_context;

	std::default_random_engine m_randEngine;
	std::uniform_real_distribution<double> m_uniformDist;
	int m_randSeed;
};

} // namespace NovelTea

#endif // NOVELTEA_SCRIPTMANAGER_HPP
