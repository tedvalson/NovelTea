#include "ActionBuildWidget.hpp"
#include "MainWindow.hpp"
#include "ui_ActionBuildWidget.h"
#include <NovelTea/Game.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/Object.hpp>
#include <QLineEdit>
#include <iostream>

ActionBuildWidget::ActionBuildWidget(NovelTea::Context *context, QWidget *parent)
: QWidget(parent)
, NovelTea::ContextObject(context)
, ui(new Ui::ActionBuildWidget)
, m_value(sj::Array("", sj::Array()))
{
	ui->setupUi(this);
	refresh();
	ui->comboVerb->setCurrentIndex(-1);
	ui->comboVerb->lineEdit()->setPlaceholderText("[ Select Verb ]");
	connect(ui->comboVerb, SIGNAL(currentIndexChanged(QString)), this, SLOT(comboBox_currentIndexChanged(QString)));
}

ActionBuildWidget::~ActionBuildWidget()
{
	delete ui;
}

void ActionBuildWidget::setValue(sj::JSON value)
{
	if (getValue() != value)
	{
		if (!value.IsArray() || value.size() != 2 ||
				!value[1].IsArray() || value[1].IsEmpty())
			return;

		auto strVerb = QString::fromStdString(value[0].ToString());
		ui->comboVerb->setCurrentIndex(ui->comboVerb->findText(strVerb));
		if (value[1].size() != m_comboBoxes.size())
			return;

		for (auto i = 0; i < m_comboBoxes.size(); ++i)
		{
			auto index = m_comboBoxes[i]->findText(QString::fromStdString(value[1][i].ToString()));
			m_comboBoxes[i]->setCurrentIndex(index);
		}

		m_value = value;
		std::cout << "ActionBuildWidget value changed" << std::endl;
		emit valueChanged(value);
	}
}

sj::JSON ActionBuildWidget::getValue() const
{
	return m_value;
}

void ActionBuildWidget::renamed(NovelTea::EntityType entityType, const std::string &oldValue, const std::string &newValue)
{
	if (entityType != NovelTea::EntityType::Verb && entityType != NovelTea::EntityType::Object)
		return;

	auto value = m_value;
	if (entityType == NovelTea::EntityType::Verb && value[0].ToString() == oldValue)
		value[0] = newValue;
	else // Is Object
		for (auto &j : value[1].ArrayRange())
			if (j.ToString() == oldValue)
				j = newValue;

	blockSignals(true);
	refresh();
	setValue(value);
	blockSignals(false);
}

void ActionBuildWidget::refresh()
{
	auto model = qobject_cast<TreeModel*>(MainWindow::instance().getItemModel());
	auto objectModelIndex = model->index(EditorTabWidget::Object);
	auto verbModelIndex = model->index(EditorTabWidget::Verb);

	m_objectStrings.clear();
	fillObjects(model, objectModelIndex);

	auto verbText = ui->comboVerb->currentText();
	ui->comboVerb->clear();
	fillVerbs(model, verbModelIndex);
	ui->comboVerb->setCurrentIndex(ui->comboVerb->findText(verbText));
}

void ActionBuildWidget::on_comboVerb_currentIndexChanged(const QString &value)
{
	QLayoutItem *child;
	m_comboBoxes.clear();
	while ((child = ui->horizontalLayout->takeAt(1)) != 0)
	{
		delete child->widget(); // Deletes comboBox
		delete child;
	}

	auto verbId = value.toStdString();
	auto verb = Proj->get<NovelTea::Verb>(verbId, getContext());
	if (verb)
	{
		auto objectCount = verb->getObjectCount();

		for (int i = 0; i < objectCount; ++i)
		{
			auto comboBox = new QComboBox;
			comboBox->setEditable(true);
			comboBox->setInsertPolicy(QComboBox::NoInsert);
			comboBox->lineEdit()->setPlaceholderText(QString("[ object%1 ]").arg(i+1));
			comboBox->addItems(m_objectStrings);
			comboBox->setCurrentIndex(-1);

			connect(comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(comboBox_currentIndexChanged(QString)));

			ui->horizontalLayout->addWidget(comboBox);
			m_comboBoxes.push_back(comboBox);
		}
	}
	std::cout << "comboVerb changed: " << value.toStdString() << std::endl;
}

bool ActionBuildWidget::isValid() const
{
	auto verbId = ui->comboVerb->currentText().toStdString();
	auto verb = Proj->get<NovelTea::Verb>(verbId, getContext());
	if (!verb || verb->getObjectCount() != m_comboBoxes.size())
		return false;
	for (auto &comboBox : m_comboBoxes)
		if (!Proj->get<NovelTea::Object>(comboBox->currentText().toStdString(), getContext()))
			return false;
	return true;
}

void ActionBuildWidget::fillVerbs(const TreeModel *model, const QModelIndex &index)
{
	for (auto i = 0; i < model->rowCount(index); ++i)
		fillVerbs(model, model->index(i, 0, index));
	if (index.parent().isValid())
		ui->comboVerb->addItem(index.data().toString());
}

void ActionBuildWidget::fillObjects(const TreeModel *model, const QModelIndex &index)
{
	for (auto i = 0; i < model->rowCount(index); ++i)
		fillObjects(model, model->index(i, 0, index));
	if (index.parent().isValid())
		m_objectStrings << index.data().toString();
}

void ActionBuildWidget::comboBox_currentIndexChanged(const QString &value)
{
	if (isValid())
	{
		auto j = sj::Array();
		auto jobjects = sj::Array();
		for (auto &comboBox : m_comboBoxes)
		{
			auto objectId = comboBox->currentText().toStdString();
			jobjects.append(objectId);
		}
		j.append(ui->comboVerb->currentText().toStdString());
		j.append(jobjects);
		setValue(j);
	}

	QString actionSentence;
	auto verb = Proj->get<NovelTea::Verb>(ui->comboVerb->currentText().toStdString(), getContext());
	if (verb)
	{
		std::vector<std::string> objectIds;
		for (auto &comboBox : m_comboBoxes)
			objectIds.push_back(comboBox->currentText().toStdString());
		actionSentence = QString::fromStdString(verb->getActionText(objectIds));
	}

	ui->label->setText(actionSentence);
}
