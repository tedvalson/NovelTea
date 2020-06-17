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
	// TODO: check for cutscene name collision
	auto mainWindow = qobject_cast<MainWindow*>(wizard()->parent());
	if (mainWindow)
	{
		auto name = field("verbName").toString().toStdString();
		auto w = new VerbWidget(name);
		mainWindow->addEditorTab(w);
	}
	return true;
}
