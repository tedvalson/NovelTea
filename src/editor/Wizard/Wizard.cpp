#include "Wizard.hpp"
#include "WizardPageStart.hpp"
#include "WizardPageProject.hpp"
#include "WizardPageAction.hpp"
#include "WizardPageCutscene.hpp"
#include "WizardPageDialogue.hpp"
#include "WizardPageMap.hpp"
#include "WizardPageObject.hpp"
#include "WizardPageRoom.hpp"
#include "WizardPageScript.hpp"
#include "WizardPageVerb.hpp"
#include <iostream>


Wizard::Wizard(QWidget *parent) :
	QWizard(parent)
{
	setPage(Page::Start, new WizardPageStart(this));
	setPage(Page::Project, new WizardPageProject(this));
	setPage(Page::Action, new WizardPageAction(this));
	setPage(Page::Cutscene, new WizardPageCutscene(this));
	setPage(Page::Dialogue, new WizardPageDialogue(this));
	setPage(Page::Map, new WizardPageMap(this));
	setPage(Page::Object, new WizardPageObject(this));
	setPage(Page::Room, new WizardPageRoom(this));
	setPage(Page::Script, new WizardPageScript(this));
	setPage(Page::Verb, new WizardPageVerb(this));

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
