#include "WizardPageProject.hpp"
#include "ui_WizardPageProject.h"

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
