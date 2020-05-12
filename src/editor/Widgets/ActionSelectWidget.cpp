#include "ActionSelectWidget.hpp"
#include "MainWindow.hpp"
#include "ui_ActionSelectWidget.h"

ActionSelectWidget::ActionSelectWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ActionSelectWidget)
{
	ui->setupUi(this);
	ui->comboEntity->setModel(MainWindow::instance().getItemModel());
	ui->comboAction->setCurrentIndex(-1);
	connect(ui->comboAction, &QComboBox::currentTextChanged, this, &ActionSelectWidget::valueChanged);
	connect(ui->comboEntity, &QComboBox::currentTextChanged, this, &ActionSelectWidget::valueChanged);
}

ActionSelectWidget::~ActionSelectWidget()
{
	delete ui;
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

void ActionSelectWidget::on_comboAction_currentIndexChanged(int index)
{
	const int mapToModelIndex[] {1, 1, 0, 1};
	ui->comboEntity->setCurrentIndex(-1);
	if (index < 0)
	{
		ui->comboEntity->setEnabled(false);
		return;
	}

	ui->comboEntity->setEnabled(true);
	ui->comboEntity->setRootModelIndex(MainWindow::instance().getItemModel()->index(mapToModelIndex[index],0));
}
