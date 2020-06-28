#include "WizardPageScript.hpp"
#include "ui_WizardPageScript.h"
#include "Widgets/MainWindow.hpp"
#include "Widgets/ScriptWidget.hpp"

WizardPageScript::WizardPageScript(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::WizardPageScript)
{
	ui->setupUi(this);
	registerField("scriptName*", ui->lineEditName);
}

WizardPageScript::~WizardPageScript()
{
	delete ui;
}

int WizardPageScript::nextId() const
{
	return -1;
}

bool WizardPageScript::validatePage()
{
	// TODO: check for name collision
	auto mainWindow = qobject_cast<MainWindow*>(wizard()->parent());
	if (mainWindow)
	{
		auto name = field("scriptName").toString().toStdString();
		auto w = new ScriptWidget(name);
		mainWindow->addEditorTab(w);
	}
	return true;
}
