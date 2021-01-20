#include "WizardPageProject.hpp"
#include "ui_WizardPageProject.h"
#include "Widgets/MainWindow.hpp"

WizardPageProject::WizardPageProject(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::WizardPageProject)
{
	ui->setupUi(this);

	registerField("projectName*", ui->lineEditName);
	registerField("projectAuthor*", ui->lineEditAuthor);
}

WizardPageProject::~WizardPageProject()
{
	delete ui;
}

int WizardPageProject::nextId() const
{
	return -1;
}

bool WizardPageProject::validatePage()
{
	auto mainWindow = qobject_cast<MainWindow*>(wizard()->parent());
	if (mainWindow)
	{
		NovelTea::ProjectData project;
		project.newProject();
		project.data()[NovelTea::ID::projectName] = field("projectName").toString().toStdString();
		project.data()[NovelTea::ID::projectAuthor] = field("projectAuthor").toString().toStdString();
		auto success = mainWindow->loadProject(project);
		if (success)
			mainWindow->addEditorTab(EditorTabWidget::Settings, "");
		return success;
	}

	return false;
}
