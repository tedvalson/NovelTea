set(SRCROOT ${PROJECT_SOURCE_DIR}/src/core)

FILE(GLOB_RECURSE TWEEN_ENGINE_SOURCE ${PROJECT_SOURCE_DIR}/external/TweenEngine/src/*.cpp)

set(SOURCE_FILES
	${SRCROOT}/Action.cpp
	${SRCROOT}/ActiveText.cpp
	${SRCROOT}/Cutscene.cpp
	${SRCROOT}/CutscenePageBreakSegment.cpp
	${SRCROOT}/CutsceneRenderer.cpp
	${SRCROOT}/CutsceneSegment.cpp
	${SRCROOT}/CutsceneTextSegment.cpp
	${SRCROOT}/Dialogue.cpp
	${SRCROOT}/DialogueRenderer.cpp
	${SRCROOT}/DialogueSegment.cpp
	${SRCROOT}/Diff.cpp
	${SRCROOT}/Engine.cpp
	${SRCROOT}/Entity.cpp
	${SRCROOT}/Game.cpp
	${SRCROOT}/json.cpp
	${SRCROOT}/Object.cpp
	${SRCROOT}/ObjectList.cpp
	${SRCROOT}/ProjectData.cpp
	${SRCROOT}/PropertyList.cpp
	${SRCROOT}/Room.cpp
	${SRCROOT}/SaveData.cpp
	${SRCROOT}/Script.cpp
	${SRCROOT}/ScriptManager.cpp
	${SRCROOT}/TextBlock.cpp
	${SRCROOT}/TextFormat.cpp
	${SRCROOT}/TextFragment.cpp
	${SRCROOT}/Timer.cpp
	${SRCROOT}/Utils.cpp
	${SRCROOT}/Verb.cpp
	${SRCROOT}/GUI/ActionBuilder.cpp
	${SRCROOT}/GUI/Button.cpp
	${SRCROOT}/GUI/Hideable.cpp
	${SRCROOT}/GUI/Inventory.cpp
	${SRCROOT}/GUI/Navigation.cpp
	${SRCROOT}/GUI/NinePatch.cpp
	${SRCROOT}/GUI/Notification.cpp
	${SRCROOT}/GUI/Scrollable.cpp
	${SRCROOT}/GUI/ScrollBar.cpp
	${SRCROOT}/GUI/TextOverlay.cpp
	${SRCROOT}/GUI/VerbList.cpp
	${SRCROOT}/GUI/Icon/IconGlow.cpp
	${SRCROOT}/States/State.cpp
	${SRCROOT}/States/StateEditor.cpp
	${SRCROOT}/States/StateIntro.cpp
	${SRCROOT}/States/StateMain.cpp
	${SRCROOT}/States/StateSettings.cpp
	${SRCROOT}/States/StateStack.cpp
	${SRCROOT}/States/StateTextLog.cpp
	${SRCROOT}/States/StateTitleScreen.cpp
	
	${PROJECT_SOURCE_DIR}/external/duktape.c
	${TWEEN_ENGINE_SOURCE}
)

add_library(NovelTea ${SOURCE_FILES})
target_link_libraries(NovelTea sfml-graphics)

install(TARGETS NovelTea
	RUNTIME DESTINATION bin COMPONENT bin
	LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT bin
	ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT devel)

if (NOT ANDROID)
	add_executable(NovelTeaLauncher ${SRCROOT}/main.cpp)
	target_link_libraries(NovelTeaLauncher NovelTea)
endif()
