#include "WizardPageStart.hpp"
#include "ui_WizardPageStart.h"
#include "Wizard.hpp"
#include <NovelTea/ProjectData.hpp>
#include <QButtonGroup>

WizardPageStart::WizardPageStart(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::WizardPageStart),
	newSelectionGroup(new QButtonGroup),
	typeId(-1)
{
	ui->setupUi(this);

	newSelectionGroup->addButton(ui->radioProject, Wizard::Page::Project);
	newSelectionGroup->addButton(ui->radioAction, Wizard::Page::Action);
	newSelectionGroup->addButton(ui->radioCutscene, Wizard::Page::Cutscene);
	newSelectionGroup->addButton(ui->radioDialogue, Wizard::Page::Dialogue);
	newSelectionGroup->addButton(ui->radioRoom, Wizard::Page::Room);
	newSelectionGroup->addButton(ui->radioObject, Wizard::Page::Object);
	newSelectionGroup->addButton(ui->radioScript, Wizard::Page::Script);
	newSelectionGroup->addButton(ui->radioVerb, Wizard::Page::Verb);

	if (!Proj.isLoaded())
	{
		ui->radioAction->setEnabled(false);
		ui->radioCutscene->setEnabled(false);
		ui->radioDialogue->setEnabled(false);
		ui->radioRoom->setEnabled(false);
		ui->radioObject->setEnabled(false);
		ui->radioScript->setEnabled(false);
		ui->radioVerb->setEnabled(false);
	}

	connect(newSelectionGroup, SIGNAL(buttonClicked(int)), this, SLOT(setTypeId(int)));
	registerField("newType*", this, "typeId", SIGNAL(typeIdChanged()));
}

WizardPageStart::~WizardPageStart()
{
	delete newSelectionGroup;
	delete ui;
}

int WizardPageStart::nextId() const
{
	// Returning -1 at launch will lock the page as final?
	auto id = newSelectionGroup->checkedId();
	return (id < 0) ? 0 : id;
}

void WizardPageStart::setTypeId(int id)
{
	typeId = id;
	emit typeIdChanged();
}

int WizardPageStart::getTypeId() const
{
	return typeId;
}
