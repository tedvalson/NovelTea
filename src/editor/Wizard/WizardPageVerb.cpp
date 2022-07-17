#include "WizardPageVerb.hpp"
#include "ui_WizardPageVerb.h"
#include "Widgets/MainWindow.hpp"
#include "Widgets/VerbWidget.hpp"

WizardPageVerb::WizardPageVerb(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::WizardPageVerb)
{
	ui->setupUi(this);
	registerField("verbName*", ui->lineEditName);
}

WizardPageVerb::~WizardPageVerb()
{
	delete ui;
}

int WizardPageVerb::nextId() const
{
	return -1;
}

bool WizardPageVerb::validatePage()
{
	auto name = field("verbName").toString();
	auto mainWindow = qobject_cast<MainWindow*>(wizard()->parent());
	if (mainWindow && mainWindow->validateEntityName(name, EditorTabWidget::Verb))
	{
		auto w = new VerbWidget(name.toStdString());
		mainWindow->addEditorTab(w);
	}
	return true;
}
