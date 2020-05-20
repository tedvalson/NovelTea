#ifndef NOVELTEA_PROJECTDATAIDENTIFIERS_HPP
#define NOVELTEA_PROJECTDATAIDENTIFIERS_HPP

#define NOVELTEA_VERSION 1.0f

#define NT_ENGINE_VERSION  "engine"
#define NT_PROJECT_NAME    "name"
#define NT_PROJECT_VERSION "version"
#define NT_PROJECT_AUTHOR  "author"
#define NT_PROJECT_WEBSITE "website"
#define NT_PROJECT_FONTS   "fonts"
#define NT_PROJECT_ENTRYPOINT "entrypoint"
#define NT_FONT_DEFAULT    "fontDefault"
#define NT_ENTITY_TYPE     0
#define NT_ENTITY_ID       1
#define NT_CUTSCENES       "cutscenes"
#define NT_CUTSCENE_TYPE   0
#define NT_CUTSCENE_TEXT   0
#define NT_CUTSCENE_BREAK  1
#define NT_TEXTFORMATS     "textformats"

// Actually for SaveData
#define NT_VARIABLES       "variables"

namespace NovelTea
{

enum class EntityType : int {
	Invalid      = -1,
	CustomScript = 0,
	Cutscene     = 1,
};

} // namespace NovelTea

#endif // NOVELTEA_PROJECTDATAIDENTIFIERS_HPP
