#include "ProjectSettingsWidget.hpp"
#include "MainWindow.hpp"
#include "ui_ProjectSettingsWidget.h"
#include "Wizard/WizardPageActionSelect.hpp"
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
	ui->comboVerb->lineEdit()->setPlaceholderText("[ Select Verb ]");
	load();

	// Set default font preview
	QFont font(ui->listFonts->item(0)->text(), 20);
	ui->labelFontPreview->setFont(font);
	ui->lineEditFontPreview->setText("Preview Text");

	// Connect all modifying signals
	MODIFIER(ui->listInventory->model(), &QAbstractItemModel::dataChanged);
	MODIFIER(ui->listInventory->model(), &QAbstractItemModel::rowsInserted);
	MODIFIER(ui->listInventory->model(), &QAbstractItemModel::rowsRemoved);
	MODIFIER(ui->lineEditName, &QLineEdit::textChanged);
	MODIFIER(ui->lineEditVersion, &QLineEdit::textChanged);
	MODIFIER(ui->lineEditAuthor, &QLineEdit::textChanged);
	MODIFIER(ui->lineEditWebsite, &QLineEdit::textChanged);
	MODIFIER(ui->buttonSetDefaultFont, &QPushButton::clicked);
	MODIFIER(ui->actionSelect, &ActionSelectWidget::valueChanged);
	MODIFIER(ui->comboVerb, &QComboBox::currentTextChanged);
	MODIFIER(ui->scriptBeforeSaveEdit, &ScriptEdit::textChanged);
	MODIFIER(ui->scriptAfterLoadEdit, &ScriptEdit::textChanged);
	MODIFIER(ui->scriptAfterActionEdit, &ScriptEdit::textChanged);
	MODIFIER(ui->scriptBeforeActionEdit, &ScriptEdit::textChanged);
	MODIFIER(ui->scriptUndefinedActionEdit, &ScriptEdit::textChanged);
	MODIFIER(ui->scriptBeforeLeaveEdit, &ScriptEdit::textChanged);
	MODIFIER(ui->scriptAfterLeaveEdit, &ScriptEdit::textChanged);
	MODIFIER(ui->scriptBeforeEnterEdit, &ScriptEdit::textChanged);
	MODIFIER(ui->scriptAfterEnterEdit, &ScriptEdit::textChanged);
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
	auto jobjects = sj::Array();
	for (int i = 0; i < ui->listInventory->count(); ++i) {
		auto item = ui->listInventory->item(i);
		jobjects.append(item->text().toStdString());
	}

	j[ID::projectName] = ui->lineEditName->text().toStdString();
	j[ID::projectVersion] = ui->lineEditVersion->text().toStdString();
	j[ID::projectAuthor] = ui->lineEditAuthor->text().toStdString();
	j[ID::projectWebsite] = ui->lineEditWebsite->text().toStdString();
	j[ID::projectFontDefault] = defaultFontIndex;
	j[ID::entrypointEntity] = ui->actionSelect->getValue();
	j[ID::quickVerb] = ui->comboVerb->currentText().toStdString();
	j[ID::startingInventory] = jobjects;

	j[ID::scriptBeforeSave] = ui->scriptBeforeSaveEdit->toPlainText().toStdString();
	j[ID::scriptAfterLoad] = ui->scriptAfterLoadEdit->toPlainText().toStdString();
	j[ID::scriptAfterAction] = ui->scriptAfterActionEdit->toPlainText().toStdString();
	j[ID::scriptBeforeAction] = ui->scriptBeforeActionEdit->toPlainText().toStdString();
	j[ID::scriptUndefinedAction] = ui->scriptUndefinedActionEdit->toPlainText().toStdString();
	j[ID::scriptBeforeLeave] = ui->scriptBeforeLeaveEdit->toPlainText().toStdString();
	j[ID::scriptAfterLeave] = ui->scriptAfterLeaveEdit->toPlainText().toStdString();
	j[ID::scriptBeforeEnter] = ui->scriptBeforeEnterEdit->toPlainText().toStdString();
	j[ID::scriptAfterEnter] = ui->scriptAfterEnterEdit->toPlainText().toStdString();
}

