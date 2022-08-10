#include "ScriptWidget.hpp"
#include "ui_ScriptWidget.h"
#include <NovelTea/Game.hpp>
#include <NovelTea/Script.hpp>
#include <QDebug>

ScriptWidget::ScriptWidget(const std::string &idName, QWidget *parent)
: EditorTabWidget(parent)
, ui(new Ui::ScriptWidget)
{
	m_idName = idName;
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
		m_script->setAutorun(ui->checkBoxAutorun->isChecked());
		m_script->setContent(ui->scriptEdit->toPlainText().toStdString());
		m_script->setProperties(ui->propertyEditor->getValue());
		Proj->set(m_script, idName());
	}
}

void ScriptWidget::loadData()
{
	m_script = Proj->get<NovelTea::Script>(idName(), getContext());

	qDebug() << "Loading Script data... " << QString::fromStdString(idName());

	if (!m_script)
	{
		// Script is new, so show it as modified
		setModified();
		m_script = std::make_shared<NovelTea::Script>(getContext());
	}

	ui->checkBoxAutorun->setChecked(m_script->getAutorun());
	ui->scriptEdit->setPlainText(QString::fromStdString(m_script->getContent()));
	ui->propertyEditor->setValue(m_script->getProperties());

	MODIFIER(ui->checkBoxAutorun, &QCheckBox::stateChanged);
	MODIFIER(ui->scriptEdit, &ScriptEdit::textChanged);
	MODIFIER(ui->propertyEditor, &PropertyEditor::valueChanged);
}
