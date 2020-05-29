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

	newSelectionGroup->addButton(ui->radioNewProject, Wizard::Page::Project);
	newSelectionGroup->addButton(ui->radioNewCutscene, Wizard::Page::Cutscene);
	newSelectionGroup->addButton(ui->radioNewRoom, Wizard::Page::Room);
	newSelectionGroup->addButton(ui->radioNewObject, Wizard::Page::Object);
	newSelectionGroup->addButton(ui->radioNewScript, Wizard::Page::Script);

	if (!Proj.isLoaded())
	{
		ui->radioNewCutscene->setEnabled(false);
		ui->radioNewRoom->setEnabled(false);
		ui->radioNewObject->setEnabled(false);
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
