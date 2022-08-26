#include "ShaderWidget.hpp"
#include "ui_ShaderWidget.h"
#include <NovelTea/Game.hpp>
#include <NovelTea/Script.hpp>
#include <NovelTea/RegexUtils.hpp>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include "QtPropertyBrowser/qtpropertymanager.h"
#include "QtPropertyBrowser/qtvariantproperty.h"
#include "QtPropertyBrowser/qttreepropertybrowser.h"

ShaderWidget::ShaderWidget(QWidget *parent)
: EditorTabWidget(parent)
, ui(new Ui::ShaderWidget)
, m_variantManager(new QtVariantPropertyManager)
, m_variantFactory(new QtVariantEditorFactory)
, m_shaderChanged(true)
{
	ui->setupUi(this);
	load();

	ui->preview->setFPS(30.f);

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
	updateErrorLog();

	ui->scriptEdit->blockSignals(false);
	m_variantManager->blockSignals(false);
}

void ShaderWidget::timerEvent(QTimerEvent *event)
{
	if (!m_shaderChanged)
		return;
	updatePropertyList();
	updateErrorLog();
	updatePreview();
	m_shaderChanged = false;
}

void ShaderWidget::saveData() const
{
	saveCurrentShaderId();
	ProjData[NovelTea::ID::shaders] = m_shaders;
}

void ShaderWidget::loadData()
{
	m_shaders = ProjData[NovelTea::ID::shaders];
	for (auto jitem : m_shaders.ObjectRange())
		ui->listWidget->addItem(QString::fromStdString(jitem.first));

	MODIFIER(ui->scriptEdit, &ScriptEdit::textChanged);
	MODIFIER(m_variantManager, &QtVariantPropertyManager::valueChanged);
	MODIFIER(ui->listWidget->model(), &QAbstractItemModel::rowsInserted);
	MODIFIER(ui->listWidget->model(), &QAbstractItemModel::rowsRemoved);
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
	auto msg = "Are you sure you want to remove shader \"" + shaderId + "\"";
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
		auto value = m_variantManager->value(prop).toDouble();
		uniforms[name] = value;
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

	auto re = std::regex {R"(uniform\s+float\s+(\S+)\s*;)"};
	auto script = ui->scriptEdit->toPlainText().toStdString();
	NovelTea::processRegex(script, re, [this, &uniforms](const NovelTea::RegexIterator& it){
		auto name = it->str(1);
		// Ignore all engine-provided uniforms
		const auto builtIns = std::vector<std::string>{
				"time", "time_delta", "screen_w", "screen_h"};
		if (std::find(builtIns.begin(), builtIns.end(), name) != builtIns.end())
			return;
		auto prop = m_variantManager->addProperty(QVariant::Double, QString::fromStdString(name));
		prop->setValue(uniforms.hasKey(name) ? uniforms[name].ToFloat() : 0.f); ui->propertyBrowser->addProperty(prop);
	});
}

void ShaderWidget::updatePreview()
{

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

void ShaderWidget::updateErrorLog()
{
	sf::Shader shader;
	auto script = ui->scriptEdit->toPlainText().toStdString();
	if (shader.loadFromMemory(script, isVertexShader(script) ? sf::Shader::Vertex : sf::Shader::Fragment))
	{
		ui->textErrorLog->setPlainText("Compiled Successfully");
	} else {
		auto errorLog = shader.getErrorLog();
		ui->textErrorLog->setPlainText(QString::fromStdString(errorLog));
	}
}
