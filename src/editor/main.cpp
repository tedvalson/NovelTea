#include "Widgets/MainWindow.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(NovelTeaEditor);

	QCoreApplication::setOrganizationName("NovelTea");
	QCoreApplication::setOrganizationDomain("noveltea.com");
	QCoreApplication::setApplicationName("NovelTea Editor");

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
