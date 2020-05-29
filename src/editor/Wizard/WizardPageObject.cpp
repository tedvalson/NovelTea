#include "WizardPageObject.hpp"
#include "ui_WizardPageObject.h"
#include "Widgets/MainWindow.hpp"
#include "Widgets/ObjectWidget.hpp"

WizardPageObject::WizardPageObject(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::WizardPageObject)
{
	ui->setupUi(this);
	registerField("objectName*", ui->lineEditName);
}

WizardPageObject::~WizardPageObject()
{
	delete ui;
}

int WizardPageObject::nextId() const
{
	return -1;
}

bool WizardPageObject::validatePage()
{
	// TODO: check for cutscene name collision
	auto mainWindow = qobject_cast<MainWindow*>(wizard()->parent());
	if (mainWindow)
	{
		auto name = field("objectName").toString().toStdString();
		auto w = new ObjectWidget(name);
		mainWindow->addEditorTab(w);
	}
	return true;
}
