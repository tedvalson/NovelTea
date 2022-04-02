#include "DialogueWidget.hpp"
#include "ui_DialogueWidget.h"
#include "DialogueTreeItem.hpp"
#include "MainWindow.hpp"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/DialogueSegment.hpp>
#include <QMessageBox>
#include <QDebug>

using NovelTea::DialogueSegment;

DialogueWidget::DialogueWidget(const std::string &idName, QWidget *parent)
	: EditorTabWidget(parent)
	, ui(new Ui::DialogueWidget)
	, m_treeModel(new DialogueTreeModel)
	, m_menuTreeView(new QMenu)
	, m_selectedItem(nullptr)
{
	m_idName = idName;
	ui->setupUi(this);

	ui->treeView->setModel(m_treeModel);

	m_menuTreeView->addAction(ui->actionAddObject);
	m_menuTreeView->addAction(ui->actionDelete);
	m_menuTreeView->addSeparator();
	m_menuTreeView->addAction(ui->actionPlayFromHere);
	m_menuTreeView->addSeparator();
	m_menuTreeView->addAction(ui->actionMoveUp);
	m_menuTreeView->addAction(ui->actionMoveDown);
	m_menuTreeView->addSeparator();
	m_menuTreeView->addAction(ui->actionCut);
	m_menuTreeView->addAction(ui->actionCopy);
	m_menuTreeView->addAction(ui->actionPaste);
	m_menuTreeView->addAction(ui->actionPasteAsLink);

	ui->radioText->setChecked(true);
	ui->checkBoxEnableDisabled->setVisible(false);

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
		m_dialogue->setDefaultName(ui->lineEditDefaultName->text().toStdString());
		m_dialogue->setNextEntityJson(ui->actionSelectWidget->getValue());
		m_dialogue->setShowDisabledOptions(ui->checkBoxShowDisabled->isChecked());
		m_dialogue->setEnableDisabledOptions(ui->checkBoxEnableDisabled->isChecked());
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
	ui->propertyEditor->setValue(m_dialogue->getProperties());
	ui->lineEditDefaultName->setText(QString::fromStdString(m_dialogue->getDefaultName()));
	ui->checkBoxShowDisabled->setChecked(m_dialogue->getShowDisabledOptions());
	ui->checkBoxEnableDisabled->setChecked(m_dialogue->getEnableDisabledOptions());
	ui->actionSelectWidget->setValue(m_dialogue->getNextEntityJson());
	fillItemSettings();

	MODIFIER(ui->propertyEditor, &PropertyEditor::valueChanged);
	MODIFIER(m_treeModel, &QAbstractItemModel::dataChanged);
	MODIFIER(m_treeModel, &QAbstractItemModel::rowsInserted);
	MODIFIER(m_treeModel, &QAbstractItemModel::rowsRemoved);
	MODIFIER(m_treeModel, &QAbstractItemModel::rowsMoved);
	MODIFIER(ui->lineEditDefaultName, &QLineEdit::textChanged);
	MODIFIER(ui->checkBoxShowDisabled, &QCheckBox::clicked);
	MODIFIER(ui->checkBoxEnableDisabled, &QCheckBox::clicked);
	MODIFIER(ui->actionSelectWidget, &ActionSelectWidget::valueChanged);
}

