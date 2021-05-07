#include "TestsWidget.hpp"
#include "ui_TestsWidget.h"
#include "MainWindow.hpp"
#include "Wizard/WizardPageActionSelect.hpp"
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Verb.hpp>
#include <QToolButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>

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
	ui->tabWidget->setEnabled(false);

	m_callback = [this](const json &j){ return processCallbackData(j); };

	// Attach the menu to the Add toolbutton
	ui->actionAddStep->setMenu(m_menuAdd);
	auto buttonAdd = static_cast<QToolButton*>(ui->toolBarSteps->widgetForAction(ui->actionAddStep));
	buttonAdd->setPopupMode(QToolButton::InstantPopup);

	connect(ui->listWidgetSteps->model(), &QAbstractItemModel::rowsMoved, this, &TestsWidget::on_rowsMoved);
	connect(&MainWindow::instance(), &MainWindow::renamed, [this](){
		m_json = ProjData[NovelTea::ID::tests];
		loadTest(m_selectedTestId);
	});
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
	ui->actionSelect->disconnect(this);
	ui->listInventory->model()->disconnect(this);
	ui->scriptEditInit->disconnect(this);
	ui->scriptEditCheck->disconnect(this);

	ui->listWidgetSteps->clear();
	ui->listInventory->clear();
	ui->tabWidget->setEnabled(!testId.empty());
	ui->tabWidget->setCurrentIndex(0);
	if (testId.empty())
		return;

	auto &j = m_json[testId];
	ui->actionSelect->setValue(j[NovelTea::ID::entrypointEntity]);
	ui->scriptEditInit->setPlainText(QString::fromStdString(j[NovelTea::ID::testScriptInit].ToString()));
	ui->scriptEditCheck->setPlainText(QString::fromStdString(j[NovelTea::ID::testScriptCheck].ToString()));
	for (auto &jstep : j[NovelTea::ID::testSteps].ArrayRange())
		addStepToList(jstep, true);

	auto &jobjects = j[NovelTea::ID::startingInventory];
	for (auto &jobject : jobjects.ArrayRange())
		ui->listInventory->addItem(QString::fromStdString(jobject.ToString()));

	MODIFIER(ui->actionSelect, &ActionSelectWidget::valueChanged);
	MODIFIER(ui->scriptEditInit, &ScriptEdit::textChanged);
	MODIFIER(ui->scriptEditCheck, &ScriptEdit::textChanged);
	MODIFIER(ui->listInventory->model(), &QAbstractItemModel::dataChanged);
	MODIFIER(ui->listInventory->model(), &QAbstractItemModel::rowsInserted);
	MODIFIER(ui->listInventory->model(), &QAbstractItemModel::rowsRemoved);
}

void TestsWidget::addStep(const json &jstep, bool append)
{
	auto &jtestSteps = m_json[m_selectedTestId][NovelTea::ID::testSteps];
	setModified();

	if (append)
	{
		jtestSteps.append(jstep);
		addStepToList(jstep, append);
		return;
	}
	// else insert
	auto row = ui->listWidgetSteps->currentRow();
	if (row < 0)
		row = 0;
	jtestSteps.insert(row, jstep);
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
		auto verb = GSave->get<NovelTea::Verb>(jstep["verb"].ToString());
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
		if (jdata.hasKey("index")) {
			m_errorStepIndex = jdata["index"].ToInt();
			auto item = ui->listWidgetSteps->item(m_errorStepIndex);
			item->setSelected(true);
			item->setBackground(QBrush(Qt::red));
		} else {
			auto msg = QString::fromStdString(jdata["error"].ToString());
			if (msg.isEmpty())
				msg = "Script check returned false.";
			else {
				ui->tabWidget->setCurrentWidget(ui->tabScripts);
				msg = "Script error:\n\n" + msg;
			}
			QMessageBox::critical(this, "Test Failed", msg);
		}
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
	saveSettings();
	resetListStyle();
	ui->preview->reset();
	m_errorStepIndex = -1;
	auto &jtest = m_json[m_selectedTestId];
	auto j = json({
		"event", "test",
		"type", "playback",
		"test", jtest,
		"record", startRecording,
		"callback", std::to_string(&m_callback),
	});
	ui->preview->processData(j);
}

void TestsWidget::saveSettings() const
{
	if (m_selectedTestId.empty() || !m_json.hasKey(m_selectedTestId))
		return;

	auto &jtest = m_json[m_selectedTestId];
	auto jobjects = sj::Array();
	for (int i = 0; i < ui->listInventory->count(); ++i) {
		auto item = ui->listInventory->item(i);
		jobjects.append(item->text().toStdString());
	}

	jtest[NovelTea::ID::startingInventory] = jobjects;
	jtest[NovelTea::ID::entrypointEntity] = ui->actionSelect->getValue();
	jtest[NovelTea::ID::testScriptInit] = ui->scriptEditInit->toPlainText().toStdString();
	jtest[NovelTea::ID::testScriptCheck] = ui->scriptEditCheck->toPlainText().toStdString();
}

void TestsWidget::saveData() const
{
	saveSettings();
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
		m_json[name.toStdString()] = sj::JSON({
			NovelTea::ID::entrypointEntity,  sj::Array(-1, ""),
			NovelTea::ID::startingInventory, sj::Array(),
			NovelTea::ID::testSteps,         sj::Array(),
			NovelTea::ID::testScriptInit,    "",
			NovelTea::ID::testScriptCheck,   ""
		});
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
	m_json[m_selectedTestId][NovelTea::ID::testSteps].erase(row);
	delete ui->listWidgetSteps->currentItem();
}

void TestsWidget::on_actionAddStepAction_triggered()
{
	auto verbId = WizardPageActionSelect::getItemId("Verbs", "Select verb for action:");
	if (verbId.empty())
		return;
	auto jobjectIds = sj::Array();
	auto verb = GSave->get<NovelTea::Verb>(verbId);
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
	saveSettings();

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
	auto &jtestSteps = m_json[m_selectedTestId][NovelTea::ID::testSteps];
	auto &jsource = jtestSteps[sourceStart];
	jtestSteps.insert(destinationRow, jsource);
	jtestSteps.erase((sourceStart < destinationRow) ? sourceStart : sourceStart+1);
}

void TestsWidget::on_actionAddObject_triggered()
{
	QWizard wizard;
	auto page = new WizardPageActionSelect;

	page->setFilterRegExp("Objects");
	page->allowCustomScript(false);

	wizard.addPage(page);

	if (wizard.exec() == QDialog::Accepted)
	{
		auto jval = page->getValue();
		auto idName = QString::fromStdString(jval[NovelTea::ID::selectEntityId].ToString());
		auto type = static_cast<NovelTea::EntityType>(jval[NovelTea::ID::selectEntityType].ToInt());
		if (type == NovelTea::EntityType::Object)
		{
			// Check if object already exists
			for (int i = 0; i < ui->listInventory->count(); ++i)
			{
				auto item = ui->listInventory->item(i);
				if (item->text() == idName)
					return;
			}

			auto item = new QListWidgetItem(idName);
			ui->listInventory->addItem(item);
		}
	}
}

void TestsWidget::on_actionRemoveObject_triggered()
{
	delete ui->listInventory->currentItem();
}

void TestsWidget::on_listInventory_currentRowChanged(int currentRow)
{
	ui->actionRemoveObject->setEnabled(currentRow >= 0);
}
