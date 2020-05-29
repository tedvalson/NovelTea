#include "Wizard.hpp"
#include "WizardPageStart.hpp"
#include "WizardPageProject.hpp"
#include "WizardPageCutscene.hpp"
#include "WizardPageRoom.hpp"
#include "WizardPageObject.hpp"
#include <iostream>


Wizard::Wizard(QWidget *parent) :
	QWizard(parent)
{
	setPage(Page::Start, new WizardPageStart(this));
	setPage(Page::Project, new WizardPageProject(this));
	setPage(Page::Cutscene, new WizardPageCutscene(this));
	setPage(Page::Room, new WizardPageRoom(this));
	setPage(Page::Object, new WizardPageObject(this));

	setStartId(Page::Start);
	setWindowTitle("Create New");
}

Wizard::~Wizard()
{

}

void Wizard::accept()
{
	QDialog::accept();
}
