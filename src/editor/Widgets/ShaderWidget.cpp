#include "ShaderWidget.hpp"
#include "ui_ShaderWidget.h"
#include "EditorUtils.hpp"
#include <NovelTea/Event.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/Script.hpp>
#include <NovelTea/RegexUtils.hpp>
#include <NovelTea/States/StateEditor.hpp>
#include <NovelTea/SFML/AssetLoaderSFML.hpp>
#include <QMessageBox>
#include <QBuffer>
#include <QInputDialog>
#include <QFileDialog>
#include <QImageReader>
#include <QStandardPaths>
#include <QDebug>
#include "QtPropertyBrowser/qtpropertymanager.h"
#include "QtPropertyBrowser/qtvariantproperty.h"
#include "QtPropertyBrowser/qttreepropertybrowser.h"

using namespace NovelTea;

ShaderWidget::ShaderWidget(QWidget *parent)
: EditorTabWidget(parent)
, ui(new Ui::ShaderWidget)
, m_variantManager(new QtVariantPropertyManager)
, m_variantFactory(new QtVariantEditorFactory)
, m_shaderChanged(true)
{
	ui->setupUi(this);

	m_systemShaderMap = {
		{ui->comboShaderBackground, ID::shaderBackground},
		{ui->comboShaderPostProcess, ID::shaderPostProcess},
	};

	ui->preview->setMode(NovelTea::StateEditorMode::Shader);
	ui->preview->setFPS(30.f);

	load();

	ui->propertyBrowser->setFactoryForManager(m_variantManager, m_variantFactory);
	ui->propertyBrowser->setPropertiesWithoutValueMarked(true);
	ui->propertyBrowser->setRootIsDecorated(false);

	connect(m_variantManager, &QtVariantPropertyManager::valueChanged, this, &ShaderWidget::propertyChanged);

	startTimer(1000);
}

ShaderWidget::~ShaderWidget()
{
	delete m_variantFactory;
	delete m_variantManager;
	delete ui;
}

QString ShaderWidget::tabText() const
{
	return "Shaders";
}

EditorTabWidget::Type ShaderWidget::getType() const
{
	return EditorTabWidget::Shaders;
}

void ShaderWidget::loadShaderId(const std::string &shaderId)
{
	if (shaderId == m_currentShaderId)
		return;
	if (!m_shaders.hasKey(shaderId)) {
		// Add empty shader if the ID doesn't exist yet
		m_shaders[shaderId] = sj::Array("", sj::Object());
	}

	saveCurrentShaderId();
	m_currentShaderId = shaderId;

	ui->scriptEdit->blockSignals(true);
	m_variantManager->blockSignals(true);

	auto items = ui->listWidget->findItems(QString::fromStdString(shaderId), Qt::MatchExactly);
	if (!items.isEmpty())
		ui->listWidget->setCurrentItem(items[0]);
	ui->scriptEdit->setPlainText(QString::fromStdString(m_shaders[shaderId][0].ToString()));

	updatePropertyList(&m_shaders[shaderId][1]);
	if (updateErrorLog())
		updatePreview();

	ui->scriptEdit->blockSignals(false);
	m_variantManager->blockSignals(false);
}

void ShaderWidget::timerEvent(QTimerEvent *event)
{
	if (!m_shaderChanged)
		return;
	updatePropertyList();
	if (updateErrorLog())
		updatePreview();
	m_shaderChanged = false;
}

void ShaderWidget::saveData() const
{
	saveCurrentShaderId();
	for (auto& p : m_systemShaderMap)
		ProjData[ID::systemShaders][p.second] = p.first->currentText().toStdString();

	auto jtextures = sj::Object();
	for (auto& texture : m_texturesData) {
		jtextures[texture.first] = sj::Array();
		Proj->setTextureData(texture.first, texture.second);
	}
	ProjData[ID::textures] = jtextures;
	ProjData[ID::shaders] = m_shaders;
}

