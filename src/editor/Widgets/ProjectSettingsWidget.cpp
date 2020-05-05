#include "ProjectSettingsWidget.hpp"
#include "ui_ProjectSettingsWidget.h"
#include <NovelTea/ProjectData.hpp>
#include <QStandardPaths>
#include <QDir>
#include <QRawFont>
#include <QDebug>

ProjectSettingsWidget::ProjectSettingsWidget(QAbstractItemModel *model, QWidget *parent) :
	EditorTabWidget(parent),
	ui(new Ui::ProjectSettingsWidget),
	itemModel(model),
	defaultFontIndex(-1)
{
	ui->setupUi(this);
	ui->comboStartingEntity->setModel(itemModel);
	load();

	// Set default font preview
	QFont font(ui->listFonts->item(0)->text(), 20);
	ui->labelFontPreview->setFont(font);
	ui->lineEditFontPreview->setText("Preview Text");

	// Connect all modifying signals
	MODIFIER(ui->lineEditName, &QLineEdit::textChanged);
	MODIFIER(ui->lineEditVersion, &QLineEdit::textChanged);
	MODIFIER(ui->lineEditAuthor, &QLineEdit::textChanged);
	MODIFIER(ui->lineEditWebsite, &QLineEdit::textChanged);
	MODIFIER(ui->buttonSetDefaultFont, &QPushButton::clicked);
	MODIFIER(ui->comboStartingAction, &QComboBox::currentTextChanged);
	MODIFIER(ui->comboStartingEntity, &QComboBox::currentTextChanged);
}

ProjectSettingsWidget::~ProjectSettingsWidget()
{
	delete ui;
}

QString ProjectSettingsWidget::tabText() const
{
	return "Settings";
}

EditorTabWidget::Type ProjectSettingsWidget::getType() const
{
	return EditorTabWidget::Settings;
}

void ProjectSettingsWidget::addFont(const QString &name)
{
	auto fontItem = new QListWidgetItem(name);
	fontItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
	fontItem->setCheckState(Qt::Unchecked);
	ui->listFonts->addItem(fontItem);
}

void ProjectSettingsWidget::makeFontDefault(int index)
{
	if (index >= ui->listFonts->count())
		index = 0;

	// Unmark previous default, if exists
	if (defaultFontIndex >= 0)
	{
		auto item = ui->listFonts->item(defaultFontIndex);
		auto font = item->font();
		font.setBold(false);
		item->setFont(font);
	}

	// Mark new default
	auto item = ui->listFonts->item(index);
	auto font = item->font();
	font.setBold(true);
	item->setFont(font);
	defaultFontIndex = index;
}

void ProjectSettingsWidget::saveData() const
{
	auto &j = ProjData;
	j[NT_PROJECT_NAME] = ui->lineEditName->text().toStdString();
	j[NT_PROJECT_VERSION] = ui->lineEditVersion->text().toStdString();
	j[NT_PROJECT_AUTHOR] = ui->lineEditAuthor->text().toStdString();
	j[NT_PROJECT_WEBSITE] = ui->lineEditWebsite->text().toStdString();
	j[NT_PROJECT_ENTRYPOINT][NT_ENTITY_TYPE] = ui->comboStartingAction->currentIndex();
	j[NT_PROJECT_ENTRYPOINT][NT_ENTITY_ID] = ui->comboStartingEntity->currentText().toStdString();
	j[NT_FONT_DEFAULT] = defaultFontIndex;
}

void ProjectSettingsWidget::loadData()
{
	auto &j = ProjData;
	ui->lineEditName->setText(QString::fromStdString(j.value(NT_PROJECT_NAME, "Project Name")));
	ui->lineEditVersion->setText(QString::fromStdString(j.value(NT_PROJECT_VERSION, "1.0")));
	ui->lineEditAuthor->setText(QString::fromStdString(j.value(NT_PROJECT_AUTHOR, "Project Author")));
	ui->lineEditWebsite->setText(QString::fromStdString(j.value(NT_PROJECT_WEBSITE, "")));

	auto entryPoint = j.value(NT_PROJECT_ENTRYPOINT, json::object());
	ui->comboStartingAction->setCurrentIndex(entryPoint.value(NT_ENTITY_TYPE, 0));
	ui->comboStartingEntity->setCurrentText(QString::fromStdString(entryPoint.value(NT_ENTITY_ID, "")));

	ui->listFonts->clear();
	ui->listFonts->addItem("DejaVu Serif");
	ui->listFonts->addItem("DejaVu Sans");

	auto fontPath = QStandardPaths::writableLocation(QStandardPaths::FontsLocation);
	QDir dir(fontPath);
	QStringList nameFilters;
	nameFilters << "*.ttf" << "*.otf";
	dir.setNameFilters(nameFilters);
	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	QFileInfoList list = dir.entryInfoList();
	for (int i = 0; i < list.size(); ++i)
	{
		QFileInfo fileInfo = list.at(i);
		QRawFont font(fileInfo.absoluteFilePath(), 10);
//		qDebug() << font.familyName();
		addFont(font.familyName());
	}

	makeFontDefault(j.value(NT_FONT_DEFAULT, 0));
}

void ProjectSettingsWidget::on_lineEditFontPreview_textChanged(const QString &arg1)
{
	ui->labelFontPreview->setText(arg1);
}

void ProjectSettingsWidget::on_comboStartingAction_currentIndexChanged(int index)
{
	qDebug() << "action changed: " << index;
	if (index == 0)
	{
		ui->comboStartingEntity->setRootModelIndex(itemModel->index(0,0));
	}
	else if (index == 1)
	{
		ui->comboStartingEntity->setRootModelIndex(itemModel->index(1,0));
	}

	ui->comboStartingEntity->setCurrentIndex(-1);
}

void ProjectSettingsWidget::on_listFonts_currentRowChanged(int currentRow)
{
	QFont font(ui->listFonts->currentItem()->text(), 20);
	ui->labelFontPreview->setFont(font);
	ui->buttonSetDefaultFont->setEnabled(currentRow != defaultFontIndex);
}

void ProjectSettingsWidget::on_buttonImportFont_clicked()
{

}

void ProjectSettingsWidget::on_buttonSetDefaultFont_clicked()
{
	makeFontDefault(ui->listFonts->currentRow());
}
