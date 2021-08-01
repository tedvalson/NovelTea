find_package(Qt5Widgets REQUIRED)
find_package(hunspell REQUIRED)

include_directories(${HUNSPELL_INCLUDE_DIR})

set(SRCROOT ${PROJECT_SOURCE_DIR}/src/editor)
set(UIROOT ${PROJECT_SOURCE_DIR}/res/forms)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(UIFILES
	${UIROOT}/ActionBuildWidget.ui
	${UIROOT}/ActionSelectWidget.ui
	${UIROOT}/ActionWidget.ui
	${UIROOT}/CutsceneWidget.ui
	${UIROOT}/DialogueWidget.ui
	${UIROOT}/MainWindow.ui
	${UIROOT}/ObjectWidget.ui
	${UIROOT}/ProjectSettingsWidget.ui
	${UIROOT}/PropertyEditor.ui
	${UIROOT}/RoomWidget.ui
	${UIROOT}/RichTextEditor.ui
	${UIROOT}/SearchWidget.ui
	${UIROOT}/ScriptWidget.ui
	${UIROOT}/SpellCheckWidget.ui
	${UIROOT}/TestsWidget.ui
	${UIROOT}/VerbWidget.ui
	${UIROOT}/WizardPageAction.ui
	${UIROOT}/WizardPageActionSelect.ui
	${UIROOT}/WizardPageCutscene.ui
	${UIROOT}/WizardPageDialogue.ui
	${UIROOT}/WizardPageObject.ui
	${UIROOT}/WizardPageProject.ui
	${UIROOT}/WizardPageRoom.ui
	${UIROOT}/WizardPageScript.ui
	${UIROOT}/WizardPageStart.ui
	${UIROOT}/WizardPageVerb.ui
)

set(SRC
	${SRCROOT}/main.cpp
	${SRCROOT}/DialogueTreeItem.cpp
	${SRCROOT}/DialogueTreeModel.cpp
	${SRCROOT}/EditorUtils.cpp
	${SRCROOT}/EntitySortFilterProxyModel.cpp
	${SRCROOT}/EntityTreeView.cpp
	${SRCROOT}/SyntaxHighlighter.cpp
	${SRCROOT}/TreeItem.cpp
	${SRCROOT}/TreeModel.cpp
	${SRCROOT}/Widgets/ActionBuildWidget.cpp
	${SRCROOT}/Widgets/ActionSelectWidget.cpp
	${SRCROOT}/Widgets/ActionWidget.cpp
	${SRCROOT}/Widgets/CutsceneWidget.cpp
	${SRCROOT}/Widgets/DialogueWidget.cpp
	${SRCROOT}/Widgets/EditorTabWidget.cpp
	${SRCROOT}/Widgets/MainWindow.cpp
	${SRCROOT}/Widgets/NovelTeaWidget.cpp
	${SRCROOT}/Widgets/ObjectWidget.cpp
	${SRCROOT}/Widgets/ProjectSettingsWidget.cpp
	${SRCROOT}/Widgets/PropertyEditor.cpp
	${SRCROOT}/Widgets/RichTextEditor.cpp
	${SRCROOT}/Widgets/RoomWidget.cpp
	${SRCROOT}/Widgets/SearchTreeWidget.cpp
	${SRCROOT}/Widgets/SearchWidget.cpp
	${SRCROOT}/Widgets/ScriptEdit.cpp
	${SRCROOT}/Widgets/ScriptWidget.cpp
	${SRCROOT}/Widgets/SpellCheckWidget.cpp
	${SRCROOT}/Widgets/SFMLWidget.cpp
	${SRCROOT}/Widgets/TestsWidget.cpp
	${SRCROOT}/Widgets/VerbWidget.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qteditorfactory.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qtpropertybrowser.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qtpropertybrowserutils.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qtpropertymanager.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qttreepropertybrowser.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qtvariantproperty.cpp
	${SRCROOT}/Wizard/Wizard.cpp
	${SRCROOT}/Wizard/WizardPageAction.cpp
	${SRCROOT}/Wizard/WizardPageActionSelect.cpp
	${SRCROOT}/Wizard/WizardPageCutscene.cpp
	${SRCROOT}/Wizard/WizardPageDialogue.cpp
	${SRCROOT}/Wizard/WizardPageObject.cpp
	${SRCROOT}/Wizard/WizardPageProject.cpp
	${SRCROOT}/Wizard/WizardPageRoom.cpp
	${SRCROOT}/Wizard/WizardPageScript.cpp
	${SRCROOT}/Wizard/WizardPageStart.cpp
	${SRCROOT}/Wizard/WizardPageVerb.cpp
)

qt5_wrap_ui(UIOUT ${UIFILES})
qt5_add_resources(RESOURCES ${PROJECT_SOURCE_DIR}/res/NovelTeaEditor.qrc)

add_executable(NovelTeaEditor ${SRC} ${UIOUT} ${RESOURCES})

target_link_libraries(NovelTeaEditor Qt5::Core Qt5::Gui Qt5::Widgets NovelTea ${HUNSPELL_LIBRARY})

# Install executable
install(TARGETS NovelTeaEditor
		RUNTIME DESTINATION .)
