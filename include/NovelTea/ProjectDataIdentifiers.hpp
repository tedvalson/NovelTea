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
};

namespace ID
{
	const auto engineVersion      = "engine";
	const auto projectName        = "name";
	const auto projectVersion     = "version";
	const auto projectAuthor      = "author";
	const auto projectWebsite     = "website";
	const auto projectEntrypoint  = "entrypoint";
	const auto projectFonts       = "fonts";
	const auto projectFontDefault = "fontDefault";
	const auto entityType         = 0;
	const auto entityId           = 1;
	const auto cutsceneSegType    = 0;
	const auto cutsceneSegText    = 0;
	const auto cutsceneSegBreak   = 1;
	const auto textFormats        = "textformats";

	// For SaveData

} // namespace ID

} // namespace NovelTea

#endif // NOVELTEA_PROJECTDATAIDENTIFIERS_HPP
