#include "DialogueWidget.hpp"
#include "ui_DialogueWidget.h"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/DialogueSegment.hpp>
#include <QDebug>

DialogueWidget::DialogueWidget(const std::string &idName, QWidget *parent)
	: EditorTabWidget(parent)
	, ui(new Ui::DialogueWidget)
	, m_treeModel(new DialogueTreeModel)
	, m_selectedItem(nullptr)
	, m_menuTreeView(new QMenu)
{
	m_idName = idName;
	ui->setupUi(this);

	ui->treeView->setModel(m_treeModel);

	m_menuTreeView->addAction(ui->actionAddObject);

	load();
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

	qDebug() << "Loading Dialogue data... " << QString::fromStdString(idName());

	if (!m_dialogue)
	{
		// Dialogue is new, so show it as modified
		setModified();
		m_dialogue = std::make_shared<NovelTea::Dialogue>();
	}

	m_treeModel->loadDialogue(m_dialogue);
	ui->propertyEditor->setValue(m_dialogue->getProperties());

	MODIFIER(ui->propertyEditor, &PropertyEditor::valueChanged);
	MODIFIER(m_treeModel, &QAbstractItemModel::dataChanged);
	MODIFIER(m_treeModel, &QAbstractItemModel::rowsInserted);
	MODIFIER(m_treeModel, &QAbstractItemModel::rowsRemoved);
}

void DialogueWidget::on_treeView_pressed(const QModelIndex &index)
{
	if (QApplication::mouseButtons() != Qt::RightButton)
		return;
	m_selectedItem = static_cast<DialogueTreeItem*>(index.internalPointer());

	m_menuTreeView->popup(QCursor::pos());
}

void DialogueWidget::on_actionAddObject_triggered()
{
	auto selectedIndex = ui->treeView->currentIndex();
	if (m_treeModel->insertRow(0, selectedIndex))
	{
		m_treeModel->setData(m_treeModel->index(0, 0, selectedIndex), QString::fromStdString("test"));
		m_treeModel->setData(m_treeModel->index(0, 1, selectedIndex), NovelTea::DialogueSegment::Text);
	}
}
