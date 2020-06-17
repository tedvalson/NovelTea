find_package(Qt5Widgets REQUIRED)

set(SRCROOT ${PROJECT_SOURCE_DIR}/src/editor)
set(UIROOT ${PROJECT_SOURCE_DIR}/res/forms)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(UIFILES
	${UIROOT}/ActionSelectWidget.ui
	${UIROOT}/WizardPageActionSelect.ui
	${UIROOT}/CutsceneWidget.ui
	${UIROOT}/RoomWidget.ui
	${UIROOT}/ObjectWidget.ui
	${UIROOT}/VerbWidget.ui
	${UIROOT}/RichTextEditor.ui
	${UIROOT}/MainWindow.ui
	${UIROOT}/ProjectSettingsWidget.ui
	${UIROOT}/WizardPageStart.ui
	${UIROOT}/WizardPageProject.ui
	${UIROOT}/WizardPageCutscene.ui
	${UIROOT}/WizardPageRoom.ui
	${UIROOT}/WizardPageVerb.ui
	${UIROOT}/WizardPageObject.ui
)

set(SRC
	${SRCROOT}/main.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qteditorfactory.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qtpropertybrowser.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qtpropertybrowserutils.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qtpropertymanager.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qttreepropertybrowser.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qtvariantproperty.cpp
	${SRCROOT}/Widgets/ActionSelectWidget.cpp
	${SRCROOT}/Widgets/EditorTabWidget.cpp
	${SRCROOT}/Widgets/CutsceneWidget.cpp
	${SRCROOT}/Widgets/RoomWidget.cpp
	${SRCROOT}/Widgets/ObjectWidget.cpp
	${SRCROOT}/Widgets/VerbWidget.cpp
	${SRCROOT}/Widgets/NovelTeaWidget.cpp
	${SRCROOT}/Widgets/ProjectSettingsWidget.cpp
	${SRCROOT}/Widgets/RichTextEditor.cpp
	${SRCROOT}/Widgets/SFMLWidget.cpp
	${SRCROOT}/Widgets/ScriptEdit.cpp
	${SRCROOT}/Widgets/MainWindow.cpp
	${SRCROOT}/Wizard/Wizard.cpp
	${SRCROOT}/Wizard/WizardPageActionSelect.cpp
	${SRCROOT}/Wizard/WizardPageStart.cpp
	${SRCROOT}/Wizard/WizardPageProject.cpp
	${SRCROOT}/Wizard/WizardPageCutscene.cpp
	${SRCROOT}/Wizard/WizardPageRoom.cpp
	${SRCROOT}/Wizard/WizardPageObject.cpp
	${SRCROOT}/Wizard/WizardPageVerb.cpp
	${SRCROOT}/TreeItem.cpp
	${SRCROOT}/TreeModel.cpp
	${SRCROOT}/SyntaxHighlighter.cpp
	${SRCROOT}/EntityTreeView.cpp
	${SRCROOT}/EntitySortFilterProxyModel.cpp
	${SRCROOT}/EditorUtils.cpp
)

qt5_wrap_ui(UIOUT ${UIFILES})
qt5_add_resources(RESOURCES ${PROJECT_SOURCE_DIR}/res/NovelTeaEditor.qrc)

add_executable(NovelTeaEditor ${SRC} ${UIOUT} ${RESOURCES})

target_link_libraries(NovelTeaEditor Qt5::Core Qt5::Gui Qt5::Widgets NovelTea)

# Install executable
install(TARGETS NovelTeaEditor
		RUNTIME DESTINATION .)
