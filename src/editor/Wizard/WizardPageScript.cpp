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
	auto name = field("scriptName").toString();
	auto mainWindow = qobject_cast<MainWindow*>(wizard()->parent());
	if (mainWindow && mainWindow->validateEntityName(name, EditorTabWidget::Script))
	{
		auto w = new ScriptWidget(name.toStdString());
		mainWindow->addEditorTab(w);
	}
	return true;
}
