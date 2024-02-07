#include "ProjectSettingsWidget.hpp"
#include "MainWindow.hpp"
#include "EditorUtils.hpp"
#include "ui_ProjectSettingsWidget.h"
#include "Wizard/WizardPageActionSelect.hpp"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/States/StateEditor.hpp>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QImageReader>
#include <QStandardPaths>
#include <QDir>
#include <QRawFont>
#include <QDebug>

using namespace NovelTea;

namespace {
enum UserData {
	FontAlias = Qt::UserRole,
	FontFileName = Qt::UserRole + 1,
	BuiltIn,
};
}

ProjectSettingsWidget::ProjectSettingsWidget(QWidget *parent)
: EditorTabWidget(parent)
, ui(new Ui::ProjectSettingsWidget)
, m_defaultFontAlias("sys")
{
	ui->setupUi(this);
	ui->comboVerb->lineEdit()->setPlaceholderText("[ Select Verb ]");
	load();

	// Set default font preview
	ui->preview->setFPS(20);
	ui->preview->setMode(NovelTea::StateEditorMode::Text);
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

bool ProjectSettingsWidget::addFontFromFile(bool systemFont, const QString &fileName, const QString &alias)
{
	QFileInfo fileInfo(fileName);
	auto data = EditorUtils::getFileContents(fileName);
	return addFontFromData(systemFont, fileInfo.fileName(), data, alias);
}

bool ProjectSettingsWidget::addFontFromData(bool systemFont, const QString &name, const std::string &data, const QString &alias)
{
	sf::Font testFont;
	if (!testFont.loadFromMemory(data.data(), data.size())) {
		QMessageBox::warning(this, "Failed to load font", QString("Failed to load font: %1 (%2)").arg(alias).arg(name));
		return false;
	}
	auto fontItem = new QListWidgetItem(name);
	auto fontAlias = alias.isEmpty() ? name : alias;
	m_fontsData[fontAlias.toStdString()] = data;
	fontItem->setData(UserData::FontAlias, fontAlias);
	fontItem->setData(UserData::FontFileName, name);
	fontItem->setData(UserData::BuiltIn, systemFont);
	ui->listFonts->addItem(fontItem);
	return true;
}

void ProjectSettingsWidget::refreshFontList()
{
	for (int i = 0; i < ui->listFonts->count(); ++i) {
		auto item = ui->listFonts->item(i);
		auto alias = item->data(UserData::FontAlias).toString();
		auto fileName = item->data(UserData::FontFileName).toString();
		QString builtIn = item->data(UserData::BuiltIn).toBool() ? "[builtin] " : "";
		item->setText(QString("%1 %2(%3)").arg(alias).arg(builtIn).arg(fileName));

		auto itemFont = item->font();
		itemFont.setBold(alias == m_defaultFontAlias);
		item->setFont(itemFont);
	}
}

bool ProjectSettingsWidget::loadImageData(const std::string &data)
{
	QImage image;
	if (!image.loadFromData(reinterpret_cast<const uchar*>(data.data()), data.size()))
		return false;
	auto pixmap = QPixmap::fromImage(image);
	ui->imageLabel->setPixmap(pixmap.scaledToHeight(ui->imageLabel->height()));
	ui->imageLabel->adjustSize();

	m_imageData = data;
	return true;
}

bool ProjectSettingsWidget::loadImageFile(const QString &fileName)
{
	QImage image(fileName);
	if (image.isNull()) {
		QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
								 tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
		return false;
	}

	setModified();
	return loadImageData(EditorUtils::getFileContents(fileName));
}

void ProjectSettingsWidget::saveData() const
{
	auto &j = ProjData;
	auto jobjects = sj::Array();
	for (int i = 0; i < ui->listInventory->count(); ++i) {
		auto item = ui->listInventory->item(i);
		jobjects.append(item->text().toStdString());
	}
	j[ID::startingInventory] = jobjects;

	auto jfonts = sj::Object();
	for (int i = 0; i < ui->listFonts->count(); ++i) {
		auto item = ui->listFonts->item(i);
		if (!item->data(UserData::BuiltIn).toBool()) {
			auto fileName = item->data(UserData::FontFileName).toString().toStdString();
			auto alias = item->data(UserData::FontAlias).toString().toStdString();
			jfonts[alias] = fileName;
		}
	}
	for (auto &data : m_fontsData)
		Proj->setFontData(data.first, data.second);
	j[ID::projectFonts] = jfonts;

	Proj->setImageData(m_imageData);

	j[ID::projectName] = ui->lineEditName->text().toStdString();
	j[ID::projectVersion] = ui->lineEditVersion->text().toStdString();
	j[ID::projectAuthor] = ui->lineEditAuthor->text().toStdString();
	j[ID::projectWebsite] = ui->lineEditWebsite->text().toStdString();
	j[ID::projectFontDefault] = m_defaultFontAlias.toStdString();
	j[ID::entrypointEntity] = ui->actionSelect->getValue();
	j[ID::quickVerb] = ui->comboVerb->currentText().toStdString();

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

	loadImageData(Proj->getImageData());

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
	for (auto& jfont : j[ID::engineFonts].ObjectRange()) {
		auto fileName = QString::fromStdString(jfont.second.ToString());
		addFontFromFile(true, "/home/android/dev/NovelTea/res/assets/fonts/" + fileName, QString::fromStdString(jfont.first));
	}
	for (auto& jfont : j[ID::projectFonts].ObjectRange()) {
		auto &data = Proj->getFontData(jfont.first);
		addFontFromData(false, QString::fromStdString(jfont.second.ToString()), data, QString::fromStdString(jfont.first));
	}
	m_defaultFontAlias = QString::fromStdString(j[ID::projectFontDefault].ToString());
	refreshFontList();
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
	ui->preview->events()->trigger({NovelTea::StateEditor::PreviewTextChanged, arg1.toStdString()});
}

void ProjectSettingsWidget::on_listFonts_currentRowChanged(int currentRow)
{
	auto alias = ui->listFonts->currentItem()->data(UserData::FontAlias).toString();
	auto builtIn = ui->listFonts->currentItem()->data(UserData::BuiltIn).toBool();
	ui->buttonSetDefaultFont->setEnabled(alias != m_defaultFontAlias);
	ui->buttonFontRename->setEnabled(!builtIn);
	ui->buttonFontDelete->setEnabled(!builtIn);

	ui->preview->events()->trigger({NovelTea::StateEditor::PreviewFontChanged, alias.toStdString()});
}

void ProjectSettingsWidget::on_buttonImportFont_clicked()
{
	// TODO: Add all supported types
	QStringList mimeTypeFilters {"application/x-font-ttf"};
	const QStringList fontLocations = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
	QFileDialog dialog(this, tr("Open File"),
					   fontLocations.isEmpty() ? QDir::currentPath() : fontLocations.first());
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setMimeTypeFilters(mimeTypeFilters);

	if (dialog.exec() == QDialog::Accepted) {
		for (auto& file : dialog.selectedFiles()) {
			QFileInfo fileInfo(file);
			QString alias;
			bool ok;
			while (alias.isEmpty() || m_fontsData.find(alias.toStdString()) != m_fontsData.end()) {
				alias = QInputDialog::getText(this, tr("Make Font Alias"),
						tr("Provide an unused alias for file: %1").arg(file), QLineEdit::Normal,
						fileInfo.fileName(), &ok);
				if (!ok)
					break;
			}
			if (ok) {
				addFontFromFile(false, file, alias);
				setModified();
			}
		}

		refreshFontList();
	}
}

void ProjectSettingsWidget::on_buttonSetDefaultFont_clicked()
{
	m_defaultFontAlias = ui->listFonts->currentItem()->data(UserData::FontAlias).toString();
	refreshFontList();
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

void ProjectSettingsWidget::on_buttonSelectImage_clicked()
{
	QStringList filters;
	filters << "Image files (*.jpg *jpeg *.png *.bmp *.gif *.tga)";
	filters << "All Files (*)";
	const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
	QFileDialog dialog(this, tr("Open File"),
					   picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first());
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilters(filters);

	while (dialog.exec() == QDialog::Accepted && !loadImageFile(dialog.selectedFiles().first())) {}
}

void ProjectSettingsWidget::on_buttonFontRename_clicked()
{
	auto item = ui->listFonts->currentItem();
	if (item->data(UserData::BuiltIn).toBool())
		return;

	bool ok;
	auto oldAlias = item->data(UserData::FontAlias).toString();
	QString text = QInputDialog::getText(this, tr("Rename Font Alias"),
			tr("Please enter a new alias:"), QLineEdit::Normal,
			oldAlias, &ok);
	if (ok && !text.isEmpty()) {
		if (text.startsWith("sys")) {
			QMessageBox::warning(this, "Invalid Alias", "User-defined aliases cannot start with 'sys' because those values are reserved.");
			return;
		}
		auto alias = text.toStdString();
		if (m_fontsData.find(alias) != m_fontsData.end()) {
			QMessageBox::warning(this, "Invalid Alias", "Alias already used.");
			return;
		}
		item->setData(UserData::FontAlias, text);
		m_fontsData[alias] = m_fontsData[oldAlias.toStdString()];
		m_fontsData.erase(oldAlias.toStdString());
		refreshFontList();
		setModified();
	}
}

void ProjectSettingsWidget::on_buttonFontDelete_clicked()
{
	auto item = ui->listFonts->currentItem();
	auto alias = item->data(UserData::FontAlias).toString();
	m_fontsData.erase(alias.toStdString());
	delete item;

	if (m_defaultFontAlias == alias)
		m_defaultFontAlias = "sys";
	refreshFontList();
	setModified();
}
