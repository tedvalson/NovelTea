#include "DialogueWidget.hpp"
#include "ui_DialogueWidget.h"
#include "DialogueTreeItem.hpp"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/DialogueSegment.hpp>
#include <QDebug>

using NovelTea::DialogueSegment;

DialogueWidget::DialogueWidget(const std::string &idName, QWidget *parent)
	: EditorTabWidget(parent)
	, ui(new Ui::DialogueWidget)
	, m_treeModel(new DialogueTreeModel)
	, m_selectedItem(nullptr)
	, m_selectedSegment(nullptr)
	, m_menuTreeView(new QMenu)
{
	m_idName = idName;
	ui->setupUi(this);

	ui->treeView->setModel(m_treeModel);

	m_menuTreeView->addAction(ui->actionAddObject);
	m_menuTreeView->addAction(ui->actionDelete);

	ui->radioText->setChecked(true);

	load();
	startTimer(100);
}

DialogueWidget::~DialogueWidget()
{
	delete m_menuTreeView;
	delete m_treeModel;
	delete ui;
}

QString DialogueWidget::tabText() const
{
	return QString::fromStdString(idName());
}

EditorTabWidget::Type DialogueWidget::getType() const
{
	return EditorTabWidget::Dialogue;
}

void DialogueWidget::timerEvent(QTimerEvent *)
{
	checkIndexChange();
}

void DialogueWidget::saveData() const
{
	if (m_dialogue)
	{
		m_treeModel->saveDialogue(m_dialogue);
		m_dialogue->setProperties(ui->propertyEditor->getValue());
		Proj.set<NovelTea::Dialogue>(m_dialogue, idName());
	}
}

void DialogueWidget::loadData()
{
	m_dialogue = Proj.get<NovelTea::Dialogue>(idName());

	if (!m_dialogue)
	{
		// Dialogue is new, so show it as modified
		setModified();
		m_dialogue = std::make_shared<NovelTea::Dialogue>();
	}

	m_treeModel->loadDialogue(m_dialogue);
	ui->treeView->expandToDepth(0);
	for (auto i = 1; i < m_treeModel->columnCount(); ++i)
		ui->treeView->hideColumn(i);
	ui->propertyEditor->setValue(m_dialogue->getProperties());
	fillItemSettings();

	MODIFIER(ui->propertyEditor, &PropertyEditor::valueChanged);
	MODIFIER(m_treeModel, &QAbstractItemModel::dataChanged);
	MODIFIER(m_treeModel, &QAbstractItemModel::rowsInserted);
	MODIFIER(m_treeModel, &QAbstractItemModel::rowsRemoved);
}

void DialogueWidget::on_treeView_pressed(const QModelIndex &index)
{
	checkIndexChange();
	if (QApplication::mouseButtons() != Qt::RightButton)
		return;

	auto type = m_selectedSegment ? m_selectedSegment->getType() : DialogueSegment::Root;

	ui->actionAddObject->setEnabled(type != DialogueSegment::Link);
	ui->actionDelete->setEnabled(type != DialogueSegment::Root);

	m_menuTreeView->popup(QCursor::pos());
}

void DialogueWidget::on_actionAddObject_triggered()
{
	auto index = ui->treeView->currentIndex();
	auto type = m_selectedSegment ? m_selectedSegment->getType() : DialogueSegment::Root;

	if (type == DialogueSegment::Link)
		return;

	auto newSegment = std::make_shared<DialogueSegment>();
	if (type == DialogueSegment::Root || type == DialogueSegment::Player)
	{
		newSegment->setType(DialogueSegment::NPC);
		newSegment->setText("NPC");
	} else {
		newSegment->setType(DialogueSegment::Player);
		newSegment->setText("Player");
	}

	m_treeModel->insertSegment(0, index, newSegment);
}

void DialogueWidget::on_actionDelete_triggered()
{
	auto selectedIndex = ui->treeView->currentIndex();
	m_treeModel->removeRow(selectedIndex.row(), selectedIndex.parent());
}

void DialogueWidget::on_radioText_toggled(bool checked)
{
	ui->plainTextEdit->setVisible(checked);
	ui->scriptEditText->setVisible(!checked);
}

void DialogueWidget::fillItemSettings()
{
	if (!m_selectedItem)
	{
		ui->plainTextEdit->clear();
		ui->scriptEdit->clear();
		ui->scriptEditText->clear();
		ui->scriptEditConditional->clear();
		ui->tabText->setEnabled(false);
		ui->tabConditional->setEnabled(false);
		ui->tabScript->setEnabled(false);
		return;
	}

	ui->tabText->setEnabled(true);
	ui->tabConditional->setEnabled(true);
	ui->tabScript->setEnabled(true);

	if (m_selectedSegment->getScriptedText())
	{
		ui->radioScript->setChecked(true);
		ui->plainTextEdit->clear();
		ui->scriptEditText->setPlainText(QString::fromStdString(m_selectedSegment->getText()));
	} else {
		ui->radioText->setChecked(true);
		ui->scriptEditText->clear();
		ui->plainTextEdit->setPlainText(QString::fromStdString(m_selectedSegment->getText()));
	}

	ui->checkBoxConditional->setChecked(m_selectedSegment->getConditionalEnabled());
	ui->scriptEditConditional->setPlainText(QString::fromStdString(m_selectedSegment->getConditionScript()));
	ui->checkBoxScript->setChecked(m_selectedSegment->getScriptEnabled());
	ui->scriptEdit->setPlainText(QString::fromStdString(m_selectedSegment->getScript()));
}

void DialogueWidget::checkIndexChange()
{
	auto index = ui->treeView->currentIndex();
	auto selectedItem = static_cast<DialogueTreeItem*>(index.internalPointer());
	if (!index.isValid() || !selectedItem->parent()->parent())
		selectedItem = nullptr;

	if (m_selectedItem != selectedItem)
	{
		m_selectedItem = selectedItem;
		if (m_selectedItem)
			m_selectedSegment = m_selectedItem->getDialogueSegment();
		else
			m_selectedSegment = nullptr;
		fillItemSettings();
	}
	else if (m_selectedItem)
	{
		auto type = m_selectedSegment->getType();
		m_selectedSegment = std::make_shared<DialogueSegment>();
		m_selectedSegment->setType(type);

		if (ui->radioScript->isChecked())
		{
			m_selectedSegment->setScriptedText(true);
			m_selectedSegment->setText(ui->scriptEditText->toPlainText().toStdString());
		} else {
			m_selectedSegment->setScriptedText(false);
			m_selectedSegment->setText(ui->plainTextEdit->toPlainText().toStdString());
		}
		m_selectedSegment->setConditionalEnabled(ui->checkBoxConditional->isChecked());
		m_selectedSegment->setConditionScript(ui->scriptEditConditional->toPlainText().toStdString());
		m_selectedSegment->setScriptEnabled(ui->checkBoxScript->isChecked());
		m_selectedSegment->setScript(ui->scriptEdit->toPlainText().toStdString());

		if (m_treeModel->updateSegment(index, m_selectedSegment))
			std::cout << "updated seg" << std::endl;
	}
}

void DialogueWidget::on_plainTextEdit_textChanged()
{
	std::cout << "text changed" << std::endl;
}

void DialogueWidget::on_checkBoxConditional_toggled(bool checked)
{
	ui->scriptEditConditional->setEnabled(checked);
}

void DialogueWidget::on_checkBoxScript_toggled(bool checked)
{
	ui->scriptEdit->setEnabled(checked);
}