void ProjectSettingsWidget::loadData()
{
	auto &j = ProjData;
	ui->lineEditName->setText(QString::fromStdString(j[ID::projectName].ToString()));
	ui->lineEditVersion->setText(QString::fromStdString(j[ID::projectVersion].ToString()));
	ui->lineEditAuthor->setText(QString::fromStdString(j[ID::projectAuthor].ToString()));
	ui->lineEditWebsite->setText(QString::fromStdString(j[ID::projectWebsite].ToString()));

	auto entryPoint = j[ID::entrypointEntity];
	ui->actionSelect->setValue(entryPoint);

	auto quickVerbId = j[ID::quickVerb].ToString();
	refreshVerbs();
	ui->comboVerb->setCurrentText(QString::fromStdString(quickVerbId));

	auto jobjects = j[ID::startingInventory];
	ui->listInventory->clear();
	for (auto &jObjectId : jobjects.ArrayRange())
		ui->listInventory->addItem(QString::fromStdString(jObjectId.ToString()));

	ui->scriptBeforeSaveEdit->setPlainText(QString::fromStdString(j[ID::scriptBeforeSave].ToString()));
	ui->scriptAfterLoadEdit->setPlainText(QString::fromStdString(j[ID::scriptAfterLoad].ToString()));
	ui->scriptAfterActionEdit->setPlainText(QString::fromStdString(j[ID::scriptAfterAction].ToString()));
	ui->scriptBeforeActionEdit->setPlainText(QString::fromStdString(j[ID::scriptBeforeAction].ToString()));
	ui->scriptUndefinedActionEdit->setPlainText(QString::fromStdString(j[ID::scriptUndefinedAction].ToString()));
	ui->scriptAfterLeaveEdit->setPlainText(QString::fromStdString(j[ID::scriptAfterLeave].ToString()));
	ui->scriptBeforeLeaveEdit->setPlainText(QString::fromStdString(j[ID::scriptBeforeLeave].ToString()));
	ui->scriptAfterEnterEdit->setPlainText(QString::fromStdString(j[ID::scriptAfterEnter].ToString()));
	ui->scriptBeforeEnterEdit->setPlainText(QString::fromStdString(j[ID::scriptBeforeEnter].ToString()));

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

	makeFontDefault(j[ID::projectFontDefault].ToInt());
}

void ProjectSettingsWidget::refreshVerbs()
{
	auto model = qobject_cast<TreeModel*>(MainWindow::instance().getItemModel());
	auto verbModelIndex = model->index(EditorTabWidget::Verb);
	auto verbText = ui->comboVerb->currentText();
	ui->comboVerb->clear();
	fillVerbs(model, verbModelIndex);
	ui->comboVerb->setCurrentIndex(ui->comboVerb->findText(verbText));
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

void ProjectSettingsWidget::on_actionAddObject_triggered()
{
	QWizard wizard;
	auto page = new WizardPageActionSelect;

	page->setFilterRegExp("Objects");
	page->allowCustomScript(false);

	wizard.addPage(page);

	if (wizard.exec() == QDialog::Accepted)
	{
		auto jval = page->getValue();
		auto idName = QString::fromStdString(jval[NovelTea::ID::selectEntityId].ToString());
		auto type = static_cast<NovelTea::EntityType>(jval[NovelTea::ID::selectEntityType].ToInt());
		if (type == NovelTea::EntityType::Object)
		{
			// Check if object already exists
			for (int i = 0; i < ui->listInventory->count(); ++i)
			{
				auto item = ui->listInventory->item(i);
				if (item->text() == idName)
					return;
			}

			auto item = new QListWidgetItem(idName);
			ui->listInventory->addItem(item);
		}
	}
}

void ProjectSettingsWidget::on_actionRemoveObject_triggered()
{
	delete ui->listInventory->currentItem();
}

void ProjectSettingsWidget::on_listInventory_currentRowChanged(int currentRow)
{
	ui->actionRemoveObject->setEnabled(currentRow >= 0);
}

void ProjectSettingsWidget::fillVerbs(const TreeModel *model, const QModelIndex &index)
{
	for (auto i = 0; i < model->rowCount(index); ++i)
		fillVerbs(model, model->index(i, 0, index));
	if (index.parent().isValid())
		ui->comboVerb->addItem(index.data().toString());
}