void DialogueWidget::on_treeView_pressed(const QModelIndex &index)
{
	checkIndexChange();
	if (QApplication::mouseButtons() != Qt::RightButton)
		return;

	auto row = index.row();
	auto type = m_selectedItem ? m_selectedItem->getDialogueSegment()->getType() : DialogueSegment::Root;
	auto cutting = m_cutIndex.isValid();
	auto copying = m_copyIndex.isValid();
	auto isLink = type == DialogueSegment::Link;

	ui->actionAddObject->setEnabled(!isLink);
	ui->actionDelete->setEnabled(type != DialogueSegment::Root);
	ui->actionMoveUp->setEnabled(row > 0);
	ui->actionMoveDown->setEnabled(index.sibling(row+1, 0).isValid());
//	ui->actionCut->setChecked(!isLink);
	ui->actionCopy->setChecked(!isLink);
	ui->actionPaste->setEnabled(!isLink && (copying || cutting));
	ui->actionPasteAsLink->setEnabled(!isLink && copying);

	m_menuTreeView->popup(QCursor::pos());
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

	auto &selectedSegment = m_selectedItem->getDialogueSegment();
	ui->tabText->setEnabled(true);
	ui->tabConditional->setEnabled(true);
	ui->tabScript->setEnabled(true);

	if (selectedSegment->getScriptedText())
	{
		ui->radioScript->setChecked(true);
		ui->plainTextEdit->clear();
		ui->scriptEditText->setPlainText(QString::fromStdString(selectedSegment->getTextRaw()));
	} else {
		ui->radioText->setChecked(true);
		ui->scriptEditText->clear();
		ui->plainTextEdit->setPlainText(QString::fromStdString(selectedSegment->getTextRaw()));
	}

	ui->checkBoxConditional->setChecked(selectedSegment->getConditionalEnabled());
	ui->scriptEditConditional->setPlainText(QString::fromStdString(selectedSegment->getConditionScript()));
	ui->checkBoxScript->setChecked(selectedSegment->getScriptEnabled());
	ui->scriptEdit->setPlainText(QString::fromStdString(selectedSegment->getScript()));
	ui->checkBoxShowOnce->setChecked(selectedSegment->getShowOnce());
	ui->checkBoxAutosave->setChecked(selectedSegment->getAutosave());
}

void DialogueWidget::checkIndexChange()
{
	auto index = ui->treeView->currentIndex();
	auto selectedItem = static_cast<DialogueTreeItem*>(index.internalPointer());
	if (!index.isValid() || !selectedItem->parent()->parent())
		selectedItem = nullptr;
	if (selectedItem && selectedItem->getLink())
		selectedItem = selectedItem->getLink();

	if (m_selectedItem != selectedItem)
	{
		m_selectedItem = selectedItem;
		fillItemSettings();
	}
	else if (m_selectedItem)
	{
		auto type = m_selectedItem->getDialogueSegment()->getType();
		auto segment = std::make_shared<DialogueSegment>();
		segment->setType(type);
		segment->setDialogue(m_dialogue.get());

		if (ui->radioScript->isChecked())
		{
			segment->setScriptedText(true);
			segment->setTextRaw(ui->scriptEditText->toPlainText().toStdString());
		} else {
			segment->setScriptedText(false);
			segment->setTextRaw(ui->plainTextEdit->toPlainText().toStdString());
		}
		segment->setConditionalEnabled(ui->checkBoxConditional->isChecked());
		segment->setConditionScript(ui->scriptEditConditional->toPlainText().toStdString());
		segment->setScriptEnabled(ui->checkBoxScript->isChecked());
		segment->setScript(ui->scriptEdit->toPlainText().toStdString());
		segment->setShowOnce(ui->checkBoxShowOnce->isChecked());
		segment->setAutosave(ui->checkBoxAutosave->isChecked());

		if (m_treeModel->updateSegment(index, segment)) {
			ui->treeView->resizeColumnToContents(0);
		}
	}
}

void DialogueWidget::on_plainTextEdit_textChanged()
{
}

void DialogueWidget::on_checkBoxConditional_toggled(bool checked)
{
	ui->scriptEditConditional->setEnabled(checked);
}

void DialogueWidget::on_checkBoxScript_toggled(bool checked)
{
	ui->scriptEdit->setEnabled(checked);
}

