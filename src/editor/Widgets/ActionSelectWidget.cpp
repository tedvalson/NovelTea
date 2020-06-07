#include "ActionSelectWidget.hpp"
#include "MainWindow.hpp"
#include "ui_ActionSelectWidget.h"
#include "Wizard/WizardPageActionSelect.hpp"
#include <QWizard>

ActionSelectWidget::ActionSelectWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ActionSelectWidget),
	m_value(nlohmann::json::array({-1,""}))
{
	ui->setupUi(this);
}

ActionSelectWidget::~ActionSelectWidget()
{
	delete ui;
}

void ActionSelectWidget::setValue(nlohmann::json value)
{
	if (getValue() != value)
	{
		m_value = value;
		emit valueChanged(value);

		auto type = static_cast<NovelTea::EntityType>(value[NovelTea::ID::entityType]);
		auto idText = QString::fromStdString(value[NovelTea::ID::entityId]).simplified();
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

		ui->label->setText(idText);
	}
}

nlohmann::json ActionSelectWidget::getValue() const
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