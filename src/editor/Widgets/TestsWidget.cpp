#include "TestsWidget.hpp"
#include "ui_TestsWidget.h"
#include "Wizard/WizardPageActionSelect.hpp"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Verb.hpp>
#include <QToolButton>
#include <QInputDialog>

TestsWidget::TestsWidget(QWidget *parent)
	: EditorTabWidget(parent)
	, ui(new Ui::TestsWidget)
	, m_menuAdd(new QMenu)
	, m_errorStepIndex(-1)
{
	ui->setupUi(this);
	load();

	m_menuAdd->addAction(ui->actionAddStepAction);
	m_menuAdd->addAction(ui->actionAddStepDialogueOption);
	m_menuAdd->addAction(ui->actionAddStepWait);
	ui->toolBarSteps->setEnabled(false);

	m_callback = [this](const json &j){ return processCallbackData(j); };

	// Attach the menu to the Add toolbutton
	ui->actionAddStep->setMenu(m_menuAdd);
	auto buttonAdd = static_cast<QToolButton*>(ui->toolBarSteps->widgetForAction(ui->actionAddStep));
	buttonAdd->setPopupMode(QToolButton::InstantPopup);

	connect(ui->listWidgetSteps->model(), &QAbstractItemModel::rowsMoved, this, &TestsWidget::on_rowsMoved);
}

TestsWidget::~TestsWidget()
{
	delete m_menuAdd;
	delete ui;
}

QString TestsWidget::tabText() const
{
	return "Tests";
}

EditorTabWidget::Type TestsWidget::getType() const
{
	return EditorTabWidget::Tests;
}

void TestsWidget::loadTest(const std::string &testId)
{
	ui->listWidgetSteps->clear();
	ui->toolBarSteps->setEnabled(!testId.empty());
	if (testId.empty())
		return;

	auto &j = m_json[testId];
	for (auto &jstep : m_json[testId].ArrayRange())
	{
		addStepToList(jstep, true);
//		ui->listWidgetSteps->addItem(QString::fromStdString(jstep.ToString()));
	}
}

void TestsWidget::addStep(const json &jstep, bool append)
{
	auto &jtest = m_json[m_selectedTestId];
//	auto type = jstep["type"].ToString();
	setModified();

	if (append)
	{
//		ui->listWidgetSteps->addItem(QString::fromStdString(text));
		jtest.append(jstep);
		addStepToList(jstep, append);
		return;
	}
	// else insert
	auto row = ui->listWidgetSteps->currentRow();
	if (row < 0)
		row = 0;
//	ui->listWidgetSteps->insertItem(row, QString::fromStdString(text));
	jtest.insert(row, jstep);
	addStepToList(jstep, append);
}

void TestsWidget::addStepToList(const json &jstep, bool append)
{
	auto type = jstep["type"].ToString();
	std::string text;
	if (type == "action")
	{
		std::vector<std::string> objectIds;
		for (auto jobjectId : jstep["objects"].ArrayRange())
			objectIds.push_back(jobjectId.ToString());
		auto verb = GSave.get<NovelTea::Verb>(jstep["verb"].ToString());
		text = "Action: " + verb->getActionText(objectIds);
	}
	else if (type == "wait")
	{
		int duration = jstep["duration"].ToInt();
		text = "Wait: " + std::to_string(duration) + "ms";
	}
	else if (type == "move")
	{
		auto direction = jstep["direction"].ToInt();
		std::string dir = std::to_string(direction);
		text = "Move: " + dir;
	}
	else if (type == "dialogue")
	{
		text = "Dialogue choice: " + std::to_string(jstep["index"].ToInt());
	}

	if (append)
	{
		ui->listWidgetSteps->addItem(QString::fromStdString(text));
		return;
	}
	// else insert
	auto row = ui->listWidgetSteps->currentRow();
	if (row < 0)
		row = 0;
	ui->listWidgetSteps->insertItem(row, QString::fromStdString(text));
}

void TestsWidget::resetListStyle()
{
	for (int i = 0; i < ui->listWidgetSteps->count(); ++i)
	{
		auto item = ui->listWidgetSteps->item(i);
		item->setBackground(QBrush());
	}
}

bool TestsWidget::processCallbackData(const json &jdata)
{
	if (jdata.hasKey("success") && !jdata["success"].ToBool())
	{
		m_errorStepIndex = jdata["index"].ToInt();
		auto item = ui->listWidgetSteps->item(m_errorStepIndex);
		item->setSelected(true);
		item->setBackground(QBrush(Qt::red));
		return true;
	}

	if (m_errorStepIndex != -1)
	{
		ui->listWidgetSteps->setCurrentRow(m_errorStepIndex);
		addStep(jdata);
	} else
		addStep(jdata, true);

//	std::cout << "process step: " << jstep["type"].ToString() << std::endl;
	return true;
}