void DialogueWidget::on_actionAddObject_triggered()
{
	auto index = ui->treeView->currentIndex();
	auto type = m_selectedItem ? m_selectedItem->getDialogueSegment()->getType() : DialogueSegment::Root;
	m_cutIndex = QModelIndex();
	m_copyIndex = QModelIndex();

	if (type == DialogueSegment::Link)
		return;

	auto newSegment = std::make_shared<DialogueSegment>();
	newSegment->setDialogue(m_dialogue.get());
	if (type == DialogueSegment::Root || type == DialogueSegment::Option)
	{
		newSegment->setType(DialogueSegment::Text);
//		newSegment->setTextRaw("NPC");
	} else {
		newSegment->setType(DialogueSegment::Option);
//		newSegment->setTextRaw("Player");
	}

	m_treeModel->insertSegment(0, index, newSegment);
	ui->treeView->setCurrentIndex(index.child(0,0));
}

void DialogueWidget::on_actionDelete_triggered()
{
	auto selectedIndex = ui->treeView->currentIndex();
	m_cutIndex = QModelIndex();
	m_copyIndex = QModelIndex();
	m_treeModel->removeRow(selectedIndex.row(), selectedIndex.parent());
}

void DialogueWidget::on_actionCut_triggered()
{
	m_cutIndex = ui->treeView->currentIndex();
	m_copyIndex = QModelIndex();
}

void DialogueWidget::on_actionCopy_triggered()
{
	m_copyIndex = ui->treeView->currentIndex();
	m_cutIndex = QModelIndex();
}

void DialogueWidget::on_actionPaste_triggered()
{
	auto index = ui->treeView->currentIndex();
	if (m_copyIndex.isValid())
		m_treeModel->copy(m_copyIndex, index);
	else if (m_cutIndex.isValid())
		m_treeModel->moveRow(m_cutIndex.parent(), m_cutIndex.row(), index, 0);
	m_copyIndex = QModelIndex();
	m_cutIndex = QModelIndex();
}

void DialogueWidget::on_actionPasteAsLink_triggered()
{
	auto index = ui->treeView->currentIndex();
	m_treeModel->insertSegmentLink(m_copyIndex, index);
	m_copyIndex = QModelIndex();
	m_cutIndex = QModelIndex();
}

void DialogueWidget::on_actionMoveUp_triggered()
{
	auto index = ui->treeView->currentIndex();
	auto row = index.row();
	m_treeModel->moveRow(index.parent(), row, index.parent(), row-1);
}

void DialogueWidget::on_actionMoveDown_triggered()
{
	auto index = ui->treeView->currentIndex();
	auto row = index.row();
	m_treeModel->moveRow(index.parent(), row, index.parent(), row+2);
}

void DialogueWidget::on_lineEditDefaultName_textChanged(const QString &arg1)
{
	m_dialogue->setDefaultName(arg1.toStdString());
	// TODO: Better way to force update of visible items?
	ui->treeView->hide();
	ui->treeView->show();
}

void DialogueWidget::on_treeView_expanded(const QModelIndex &index)
{
	ui->treeView->resizeColumnToContents(0);
}

void DialogueWidget::on_treeView_collapsed(const QModelIndex &index)
{
	ui->treeView->resizeColumnToContents(0);
}

void DialogueWidget::on_checkBoxShowDisabled_toggled(bool checked)
{
	ui->checkBoxEnableDisabled->setChecked(false);
	ui->checkBoxEnableDisabled->setVisible(checked);
}

void DialogueWidget::on_actionPlayFromHere_triggered()
{
	// Dialogue needs to be saved first
	if (isModified())
	{
		auto result = QMessageBox::warning(this, "Dialogue Changed", "You need to save changes before previewing.",
				QMessageBox::Save | QMessageBox::Cancel, QMessageBox::Save);
		if (result == QMessageBox::Cancel)
			return;
		save();
		Proj.saveToFile();
	}
	int i = m_selectedItem->getDialogueSegment()->getId();
	MainWindow::instance().launchPreview(NovelTea::EntityType::Dialogue, idName(), sj::Array("", i));
}
