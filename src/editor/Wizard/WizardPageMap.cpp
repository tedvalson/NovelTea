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
	auto name = field("mapName").toString();
	auto mainWindow = qobject_cast<MainWindow*>(wizard()->parent());
	if (mainWindow && mainWindow->validateEntityName(name, EditorTabWidget::Map))
	{
		auto w = new MapWidget(name.toStdString());
		mainWindow->addEditorTab(w);
	}
	return true;
}
