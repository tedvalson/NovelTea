set(SRCROOT ${PROJECT_SOURCE_DIR}/src/core)

FILE(GLOB_RECURSE TWEEN_ENGINE_SOURCE ${PROJECT_SOURCE_DIR}/external/TweenEngine/src/*.cpp)

set(SOURCE_FILES
	${SRCROOT}/Action.cpp
	${SRCROOT}/AssetManager.cpp
	${SRCROOT}/BBCodeParser.cpp
	${SRCROOT}/Context.cpp
	${SRCROOT}/Cutscene.cpp
	${SRCROOT}/CutscenePageBreakSegment.cpp
	${SRCROOT}/CutscenePageSegment.cpp
	${SRCROOT}/CutscenePlayer.cpp
	${SRCROOT}/CutsceneScriptSegment.cpp
	${SRCROOT}/CutsceneSegment.cpp
	${SRCROOT}/CutsceneTextSegment.cpp
	${SRCROOT}/Dialogue.cpp
	${SRCROOT}/DialoguePlayer.cpp
	${SRCROOT}/DialogueSegment.cpp
	${SRCROOT}/Diff.cpp
	${SRCROOT}/Engine.cpp
	${SRCROOT}/Entity.cpp
	${SRCROOT}/Err.cpp
	${SRCROOT}/Event.cpp
	${SRCROOT}/FileUtils.cpp
	${SRCROOT}/Game.cpp
	${SRCROOT}/json.cpp
	${SRCROOT}/Map.cpp
	${SRCROOT}/Notification.cpp
	${SRCROOT}/Object.cpp
	${SRCROOT}/ObjectList.cpp
	${SRCROOT}/Profile.cpp
	${SRCROOT}/ProjectData.cpp
	${SRCROOT}/PropertyList.cpp
	${SRCROOT}/RegexUtils.cpp
	${SRCROOT}/Room.cpp
	${SRCROOT}/SaveData.cpp
	${SRCROOT}/Script.cpp
	${SRCROOT}/ScriptManager.cpp
	${SRCROOT}/Settings.cpp
	${SRCROOT}/StateEventManager.cpp
	${SRCROOT}/StringUtils.cpp
	${SRCROOT}/TextInput.cpp
	${SRCROOT}/TextLog.cpp
	${SRCROOT}/Timer.cpp
	${SRCROOT}/Utils.cpp
	${SRCROOT}/Verb.cpp
	${SRCROOT}/Zip/zip.c
	${SRCROOT}/Zip/Zip.cpp

	${PROJECT_SOURCE_DIR}/external/duktape.c
	${TWEEN_ENGINE_SOURCE}
)

add_library(NovelTea ${SOURCE_FILES})

install(TARGETS NovelTea)
