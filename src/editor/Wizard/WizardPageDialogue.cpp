#include "WizardPageDialogue.hpp"
#include "ui_WizardPageDialogue.h"
#include "Widgets/MainWindow.hpp"
#include "Widgets/DialogueWidget.hpp"

WizardPageDialogue::WizardPageDialogue(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::WizardPageDialogue)
{
	ui->setupUi(this);
	registerField("dialogueName*", ui->lineEditName);
}

WizardPageDialogue::~WizardPageDialogue()
{
	delete ui;
}

int WizardPageDialogue::nextId() const
{
	return -1;
}

bool WizardPageDialogue::validatePage()
{
	// TODO: check for cutscene name collision
	auto mainWindow = qobject_cast<MainWindow*>(wizard()->parent());
	if (mainWindow)
	{
		auto name = field("dialogueName").toString().toStdString();
		auto w = new DialogueWidget(name);
		mainWindow->addEditorTab(w);
	}
	return true;
}
