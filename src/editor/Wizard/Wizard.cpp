#include "Wizard.hpp"
#include "WizardPageStart.hpp"
#include "WizardPageProject.hpp"
#include "WizardPageCutscene.hpp"
#include <iostream>


Wizard::Wizard(QWidget *parent) :
	QWizard(parent)
{
	setPage(cNewStart, new WizardPageStart(this));
	setPage(cNewProject, new WizardPageProject(this));
	setPage(cNewCutscene, new WizardPageCutscene(this));

	setStartId(cNewStart);
	setWindowTitle("Create New");
}

Wizard::~Wizard()
{

}

void Wizard::accept()
{
	QDialog::accept();
}
