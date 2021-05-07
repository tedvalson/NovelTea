#include "ActionWidget.hpp"
#include "ui_ActionWidget.h"
#include "MainWindow.hpp"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/Object.hpp>
#include <iostream>
#include <QDebug>

ActionWidget::ActionWidget(const std::string &idName, QWidget *parent)
: EditorTabWidget(parent)
, ui(new Ui::ActionWidget)
{
	m_idName = idName;
	ui->setupUi(this);
	load();

	MODIFIER(ui->script, &ScriptEdit::textChanged);
	MODIFIER(ui->checkBox, &QCheckBox::toggled);
	MODIFIER(ui->actionBuilder, &ActionBuildWidget::valueChanged);
	MODIFIER(ui->propertyEditor, &PropertyEditor::valueChanged);
	connect(&MainWindow::instance(), &MainWindow::renamed, ui->actionBuilder, &ActionBuildWidget::renamed);
}

ActionWidget::~ActionWidget()
{
	delete ui;
}

QString ActionWidget::tabText() const
{
	return QString::fromStdString(idName());
}

EditorTabWidget::Type ActionWidget::getType() const
{
	return EditorTabWidget::Action;
}

void ActionWidget::saveData() const
{
	if (m_action)
	{
		m_action->setScript(ui->script->toPlainText().toStdString());
		m_action->setPositionDependent(!ui->checkBox->isChecked());
		m_action->setVerbObjectCombo(ui->actionBuilder->getValue());
		m_action->setProperties(ui->propertyEditor->getValue());
		Proj.set<NovelTea::Action>(m_action, idName());
	}
}

void ActionWidget::loadData()
{
	m_action = Proj.get<NovelTea::Action>(idName());

	qDebug() << "Loading action data... " << QString::fromStdString(idName());

	if (!m_action)
	{
		// Object is new, so show it as modified
		setModified();
		m_action = std::make_shared<NovelTea::Action>();
	}

	ui->script->setPlainText(QString::fromStdString(m_action->getScript()));
	ui->checkBox->setChecked(!m_action->getPositionDependent());
	ui->actionBuilder->setValue(m_action->getVerbObjectCombo());
	ui->propertyEditor->setValue(m_action->getProperties());
}

void ActionWidget::on_pushButton_clicked()
{
	std::cout << ui->actionBuilder->getValue() << std::endl;
	ui->actionBuilder->refresh();
}
