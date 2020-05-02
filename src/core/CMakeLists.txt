set(SRCROOT ${PROJECT_SOURCE_DIR}/src/core)

FILE(GLOB_RECURSE TWEEN_ENGINE_SOURCE ${PROJECT_SOURCE_DIR}/external/TweenEngine/src/*.cpp)

set(SOURCE_FILES
	${SRCROOT}/Engine.cpp
	${SRCROOT}/States/StateStack.cpp
	${SRCROOT}/States/State.cpp
	${SRCROOT}/States/StateMain.cpp
	${SRCROOT}/JsonSerializable.cpp
	${SRCROOT}/Cutscene.cpp
	${SRCROOT}/ProjectData.cpp
	${SRCROOT}/ScriptManager.cpp
	${SRCROOT}/TextFormat.cpp
	${SRCROOT}/CutsceneRenderer.cpp
	${SRCROOT}/CutsceneSegment.cpp
	${SRCROOT}/CutsceneTextSegment.cpp
	${SRCROOT}/ActiveText.cpp
	${SRCROOT}/TextFragment.cpp
	${SRCROOT}/TextBlock.cpp
	${SRCROOT}/CutscenePageBreakSegment.cpp	
	${PROJECT_SOURCE_DIR}/external/duktape.c
	${TWEEN_ENGINE_SOURCE}
)

add_library(NovelTea ${SOURCE_FILES})

add_executable(NovelTeaLauncher ${SRCROOT}/main.cpp)
target_link_libraries(NovelTeaLauncher NovelTea sfml-graphics sfml-window sfml-system sfml-audio)
