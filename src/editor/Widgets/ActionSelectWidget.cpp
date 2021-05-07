#include "ActionSelectWidget.hpp"
#include "MainWindow.hpp"
#include "ui_ActionSelectWidget.h"
#include "Wizard/WizardPageActionSelect.hpp"
#include <QWizard>

ActionSelectWidget::ActionSelectWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ActionSelectWidget),
	m_value(sj::Array(-1,""))
{
	ui->setupUi(this);
	connect(&MainWindow::instance(), &MainWindow::renamed, this, &ActionSelectWidget::renamed);
}

ActionSelectWidget::~ActionSelectWidget()
{
	delete ui;
}

void ActionSelectWidget::setValue(sj::JSON value)
{
	if (getValue() != value)
	{
		m_value = value;
		emit valueChanged(value);
		refresh();
	}
}

sj::JSON ActionSelectWidget::getValue() const
{
	return m_value;
}

void ActionSelectWidget::on_pushButton_clicked()
{
	QWizard wizard;
	auto wizardPageActionSelect = new WizardPageActionSelect;
	wizardPageActionSelect->setValue(getValue());
	wizard.addPage(wizardPageActionSelect);
	auto result = wizard.exec();

	if (result == QDialog::Accepted)
		setValue(wizardPageActionSelect->getValue());
}

void ActionSelectWidget::renamed(NovelTea::EntityType entityType, const std::string &oldValue, const std::string &newValue)
{
	auto value = m_value;
	auto type = static_cast<NovelTea::EntityType>(value[NovelTea::ID::selectEntityType].ToInt());
	if (entityType == type && oldValue == value[NovelTea::ID::selectEntityId].ToString())
	{
		value[NovelTea::ID::selectEntityId] = newValue;
		blockSignals(true);
		setValue(value);
		blockSignals(false);
	}
}

void ActionSelectWidget::refresh()
{
	auto type = static_cast<NovelTea::EntityType>(m_value[NovelTea::ID::selectEntityType].ToInt());
	auto idText = QString::fromStdString(m_value[NovelTea::ID::selectEntityId].ToString()).simplified();
	if (idText.length() > 30)
	{
		idText.truncate(28);
		idText += "...";
	}
	idText = idText.toHtmlEscaped();

	if (type == NovelTea::EntityType::Cutscene)
		idText = "<b>Cutscene:</b> " + idText;
	else if (type == NovelTea::EntityType::CustomScript)
		idText = "<b>Custom:</b> " + idText;
	else if (type == NovelTea::EntityType::Dialogue)
		idText = "<b>Dialogue:</b> " + idText;
	else if (type == NovelTea::EntityType::Room)
		idText = "<b>Room:</b> " + idText;
	else if (type == NovelTea::EntityType::Script)
		idText = "<b>Script:</b> " + idText;
	else
		idText = "<Nothing Selected>";

	ui->label->setText(idText);
}