void ShaderWidget::loadData()
{
	m_shaders = ProjData[ID::shaders];
	for (auto jitem : m_shaders.ObjectRange())
		ui->listWidget->addItem(QString::fromStdString(jitem.first));

	auto& sysShaders = ProjData[ID::systemShaders];
	for (auto& p : m_systemShaderMap) {
		p.first->setModel(ui->listWidget->model());
		p.first->setCurrentText(QString::fromStdString(sysShaders[p.second].ToString()));
		MODIFIER(p.first, &QComboBox::currentTextChanged);
	}

	for (auto& jtexture : ProjData[ID::textures].ObjectRange()) {
		auto &data = Proj->getTextureData(jtexture.first);
		addTextureToList(data, jtexture.first);
	}

	MODIFIER(ui->scriptEdit, &ScriptEdit::textChanged);
	MODIFIER(m_variantManager, &QtVariantPropertyManager::valueChanged);
	MODIFIER(ui->listWidget->model(), &QAbstractItemModel::rowsInserted);
	MODIFIER(ui->listWidget->model(), &QAbstractItemModel::rowsRemoved);
	MODIFIER(ui->listTextures->model(), &QAbstractItemModel::rowsInserted);
	MODIFIER(ui->listTextures->model(), &QAbstractItemModel::rowsRemoved);
}

void ShaderWidget::on_actionAddShader_triggered()
{
	auto name = QInputDialog::getText(this, tr("New Shader"),
			tr("Enter name for new shader:"));
	if (!name.isEmpty() && !m_shaders.hasKey(name.toStdString()))
	{
		ui->listWidget->addItem(name);
		loadShaderId(name.toStdString());
	}
}

void ShaderWidget::on_actionDeleteShader_triggered()
{
	auto shaderId = ui->listWidget->currentItem()->text().toStdString();
	auto msg = "Are you sure you want to remove shader \"" + shaderId + "\"?";
	auto ret = QMessageBox::warning(this, "Delete Shader", QString::fromStdString(msg), QMessageBox::Cancel | QMessageBox::Ok, QMessageBox::Cancel);
	if (ret == QMessageBox::Cancel)
		return;

	m_currentShaderId.clear();
	m_shaders.erase(shaderId);
	delete ui->listWidget->currentItem();
}

void ShaderWidget::on_listWidget_currentRowChanged(int currentRow)
{
	auto item = ui->listWidget->currentItem();
	ui->actionDeleteShader->setEnabled(item);
	if (!item) {
		return;
	}
	auto shaderId = item->text().toStdString();
	if (shaderId == "defaultFrag" || shaderId == "defaultVert")
		ui->actionDeleteShader->setEnabled(false);
	loadShaderId(shaderId);
}

void ShaderWidget::on_tabWidget_currentChanged(int index)
{
	if (index == 0 && !m_currentShaderId.empty()) {
		// In case textures changed, reload property browser
		m_variantManager->blockSignals(true);
		updatePropertyList(&m_shaders[m_currentShaderId][1]);
		m_variantManager->blockSignals(false);
	}
}

void ShaderWidget::saveCurrentShaderId() const
{
	if (m_currentShaderId.empty())
		return;
	auto script = ui->scriptEdit->toPlainText().toStdString();
	m_shaders[m_currentShaderId] = sj::Array(script, getUniforms());
}

sj::JSON ShaderWidget::getUniforms() const
{
	auto uniforms = sj::Object();
	for (auto &prop : ui->propertyBrowser->properties())
	{
		auto name = prop->propertyName().toStdString();
		auto type = m_variantManager->valueType(prop);
		if (type == QVariant::Double)
			uniforms[name] = m_variantManager->value(prop).toDouble();
		else {
			auto i = m_variantManager->value(prop).toInt();
			for (auto& texture : m_texturesData)
				if (i-- == 0) {
					uniforms[name] = texture.first;
					break;
				}
		}
	}
	return uniforms;
}

void ShaderWidget::propertyChanged(QtProperty *property, const QVariant &value)
{
	updatePreview();
}

