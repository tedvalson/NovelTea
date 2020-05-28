#ifndef NOVELTEA_PROJECTDATAIDENTIFIERS_HPP
#define NOVELTEA_PROJECTDATAIDENTIFIERS_HPP

#define NOVELTEA_VERSION 1.0f

namespace NovelTea
{

enum class EntityType : int {
	Invalid      = -1,
	CustomScript = 0,
	Cutscene     = 1,
	Room,
	Object,
	Dialogue,
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
	const auto cutscenes          = "cutscenes";
	const auto cutsceneSegType    = 0;
	const auto cutsceneSegText    = 0;
	const auto cutsceneSegBreak   = 1;
	const auto rooms              = "rooms";
	const auto textFormats        = "textformats";
	const auto objects            = "objects";
	const auto dialogues          = "dialogues";

	// For SaveData
	const auto variables          = "variabes";

} // namespace ID

} // namespace NovelTea

#endif // NOVELTEA_PROJECTDATAIDENTIFIERS_HPP
