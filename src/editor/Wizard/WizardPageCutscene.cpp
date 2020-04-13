#include "WizardPageCutscene.hpp"
#include "ui_WizardPageCutscene.h"
#include "Widgets/MainWindow.hpp"
#include "Widgets/CutsceneWidget.hpp"

WizardPageCutscene::WizardPageCutscene(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::WizardPageCutscene)
{
	ui->setupUi(this);
	registerField("cutsceneName*", ui->lineEditName);
}

WizardPageCutscene::~WizardPageCutscene()
{
	delete ui;
}

int WizardPageCutscene::nextId() const
{
	return -1;
}

bool WizardPageCutscene::validatePage()
{
	auto mainWindow = qobject_cast<MainWindow*>(wizard()->parent());
	if (mainWindow)
	{
		auto name = field("cutsceneName").toString().toStdString();
		auto w = new CutsceneWidget(name);
		mainWindow->addEditorTab(w);
	}
	return true;
}
