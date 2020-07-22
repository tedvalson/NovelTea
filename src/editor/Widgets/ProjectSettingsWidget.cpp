#include "ProjectSettingsWidget.hpp"
#include "MainWindow.hpp"
#include "ui_ProjectSettingsWidget.h"
#include <NovelTea/ProjectData.hpp>
#include <QStandardPaths>
#include <QDir>
#include <QRawFont>
#include <QDebug>

using namespace NovelTea;

ProjectSettingsWidget::ProjectSettingsWidget(QWidget *parent) :
	EditorTabWidget(parent),
	ui(new Ui::ProjectSettingsWidget),
	defaultFontIndex(-1)
{
	ui->setupUi(this);
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
	MODIFIER(ui->actionSelect, &ActionSelectWidget::valueChanged);
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
	j[ID::projectName] = ui->lineEditName->text().toStdString();
	j[ID::projectVersion] = ui->lineEditVersion->text().toStdString();
	j[ID::projectAuthor] = ui->lineEditAuthor->text().toStdString();
	j[ID::projectWebsite] = ui->lineEditWebsite->text().toStdString();
	j[ID::entrypointEntity] = ui->actionSelect->getValue();
	j[ID::projectFontDefault] = defaultFontIndex;
	MainWindow::instance().reloadProject();
}

void ProjectSettingsWidget::loadData()
{
	auto &j = ProjData;
	ui->lineEditName->setText(QString::fromStdString(j.value(ID::projectName, "Project Name")));
	ui->lineEditVersion->setText(QString::fromStdString(j.value(ID::projectVersion, "1.0")));
	ui->lineEditAuthor->setText(QString::fromStdString(j.value(ID::projectAuthor, "Project Author")));
	ui->lineEditWebsite->setText(QString::fromStdString(j.value(ID::projectWebsite, "")));

	auto entryPoint = j.value(ID::entrypointEntity, json::array());
	ui->actionSelect->setValue(entryPoint);

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

	makeFontDefault(j.value(ID::projectFontDefault, 0));
}

void ProjectSettingsWidget::on_lineEditFontPreview_textChanged(const QString &arg1)
{
	ui->labelFontPreview->setText(arg1);
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
