#ifndef NOVELTEA_PROJECTDATAIDENTIFIERS_HPP
#define NOVELTEA_PROJECTDATAIDENTIFIERS_HPP

#define NOVELTEA_VERSION 1.0f

namespace NovelTea
{

enum class EntityType : int {
	Invalid      = -1,
	CustomScript = 0,
	Cutscene     = 1,
	Action,
	Room,
	Object,
	Dialogue,
	Script,
	Verb,
	Map,
};

namespace ID
{
	const auto engineVersion      = "engine";
	const auto engineFonts        = "sysfonts";
	const auto projectName        = "name";
	const auto projectVersion     = "version";
	const auto projectAuthor      = "author";
	const auto projectWebsite     = "website";
	const auto projectFonts       = "fonts";
	const auto projectFontDefault = "fontDefault";
	const auto startingInventory  = "startInv";
	const auto entrypointEntity   = "entrypoint";
	const auto entrypointMetadata = "entrydata";
	const auto quickVerb          = "quickVerb";
	const auto scriptBeforeSave   = "sbs";
	const auto scriptAfterLoad    = "sas";
	const auto scriptAfterAction  = "saa";
	const auto scriptBeforeAction = "sba";
	const auto scriptUndefinedAction = "sua";
	const auto scriptAfterEnter   = "sae";
	const auto scriptBeforeEnter  = "sbe";
	const auto scriptAfterLeave   = "sal";
	const auto scriptBeforeLeave  = "sbl";
	const auto selectEntityType   = 0;
	const auto selectEntityId     = 1;
	const auto entityId           = 0;
	const auto entityParentId     = 1;
	const auto entityProperties   = 2;
	const auto cutsceneSegType    = 0;
	const auto player             = "player";

	// Shaders
	const auto shaders            = "shaders";
	const auto systemShaders      = "systemShaders";
	const auto shaderBackground   = 0;
	const auto shaderPostProcess  = 1;
	const auto textures           = "textures";

	// Entity IDs
	constexpr auto Action   = "action";
	constexpr auto Cutscene = "cutscene";
	constexpr auto Dialogue = "dialogue";
	constexpr auto Map      = "map";
	constexpr auto Object   = "object";
	constexpr auto Room     = "room";
	constexpr auto Script   = "script";
	constexpr auto Verb     = "verb";

	// For Editor
	const auto tests              = "tests";
	const auto testScriptInit     = "init";
	const auto testScriptCheck    = "check";
	const auto testSteps          = "steps";
	const auto openTabs           = "tabs";
	const auto openTabIndex       = "tabIndex";
	const auto entityColors       = "colors";
	const auto entityPreview      = "preview";
	const auto spellWhitelist     = "whitelist";

	// For SaveData
	const auto entityQueue        = "entityQueue";
	const auto log                = "log";
	const auto map                = "map";
	const auto mapEnabled         = "mapEnabled";
	const auto navigationEnabled  = "navEnabled";
	const auto objectLocations    = "objectLocations";
	const auto playTime           = "playTime";
	const auto properties         = "properties";
	const auto roomDescriptions   = "roomDescriptions";
	const auto visitedRooms       = "visitedRooms";

} // namespace ID

} // namespace NovelTea

#endif // NOVELTEA_PROJECTDATAIDENTIFIERS_HPP
