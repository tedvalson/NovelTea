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
	auto name = field("objectName").toString();
	auto mainWindow = qobject_cast<MainWindow*>(wizard()->parent());
	if (mainWindow && mainWindow->validateEntityName(name, EditorTabWidget::Object))
	{
		auto w = new ObjectWidget(name.toStdString());
		mainWindow->addEditorTab(w);
	}
	return true;
}
