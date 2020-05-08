#include "ActionSelectWidget.hpp"
#include "ui_ActionSelectWidget.h"

ActionSelectWidget::ActionSelectWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ActionSelectWidget)
{
	ui->setupUi(this);
}

ActionSelectWidget::~ActionSelectWidget()
{
	delete ui;
}

void ActionSelectWidget::setModel(QAbstractItemModel *model)
{
	itemModel = model;
	ui->comboEntity->setModel(model);
}

void ActionSelectWidget::setValue(nlohmann::json value)
{
	ui->comboAction->setCurrentIndex(value[0]);
	ui->comboEntity->setCurrentText(QString::fromStdString(value[1]));
}

nlohmann::json ActionSelectWidget::getValue() const
{
	return nlohmann::json::array({
		ui->comboAction->currentIndex(),
		ui->comboEntity->currentText().toStdString()
	});
}
