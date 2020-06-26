#include "ActionBuildWidget.hpp"
#include "MainWindow.hpp"
#include "ui_ActionBuildWidget.h"
#include <NovelTea/Verb.hpp>
#include <NovelTea/Object.hpp>
#include <QLineEdit>
#include <iostream>

ActionBuildWidget::ActionBuildWidget(QWidget *parent)
: QWidget(parent)
, ui(new Ui::ActionBuildWidget)
, m_value(json::array({"",{}}))
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

void ActionBuildWidget::setValue(nlohmann::json value)
{
	if (getValue() != value)
	{
		if (!value.is_array() || value.size() != 2 ||
				!value[1].is_array() || value[1].empty())
			return;

		auto strVerb = QString::fromStdString(value[0]);
		ui->comboVerb->setCurrentIndex(ui->comboVerb->findText(strVerb));
		if (value[1].size() != m_comboBoxes.size())
			return;

		for (auto i = 0; i < m_comboBoxes.size(); ++i)
		{
			auto index = m_comboBoxes[i]->findText(QString::fromStdString(value[1][i]));
			m_comboBoxes[i]->setCurrentIndex(index);
		}

		m_value = value;
		std::cout << "ActionBuildWidget value changed" << std::endl;
		emit valueChanged(value);
	}
}

nlohmann::json ActionBuildWidget::getValue() const
{
	return m_value;
}

void ActionBuildWidget::refresh()
{
	auto model = qobject_cast<TreeModel*>(MainWindow::instance().getItemModel());
	auto objectModelIndex = model->index(EditorTabWidget::Object);
	auto verbModelIndex = model->index(EditorTabWidget::Verb);

	auto verbText = ui->comboVerb->currentText();
	ui->comboVerb->clear();

	for (auto i = 0; i < model->rowCount(verbModelIndex); ++i)
		ui->comboVerb->addItem(model->index(i, 0, verbModelIndex).data().toString());
	ui->comboVerb->setCurrentIndex(ui->comboVerb->findText(verbText));

	m_objectStrings.clear();
	for (auto i = 0; i < model->rowCount(objectModelIndex); ++i)
		m_objectStrings << model->index(i, 0, objectModelIndex).data().toString();
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
	auto verb = Proj.verb(verbId);
	if (verb)
	{
		auto objectCount = verb->getObjectCount();

		for (int i = 0; i < objectCount; ++i)
		{
			auto comboBox = new QComboBox;
			comboBox->setEditable(true);
			comboBox->setInsertPolicy(QComboBox::NoInsert);
			comboBox->lineEdit()->setPlaceholderText(QString("[ Object %1 ]").arg(i+1));
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
	auto verb = Proj.verb(verbId);
	if (!verb || verb->getObjectCount() != m_comboBoxes.size())
		return false;
	for (auto &comboBox : m_comboBoxes)
		if (!Proj.object(comboBox->currentText().toStdString()))
			return false;
	return true;
}

void ActionBuildWidget::comboBox_currentIndexChanged(const QString &value)
{
	if (isValid())
	{
		auto j = json::array();
		auto jobjects = json::array();
		for (auto &comboBox : m_comboBoxes)
		{
			auto objectId = comboBox->currentText().toStdString();
			jobjects.push_back(objectId);
		}
		j.push_back(ui->comboVerb->currentText().toStdString());
		j.push_back(jobjects);
		setValue(j);
	}

	QString actionSentence;
	auto verb = Proj.verb(ui->comboVerb->currentText().toStdString());
	if (verb)
	{
		auto actionStructure = verb->getActionStructure();
		actionSentence += QString::fromStdString(actionStructure[0]);
		for (int i = 0; i < m_comboBoxes.size(); ++i)
		{
			auto strObject = m_comboBoxes[i]->currentText();
			auto object = Proj.object(strObject.toStdString());
			if (object)
				strObject = QString::fromStdString(object->getName()).toLower();
			else
				strObject = "______";
			actionSentence += " " + strObject;
			actionSentence += " " + QString::fromStdString(actionStructure[i+1]);
		}
	}

	ui->label->setText(actionSentence);
}
