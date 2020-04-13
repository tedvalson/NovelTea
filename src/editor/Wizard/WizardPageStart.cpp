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

	newSelectionGroup->addButton(ui->radioNewProject, cNewProject);
	newSelectionGroup->addButton(ui->radioNewCutscene, cNewCutscene);
	newSelectionGroup->addButton(ui->radioNewScript, cNewScript);

	if (!Proj.isLoaded())
	{
		ui->radioNewCutscene->setEnabled(false);
		ui->radioNewScript->setEnabled(false);
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
	auto selection = newSelectionGroup->checkedId();
	if (selection == cNewCutscene)
		return Wizard::Cutscene;
	if (selection == cNewScript)
		return Wizard::Script;
	return Wizard::Project;
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
