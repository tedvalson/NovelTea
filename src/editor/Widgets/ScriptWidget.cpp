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
		ProjData[NovelTea::Script::id][idName()] = *m_script;
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
}

void ScriptWidget::on_actionRemoveObject_triggered()
{
}
