#include "WizardPageAction.hpp"
#include "ui_WizardPageAction.h"
#include "Widgets/MainWindow.hpp"
#include "Widgets/ActionWidget.hpp"

WizardPageAction::WizardPageAction(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::WizardPageAction)
{
	ui->setupUi(this);
	registerField("actionName*", ui->lineEditName);
}

WizardPageAction::~WizardPageAction()
{
	delete ui;
}

int WizardPageAction::nextId() const
{
	return -1;
}

bool WizardPageAction::validatePage()
{
	auto name = field("actionName").toString();
	auto mainWindow = qobject_cast<MainWindow*>(wizard()->parent());
	if (mainWindow && mainWindow->validateEntityName(name, EditorTabWidget::Action))
	{
		auto w = new ActionWidget(name.toStdString());
		mainWindow->addEditorTab(w);
		return true;
	}
	return false;
}
