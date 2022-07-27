#include "Widgets/MainWindow.hpp"
#include "Widgets/CutsceneWidget.hpp"
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/Settings.hpp>
#include <QApplication>

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(NovelTeaEditor);

	QCoreApplication::setOrganizationName("NovelTea");
	QCoreApplication::setOrganizationDomain("noveltea.com");
	QCoreApplication::setApplicationName("NovelTea Editor");

	GSettings.setSaveEnabled(false);
	GSettings.ensureProfileExists();
	NovelTea::AssetPath::set("/home/android/dev/NovelTea/res/assets/");

	QApplication a(argc, argv);
	auto &window = MainWindow::instance();
	window.show();

	return a.exec();
}
