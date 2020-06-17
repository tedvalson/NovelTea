#include "WizardPageActionSelect.hpp"
#include "ui_WizardPageActionSelect.h"
#include "Widgets/MainWindow.hpp"
#include "TreeItem.hpp"

WizardPageActionSelect::WizardPageActionSelect(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::WizardPageActionSelect)
{
	ui->setupUi(this);
	ui->treeView->setModel(MainWindow::instance().getItemModel());
	ui->treeView->setHeaderHidden(true);
	setFilterRegExp("^(?!Objects|Verbs)");

	ui->scriptEdit->hide();
	ui->treeView->hide();

	connect(ui->radioExisting, &QRadioButton::toggled, this, &QWizardPage::completeChanged);
	connect(ui->radioCustom, &QRadioButton::toggled, this, &QWizardPage::completeChanged);
	connect(ui->treeView, &QTreeView::clicked, this, &QWizardPage::completeChanged);
	connect(ui->scriptEdit, &QTextEdit::textChanged, this, &QWizardPage::completeChanged);

	startTimer(50);
}

WizardPageActionSelect::~WizardPageActionSelect()
{
	delete ui;
}

void WizardPageActionSelect::setValue(nlohmann::json value)
{
	auto type = static_cast<NovelTea::EntityType>(value[NovelTea::ID::entityType]);
	if (type == NovelTea::EntityType::CustomScript)
	{
		ui->radioCustom->setChecked(true);
		ui->scriptEdit->setText(QString::fromStdString(value[NovelTea::ID::entityId]));
	}
	else if (type != NovelTea::EntityType::Invalid)
	{
		ui->radioExisting->setChecked(true);
	}
}

nlohmann::json WizardPageActionSelect::getValue() const
{
	if (isComplete())
	{
		if (ui->radioExisting->isChecked())
		{
			auto index = ui->treeView->mapToSource(ui->treeView->currentIndex());
			auto treeItem = static_cast<TreeItem*>(index.internalPointer());
			if (treeItem)
			{
				auto typeData = treeItem->data(1);
				if (typeData.isValid())
				{
					auto type = static_cast<NovelTea::EntityType>(typeData.toInt());
					auto id = treeItem->data(0).toString().toStdString();
					return json::array({type, id});
				}
			}
		}
		if (ui->radioCustom->isChecked())
			return json::array({NovelTea::EntityType::CustomScript, ui->scriptEdit->toPlainText().toStdString()});
	}

	return json::array({-1,""});
}

void WizardPageActionSelect::setFilterRegExp(const QString &pattern)
{
	ui->treeView->setFilterRegExp(pattern);
}

void WizardPageActionSelect::allowCustomScript(bool allow)
{
	ui->radioCustom->setVisible(allow);
	ui->radioExisting->setVisible(allow);
	if (!allow)
	{
		ui->radioExisting->setChecked(true);
		ui->treeView->expandToDepth(1);
	}
}

bool WizardPageActionSelect::isComplete() const
{
	return ((ui->radioExisting->isChecked() && currentIndex.parent().isValid()) ||
			(ui->radioCustom->isChecked() && !ui->scriptEdit->toPlainText().isEmpty()));
}

void WizardPageActionSelect::timerEvent(QTimerEvent *)
{
	if (currentIndex != ui->treeView->currentIndex())
	{
		currentIndex = ui->treeView->currentIndex();
		emit completeChanged();
	}
}

void WizardPageActionSelect::on_radioExisting_toggled(bool checked)
{
	if (checked)
	{
		ui->treeView->show();
		ui->scriptEdit->hide();
	}
}

void WizardPageActionSelect::on_radioCustom_toggled(bool checked)
{
	if (checked)
	{
		ui->treeView->hide();
		ui->scriptEdit->show();
	}
}
