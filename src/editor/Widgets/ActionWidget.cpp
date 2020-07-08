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
	_idName = idName;
	ui->setupUi(this);
	load();

	MODIFIER(ui->script, &ScriptEdit::textChanged);
	MODIFIER(ui->actionBuilder, &ActionBuildWidget::valueChanged);
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
		m_action->setVerbObjectCombo(ui->actionBuilder->getValue());
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
	ui->actionBuilder->setValue(m_action->getVerbObjectCombo());
}

void ActionWidget::on_pushButton_clicked()
{
	std::cout << ui->actionBuilder->getValue() << std::endl;
	ui->actionBuilder->refresh();
}
