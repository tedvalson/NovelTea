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
	auto name = field("dialogueName").toString();
	auto mainWindow = qobject_cast<MainWindow*>(wizard()->parent());
	if (mainWindow && mainWindow->validateEntityName(name, EditorTabWidget::Dialogue))
	{
		auto w = new DialogueWidget(name.toStdString());
		mainWindow->addEditorTab(w);
	}
	return true;
}
