set(SRCROOT ${PROJECT_SOURCE_DIR}/src/core)

set(SOURCE_FILES
	${SRCROOT}/ActiveText.cpp
	${SRCROOT}/ActiveTextSegment.cpp
	${SRCROOT}/CutsceneRenderer.cpp
	${SRCROOT}/DialogueRenderer.cpp
	${SRCROOT}/MapRenderer.cpp
	${SRCROOT}/GUI/ActionBuilder.cpp
	${SRCROOT}/GUI/Button.cpp
	${SRCROOT}/GUI/Hideable.cpp
	${SRCROOT}/GUI/Inventory.cpp
	${SRCROOT}/GUI/Navigation.cpp
	${SRCROOT}/GUI/NinePatch.cpp
	${SRCROOT}/GUI/Scrollable.cpp
	${SRCROOT}/GUI/ScrollBar.cpp
	${SRCROOT}/GUI/TextOverlay.cpp
	${SRCROOT}/GUI/VerbList.cpp
	${SRCROOT}/GUI/Icon/IconContinue.cpp
	${SRCROOT}/GUI/Icon/IconGlow.cpp
	${SRCROOT}/GUI/Icon/IconTouch.cpp
	${SRCROOT}/GUI/TextLog/TextLogDialogueOptionItem.cpp
	${SRCROOT}/GUI/TextLog/TextLogDialogueTextItem.cpp
	${SRCROOT}/GUI/TextLog/TextLogGenericItem.cpp
	${SRCROOT}/GUI/TextLog/TextLogItem.cpp
	${SRCROOT}/GUI/TextLog/TextLogRenderer.cpp
	${SRCROOT}/States/State.cpp
	${SRCROOT}/States/StateEditor.cpp
	${SRCROOT}/States/StateIntro.cpp
	${SRCROOT}/States/StateMain.cpp
	${SRCROOT}/States/StateProfileManager.cpp
	${SRCROOT}/States/StateSettings.cpp
	${SRCROOT}/States/StateStack.cpp
	${SRCROOT}/States/StateTextLog.cpp
	${SRCROOT}/States/StateTextSettings.cpp
	${SRCROOT}/States/StateTitleScreen.cpp

	${SRCROOT}/../renderers/SFML/AssetLoaderSFML.cpp
	${SRCROOT}/../renderers/SFML/EngineSFML.cpp
	${SRCROOT}/../renderers/SFML/NotificationSFML.cpp
	${SRCROOT}/../renderers/SFML/UtilsSFML.cpp
)

add_library(NovelTea-SFML ${SOURCE_FILES})
target_link_libraries(NovelTea-SFML NovelTea sfml-graphics GLESv2)
