#include "WizardPageCutscene.hpp"
#include "ui_WizardPageCutscene.h"
#include "Widgets/MainWindow.hpp"
#include "Widgets/CutsceneWidget.hpp"
#include <NovelTea/CutsceneTextSegment.hpp>

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
	// TODO: check for cutscene name collision
	auto mainWindow = qobject_cast<MainWindow*>(wizard()->parent());
	if (mainWindow)
	{
		if (ui->groupBox->isChecked())
			generateFromText();
		auto name = field("cutsceneName").toString().toStdString();
		auto w = new CutsceneWidget(name);
		mainWindow->addEditorTab(w);
	}
	return true;
}

void WizardPageCutscene::generateFromText()
{
	auto name = field("cutsceneName").toString().toStdString();
	auto cutscene = std::make_shared<NovelTea::Cutscene>();

	auto lines = ui->textEdit->toPlainText().split("\n");
	for (auto &line : lines)
	{
		auto text = std::make_shared<NovelTea::ActiveText>();
		text->setText(line.toStdString());

		auto segment = std::make_shared<NovelTea::CutsceneTextSegment>();
		segment->setActiveText(text);
		segment->setBeginWithNewLine(true);
		segment->setDuration(ui->spinBoxDuration->value());
		segment->setDelay(ui->spinBoxDelay->value());
		segment->setWaitForClick(ui->checkBoxWait->isChecked());
		segment->setCanSkip(ui->checkBoxCanSkip->isChecked());
		segment->setTransition(ui->comboTransitionEffect->currentIndex());
		cutscene->addSegment(segment);
	}

	Proj.set(cutscene, name);
}