void TestsWidget::processSteps(bool startRecording)
{
	resetListStyle();
	ui->preview->reset();
	m_errorStepIndex = -1;
	auto &jsteps = m_json[m_selectedTestId];
	auto j = json({
		"event", "test",
		"type", "playback",
		"steps", jsteps,
		"record", startRecording,
		"callback", std::to_string(&m_callback),
	});
	ui->preview->processData(j);
}

void TestsWidget::saveData() const
{
	ProjData[NovelTea::ID::tests] = m_json;
}

void TestsWidget::loadData()
{
	m_json = ProjData[NovelTea::ID::tests];
	ui->listWidgetTests->clear();
	ui->listWidgetSteps->clear();

	for (auto &item : m_json.ObjectRange())
		ui->listWidgetTests->addItem(QString::fromStdString(item.first));

//	MODIFIER(ui->listWidgetTests->model(), &QAbstractItemModel::dataChanged);
	MODIFIER(ui->listWidgetTests->model(), &QAbstractItemModel::rowsInserted);
	MODIFIER(ui->listWidgetTests->model(), &QAbstractItemModel::rowsRemoved);
}

void TestsWidget::on_actionAddTest_triggered()
{
	auto name = QInputDialog::getText(this, tr("New Test Name"),
			tr("Enter name for new test:"));
	if (!name.isEmpty() && !m_json.hasKey(name.toStdString()))
	{
		m_json[name.toStdString()] = sj::Array();
		ui->listWidgetTests->addItem(name);
	}
}

void TestsWidget::on_actionRemoveTest_triggered()
{
	m_json.erase(m_selectedTestId);
	delete ui->listWidgetTests->currentItem();
}

void TestsWidget::on_actionRemoveStep_triggered()
{
	setModified();
	auto row = ui->listWidgetSteps->currentRow();
	m_json[m_selectedTestId].erase(row);
	delete ui->listWidgetSteps->currentItem();
}

void TestsWidget::on_actionAddStepAction_triggered()
{
	auto verbId = WizardPageActionSelect::getItemId("Verbs", "Select verb for action:");
	if (verbId.empty())
		return;
	auto jobjectIds = sj::Array();
	auto verb = GSave.get<NovelTea::Verb>(verbId);
	for (int i = 0; i < verb->getObjectCount(); ++i)
	{
		auto labelMessage = "Select object " + std::to_string(i+1) + " of " + std::to_string(verb->getObjectCount()) + ":";
		auto objectId = WizardPageActionSelect::getItemId("Objects", QString::fromStdString(labelMessage));
		if (objectId.empty())
			return;
		jobjectIds.append(objectId);
	}
	json j({
		"type", "action",
		"verb", verbId,
		"objects", jobjectIds
	});
	addStep(j);
}

void TestsWidget::on_actionAddStepWait_triggered()
{
	int duration = QInputDialog::getInt(this, tr("Add Wait"), tr("Enter wait duration (ms):"));
	if (duration == 0)
		return;
	json j({
		"type", "wait",
		"duration", duration
	});
	addStep(j);
}

void TestsWidget::on_actionAddStepDialogueOption_triggered()
{
	int index = QInputDialog::getInt(this, tr("Add Dialogue Option"), tr("Enter dialogue option index:"));
	json j({
		"type", "dialogue",
		"index", index
	});
	addStep(j);
}

void TestsWidget::on_listWidgetTests_currentRowChanged(int currentRow)
{
	auto item = ui->listWidgetTests->currentItem();
	if (item)
		m_selectedTestId = item->text().toStdString();
	else
		m_selectedTestId.clear();

	loadTest(m_selectedTestId);
	ui->actionRemoveTest->setEnabled(item);
}

void TestsWidget::on_listWidgetSteps_currentRowChanged(int currentRow)
{
	ui->actionRemoveStep->setEnabled(currentRow >= 0);
}

void TestsWidget::on_actionRunSteps_triggered()
{
	processSteps(false);
}

void TestsWidget::on_actionRecordSteps_triggered()
{
	processSteps(true);
}

void TestsWidget::on_rowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
	setModified();
	auto &jtest = m_json[m_selectedTestId];
	auto &jsource = jtest[sourceStart];
	jtest.insert(destinationRow, jsource);
	jtest.erase((sourceStart < destinationRow) ? sourceStart : sourceStart+1);
}
