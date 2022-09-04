#include "ActionWidget.hpp"
#include "ui_ActionWidget.h"
#include "ActionBuildWidget.hpp"
#include "MainWindow.hpp"
#include <NovelTea/Action.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/Game.hpp>
#include <iostream>
#include <QDebug>

ActionWidget::ActionWidget(const std::string &idName, QWidget *parent)
: EditorTabWidget(parent)
, ui(new Ui::ActionWidget)
{
	m_idName = idName;
	ui->setupUi(this);
	m_actionBuilder = new ActionBuildWidget(getContext(), ui->tabSettings);
	load();

	MODIFIER(ui->script, &ScriptEdit::textChanged);
	MODIFIER(ui->checkBox, &QCheckBox::toggled);
	MODIFIER(m_actionBuilder, &ActionBuildWidget::valueChanged);
	MODIFIER(ui->propertyEditor, &PropertyEditor::valueChanged);
	connect(&MainWindow::instance(), &MainWindow::renamed, m_actionBuilder, &ActionBuildWidget::renamed);
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
		m_action->setVerbObjectCombo(m_actionBuilder->getValue());
		m_action->setProperties(ui->propertyEditor->getValue());
		Proj->set(m_action, idName());
	}
}

void ActionWidget::loadData()
{
	m_action = Proj->get<NovelTea::Action>(idName(), getContext());

	if (!m_action)
	{
		// Object is new, so show it as modified
		setModified();
		m_action = std::make_shared<NovelTea::Action>(getContext());
	}

	ui->script->setPlainText(QString::fromStdString(m_action->getScript()));
	ui->checkBox->setChecked(!m_action->getPositionDependent());
	ui->propertyEditor->setValue(m_action->getProperties());
	m_actionBuilder->setValue(m_action->getVerbObjectCombo());
}

void ActionWidget::on_pushButton_clicked()
{
	std::cout << m_actionBuilder->getValue() << std::endl;
	m_actionBuilder->refresh();
}
