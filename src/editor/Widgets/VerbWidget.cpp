#include "VerbWidget.hpp"
#include "ui_VerbWidget.h"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Verb.hpp>
#include <QDebug>

VerbWidget::VerbWidget(const std::string &idName, QWidget *parent)
	: EditorTabWidget(parent)
	, ui(new Ui::VerbWidget)
{
	_idName = idName;
	ui->setupUi(this);
	load();

	MODIFIER(ui->lineEditName, &QLineEdit::textChanged);
	MODIFIER(ui->scriptEditSuccess, &ScriptEdit::textChanged);
	MODIFIER(ui->scriptEditFailure, &ScriptEdit::textChanged);
	MODIFIER(ui->horizontalSlider, &QSlider::valueChanged);
	MODIFIER(ui->propertyEditor, &PropertyEditor::valueChanged);
}

VerbWidget::~VerbWidget()
{
	delete ui;
}

QString VerbWidget::tabText() const
{
	return QString::fromStdString(idName());
}

EditorTabWidget::Type VerbWidget::getType() const
{
	return EditorTabWidget::Verb;
}

void VerbWidget::saveData() const
{
	if (m_verb)
	{
		std::vector<std::string> actionStructure;
		for (auto lineEdit : m_lineEdits)
			actionStructure.push_back(lineEdit->text().toStdString());
		m_verb->setName(ui->lineEditName->text().toStdString());
		m_verb->setDefaultScriptSuccess(ui->scriptEditSuccess->toPlainText().toStdString());
		m_verb->setDefaultScriptFailure(ui->scriptEditFailure->toPlainText().toStdString());
		m_verb->setActionStructure(actionStructure);
		m_verb->setProperties(ui->propertyEditor->getValue());
		Proj.set<NovelTea::Verb>(m_verb, idName());
	}
}

void VerbWidget::loadData()
{
	m_verb = Proj.get<NovelTea::Verb>(idName());

	qDebug() << "Loading verb data... " << QString::fromStdString(idName());

	if (!m_verb)
	{
		// Object is new, so show it as modified
		setModified();
		m_verb = std::make_shared<NovelTea::Verb>();
	}

	ui->lineEditName->setText(QString::fromStdString(m_verb->getName()));
	ui->scriptEditSuccess->setPlainText(QString::fromStdString(m_verb->getDefaultScriptSuccess()));
	ui->scriptEditFailure->setPlainText(QString::fromStdString(m_verb->getDefaultScriptFailure()));
	ui->horizontalSlider->setValue(m_verb->getObjectCount());
	ui->propertyEditor->setValue(m_verb->getProperties());
	loadActionStructure();
}

void VerbWidget::loadActionStructure()
{
	m_lineEdits.clear();
	QLayoutItem *child;
	while ((child = ui->layoutActionStructure->takeAt(0)) != 0)
	{
		delete child->widget();
		delete child;
	}

	addLineEdit();
	for (int i = 0; i < m_verb->getObjectCount(); ++i)
	{
		auto label = new QLabel;
		label->setText(QString("object%1").arg(i+1));
		label->setMargin(10);
		ui->layoutActionStructure->addWidget(label);
		addLineEdit();
	}

	for (int i = 0; i < m_lineEdits.size(); ++i)
	{
		auto &lineEdit = m_lineEdits[i];
		std::string str;
		if (i < m_verb->getActionStructure().size())
			str = m_verb->getActionStructure()[i];
		if (i == 0 && str.empty())
			str = m_verb->getName();
		lineEdit->setText(QString::fromStdString(str));
		MODIFIER(lineEdit, &QLineEdit::textChanged);
	}
}

void VerbWidget::addLineEdit()
{
	auto lineEdit = new QLineEdit;
	lineEdit->setAlignment(Qt::AlignHCenter);
	ui->layoutActionStructure->addWidget(lineEdit);
	m_lineEdits.push_back(lineEdit);
}

void VerbWidget::on_horizontalSlider_valueChanged(int value)
{
	m_verb->setObjectCount(value);
	loadActionStructure();
}
