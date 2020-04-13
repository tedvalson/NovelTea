find_package(Qt5Widgets REQUIRED)

set(SRCROOT ${PROJECT_SOURCE_DIR}/src/editor)
set(UIROOT ${PROJECT_SOURCE_DIR}/res/forms)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(UIFILES
	${UIROOT}/CutsceneWidget.ui
	${UIROOT}/RichTextEditor.ui
	${UIROOT}/MainWindow.ui
	${UIROOT}/ProjectSettingsWidget.ui
	${UIROOT}/RoomWidget.ui
	${UIROOT}/WizardPageStart.ui
	${UIROOT}/WizardPageProject.ui
	${UIROOT}/WizardPageCutscene.ui
)

set(SRC
	${SRCROOT}/main.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qteditorfactory.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qtpropertybrowser.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qtpropertybrowserutils.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qtpropertymanager.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qttreepropertybrowser.cpp
	${SRCROOT}/Widgets/QtPropertyBrowser/qtvariantproperty.cpp
	${SRCROOT}/Widgets/EditorTabWidget.cpp
	${SRCROOT}/Widgets/CutsceneWidget.cpp
	${SRCROOT}/Widgets/NovelTeaWidget.cpp
	${SRCROOT}/Widgets/ProjectSettingsWidget.cpp
	${SRCROOT}/Widgets/RichTextEditor.cpp
	${SRCROOT}/Widgets/RoomWidget.cpp
	${SRCROOT}/Widgets/SFMLWidget.cpp
	${SRCROOT}/Widgets/MainWindow.cpp
	${SRCROOT}/Wizard/Wizard.cpp
	${SRCROOT}/Wizard/WizardPageStart.cpp
	${SRCROOT}/Wizard/WizardPageProject.cpp
	${SRCROOT}/Wizard/WizardPageCutscene.cpp
	${SRCROOT}/TreeItem.cpp
	${SRCROOT}/TreeModel.cpp
	${SRCROOT}/Utils.cpp
)

qt5_wrap_ui(UIOUT ${UIFILES})
qt5_add_resources(RESOURCES ${PROJECT_SOURCE_DIR}/res/NovelTeaEditor.qrc)

add_executable(NovelTeaEditor ${SRC} ${UIOUT} ${RESOURCES})

target_link_libraries(NovelTeaEditor Qt5::Core Qt5::Gui Qt5::Widgets NovelTea sfml-graphics sfml-window sfml-system sfml-audio)