void ShaderWidget::on_actionPreview_toggled(bool checked)
{
	if (checked)
		ui->preview->show();
	else
		ui->preview->hide();
}

void ShaderWidget::on_actionUniforms_toggled(bool checked)
{
	if (checked)
		ui->propertyBrowser->show();
	else
		ui->propertyBrowser->hide();
}

void ShaderWidget::on_actionCompileOutput_toggled(bool checked)
{
	if (checked)
		ui->textErrorLog->show();
	else
		ui->textErrorLog->hide();
}

void ShaderWidget::updatePropertyList(const sj::JSON *uniformArray)
{
	auto uniforms = (uniformArray) ? *uniformArray : getUniforms();

	ui->propertyBrowser->clear();

	auto re = std::regex {R"(uniform\s+(float|sampler2D)\s+(\S+)\s*;)"};
	auto script = ui->scriptEdit->toPlainText().toStdString();
	processRegex(script, re, [this, &uniforms](const RegexIterator& it) {
		auto type = it->str(1);
		auto name = it->str(2);
		// Ignore all engine-provided uniforms
		const auto builtIns = std::vector<std::string>{
			"time", "time_delta", "texture"
		};
		if (std::find(builtIns.begin(), builtIns.end(), name) != builtIns.end())
			return;
		QtVariantProperty *prop;
		if (type == "float") {
			prop = m_variantManager->addProperty(QVariant::Double, QString::fromStdString(name));
			prop->setValue(uniforms.hasKey(name) ? uniforms[name].ToFloat() : 0.f);
			prop->setAttribute("decimals", 4);
		} else { // sampler2D
			QString textureName = uniforms.hasKey(name) ? QString::fromStdString(uniforms[name].ToString()) : "";
			QStringList textureNames;
			for (auto& texture : m_texturesData)
				textureNames << QString::fromStdString(texture.first);
			prop = m_variantManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString::fromStdString(name));
			prop->setAttribute("enumNames", textureNames);
			prop->setValue(textureNames.indexOf(textureName));
		}
		ui->propertyBrowser->addProperty(prop);
	});
}

void ShaderWidget::updatePreview()
{
	Asset->loadUniforms(&m_shader, getUniforms());
	ui->preview->events()->trigger({NovelTea::StateEditor::EntityChanged, &m_shader});
}

void ShaderWidget::on_scriptEdit_textChanged()
{
	m_shaderChanged = true;
}

bool ShaderWidget::isVertexShader(const std::string &script)
{
	auto re = std::regex {R"(attribute\s+vec2\s+position\s*;)"};
	return 	std::regex_search(script.begin(), script.end(), re);
}

bool ShaderWidget::updateErrorLog()
{
	auto script = ui->scriptEdit->toPlainText().toStdString();
	auto isVertex = isVertexShader(script);
	auto fragShader = isVertex ? m_shaders["defaultFrag"][0].ToString() : script;
	auto vertShader = isVertex ? script : m_shaders["defaultVert"][0].ToString();
	if (m_shader.loadFromMemory(vertShader, fragShader))
	{
		m_shader.setUniform("texture", sf::Shader::CurrentTexture);
		ui->textErrorLog->setPlainText("Compiled Successfully");
		return true;
	} else {
		auto errorLog = m_shader.getErrorLog();
		ui->textErrorLog->setPlainText(QString::fromStdString(errorLog));
		return false;
	}
}

void ShaderWidget::on_actionAddTexture_triggered()
{
	QStringList filters;
	filters << "Image files (*.jpg *jpeg *.png *.bmp *.gif *.tga)";
	filters << "All Files (*)";
	const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
	QFileDialog dialog(this, tr("Select Texture Images"),
					   picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first());
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setNameFilters(filters);

	if (dialog.exec() != QDialog::Accepted)
		return;

	for (auto& fileName : dialog.selectedFiles()) {
		QFileInfo fileInfo(fileName);
		auto data = EditorUtils::getFileContents(fileName);
		if (!addTextureToList(data, fileInfo.fileName().toStdString()))
			QMessageBox::warning(this, tr("Failed to Add Texture"), tr("Image file '%1' cannot be loaded. It is either corrupted or not a supported image format.").arg(fileName));
	}
}

