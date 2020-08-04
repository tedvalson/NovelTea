#include "DialogueWidget.hpp"
#include "ui_DialogueWidget.h"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Dialogue.hpp>
#include <QDebug>

namespace {
	const auto propName = "Name";
}

DialogueWidget::DialogueWidget(const std::string &idName, QWidget *parent)
	: EditorTabWidget(parent)
	, ui(new Ui::DialogueWidget)
{
	m_idName = idName;
	ui->setupUi(this);
	load();
}

DialogueWidget::~DialogueWidget()
{
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

	ui->propertyEditor->setValue(m_dialogue->getProperties());

	MODIFIER(ui->propertyEditor, &PropertyEditor::valueChanged);
}

void DialogueWidget::on_actionRemoveObject_triggered()
{
}
