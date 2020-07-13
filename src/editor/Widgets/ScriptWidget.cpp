#include "ScriptWidget.hpp"
#include "ui_ScriptWidget.h"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Script.hpp>
#include <QDebug>

ScriptWidget::ScriptWidget(const std::string &idName, QWidget *parent)
: EditorTabWidget(parent)
, ui(new Ui::ScriptWidget)
{
	_idName = idName;
	ui->setupUi(this);
	load();
}

ScriptWidget::~ScriptWidget()
{
	delete ui;
}

QString ScriptWidget::tabText() const
{
	return QString::fromStdString(idName());
}

EditorTabWidget::Type ScriptWidget::getType() const
{
	return EditorTabWidget::Script;
}

void ScriptWidget::saveData() const
{
	if (m_script)
	{
		m_script->setGlobal(ui->checkBoxGlobal->isChecked());
		m_script->setAutorun(ui->checkBoxAutorun->isChecked());
		m_script->setContent(ui->scriptEdit->toPlainText().toStdString());
		Proj.set<NovelTea::Script>(m_script, idName());
	}
}

void ScriptWidget::loadData()
{
	m_script = Proj.get<NovelTea::Script>(idName());

	qDebug() << "Loading Script data... " << QString::fromStdString(idName());

	if (!m_script)
	{
		// Script is new, so show it as modified
		setModified();
		m_script = std::make_shared<NovelTea::Script>();
	}

	ui->checkBoxGlobal->setChecked(m_script->getGlobal());
	ui->checkBoxAutorun->setChecked(m_script->getAutorun());
	ui->scriptEdit->setPlainText(QString::fromStdString(m_script->getContent()));

	MODIFIER(ui->checkBoxGlobal, &QCheckBox::stateChanged);
	MODIFIER(ui->checkBoxAutorun, &QCheckBox::stateChanged);
	MODIFIER(ui->scriptEdit, &ScriptEdit::textChanged);
}