void ShaderWidget::on_actionRemoveTexture_triggered()
{
	auto item = ui->listTextures->currentItem();
	auto textureId = item->text();
	auto msg = "Are you sure you want to remove texture \"" + textureId + "\"?";
	auto ret = QMessageBox::warning(this, "Delete Texture", msg, QMessageBox::Cancel | QMessageBox::Ok, QMessageBox::Cancel);
	if (ret == QMessageBox::Cancel)
		return;
	m_texturesData.erase(textureId.toStdString());
	delete item;
}

std::string ShaderWidget::makeTextureName(const std::string &textureName)
{
	if (textureName.empty())
		return makeTextureName("texture");
	auto i = 0;
	auto name = textureName;
	while (m_texturesData.find(name) != m_texturesData.end())
		name = textureName + std::to_string(i++);
	// If name changed above, it is presumed valid since only digits added
	if (name != textureName)
		return name;

	static const char allowedChars[] = "_.-";
	auto valid = true;
	// Cannot start with a digit
	if (std::isdigit(name[0])) {
		name = "t" + name;
		valid = false;
	}
	// Replace all invalid chars
	for (auto& c : name)
		if (!std::isalnum(c) && std::strchr(allowedChars, c) == nullptr) {
			c = '_';
			valid = false;
		}
	if (!valid)
		return makeTextureName(name);
	return name;
}

bool ShaderWidget::addTextureToList(const std::string &textureData, const std::string &textureName)
{
	auto name = makeTextureName(textureName);
	auto data = textureData;

	QImage image;
	const int maxSize = 1024;
	if (!image.loadFromData(reinterpret_cast<const uchar*>(data.data()), data.size()))
		return false;
	auto h = image.size().height();
	auto w = image.size().width();
	if (w > maxSize || h > maxSize) {
		QMessageBox::warning(this, tr("Image Resized"), tr("For compatibility, image has been resized to fit the maximum dimension of 1024 pixels. However, it is recomended that you resize the image yourself and try again."));
		if (w > h)
			image = image.scaledToWidth(maxSize);
		else
			image = image.scaledToHeight(maxSize);
		QByteArray ba;
		QBuffer buffer(&ba);
		buffer.open(QIODevice::WriteOnly);
		image.save(&buffer, "PNG", 0);
		data.assign(ba.data(), ba.size());
	}

	auto pixmap = QPixmap::fromImage(image);
	QIcon icon(pixmap);

	auto item = new QListWidgetItem;
	item->setIcon(icon);
	item->setText(QString::fromStdString(name));
	item->setTextAlignment(Qt::AlignHCenter);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

	ui->listTextures->addItem(item);

	m_texturesData[name] = data;
	return true;
}

void ShaderWidget::on_listTextures_itemChanged(QListWidgetItem *item)
{
	auto oldName = m_lastSelectedTextureName.toStdString();
	auto newName = item->text().toStdString();
	if (oldName.empty() || oldName == newName)
		return;

	auto name = makeTextureName(newName);
	if (name != newName) {
		item->setText(QString::fromStdString(name));
	}
	else {
		// Update uniforms that use the old texture name
		for (auto& shader : m_shaders.ObjectRange()) {
			for (auto& uniform : shader.second[1].ObjectRange()) {
				bool ok;
				auto name = uniform.second.ToString(ok);
				if (ok && name == oldName)
					uniform.second = newName;
			}
		}

		m_texturesData[newName] = m_texturesData[oldName];
		m_texturesData.erase(oldName);
		m_lastSelectedTextureName = QString::fromStdString(newName);
		setModified();
	}
}

void ShaderWidget::on_listTextures_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	ui->actionRemoveTexture->setEnabled(current);
	if (current)
		m_lastSelectedTextureName = current->text();
}
