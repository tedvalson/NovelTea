#include "WizardPageMap.hpp"
#include "ui_WizardPageMap.h"
#include "Widgets/MainWindow.hpp"
#include "Widgets/MapWidget.hpp"

WizardPageMap::WizardPageMap(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::WizardPageMap)
{
	ui->setupUi(this);
	registerField("mapName*", ui->lineEditName);
}

WizardPageMap::~WizardPageMap()
{
	delete ui;
}

int WizardPageMap::nextId() const
{
	return -1;
}

bool WizardPageMap::validatePage()
{
	// TODO: check for name collision
	auto mainWindow = qobject_cast<MainWindow*>(wizard()->parent());
	if (mainWindow)
	{
		auto name = field("mapName").toString().toStdString();
		auto w = new MapWidget(name);
		mainWindow->addEditorTab(w);
	}
	return true;
}
