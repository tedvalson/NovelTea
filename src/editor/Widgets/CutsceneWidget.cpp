#include "CutsceneWidget.hpp"
#include "ui_CutsceneWidget.h"
#include "EditorUtils.hpp"
#include <NovelTea/Engine.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/CutscenePageBreakSegment.hpp>
#include <NovelTea/CutscenePageSegment.hpp>
#include <NovelTea/States/StateEditor.hpp>
#include <QToolButton>
#include <QDebug>

Q_DECLARE_METATYPE(std::shared_ptr<NovelTea::ActiveText>)

namespace
{
	const auto propText           = "Text";
	const auto propBeginNewLine   = "Begin New Line";
	const auto propTextEffect     = "Text Effect";
	const auto propBreakEffect    = "Break Effect";
	const auto propDuration       = "Effect Duration";
	const auto propDelay          = "Delay";
	const auto propOffset         = "Offset";
	const auto propWaitForClick   = "Wait For Click";
	const auto propCanSkip        = "Can Skip";

	const auto propTextDelay      = "Text Delay";
	const auto propTextDuration   = "Text Duration";
	const auto propTextDelimiter  = "Text Delimiter";
	const auto propBreakDelay     = "Break Delay";
	const auto propBreakDuration  = "Break Duration";
	const auto propBreakDelimiter = "Break Delimiter";

	const auto propFullScreen     = "Full Screen";
	const auto propCanFastForward = "Can Fast-Foward";
	const auto propSpeedFactor    = "Speed Factor";
}

CutsceneWidget::CutsceneWidget(const std::string &idName, QWidget *parent) :
	EditorTabWidget(parent),
	ui(new Ui::CutsceneWidget),
	menuAdd(nullptr),
	selectedIndex(-1),
	segmentsVariantManager(new QtVariantPropertyManager),
	settingsVariantManager(new QtVariantPropertyManager),
	variantFactory(new QtVariantEditorFactory),
	m_cutscenePlaying(false)
{
	m_idName = idName;
	ui->setupUi(this);
	ui->preview->setMode(NovelTea::StateEditorMode::Cutscene);
	createMenus();
	load();

	ui->richTextEditor->hide();
	auto showBrowser = [this]() {
		ui->richTextEditor->hide();
		ui->propertyBrowser->show();
	};
	connect(ui->richTextEditor, &RichTextEditor::invoked, [this](){
		ui->richTextEditor->show();
		ui->propertyBrowser->hide();
	});
	connect(ui->richTextEditor, &RichTextEditor::saved, showBrowser);
	connect(ui->richTextEditor, &RichTextEditor::canceled, showBrowser);

	connect(ui->listWidget->model(), &QAbstractItemModel::rowsMoved, this, &CutsceneWidget::on_rowsMoved);
	connect(segmentsVariantManager, &QtVariantPropertyManager::valueChanged, this, &CutsceneWidget::segmentPropertyChanged);
	connect(settingsVariantManager, &QtVariantPropertyManager::valueChanged, this, &CutsceneWidget::settingPropertyChanged);

	connect(ui->actionSelect, &ActionSelectWidget::valueChanged, [this](){
		setModified();
		m_cutscene->setNextEntity(ui->actionSelect->getValue());
	});
}

CutsceneWidget::~CutsceneWidget()
{
	delete variantFactory;
	delete settingsVariantManager;
	delete segmentsVariantManager;
	delete menuAdd;
	delete ui;
}

QString CutsceneWidget::tabText() const
{
	return QString::fromStdString(idName());
}

EditorTabWidget::Type CutsceneWidget::getType() const
{
	return EditorTabWidget::Cutscene;
}

void CutsceneWidget::createMenus()
{
	if (menuAdd)
		return;

	menuAdd = new QMenu();
	menuAdd->addAction(ui->actionAddText);
	menuAdd->addAction(ui->actionAddPage);
	menuAdd->addAction(ui->actionAddPageBreak);

	// Attach the menu to the Add toolbutton
	ui->actionAddSegment->setMenu(menuAdd);
	auto buttonAdd = static_cast<QToolButton*>(ui->toolBarEntities->widgetForAction(ui->actionAddSegment));
	buttonAdd->setPopupMode(QToolButton::InstantPopup);
}

#define PROP_TIME_MS(name, value) \
	prop = segmentsVariantManager->addProperty(QVariant::Int, name); \
	prop->setValue(static_cast<unsigned int>(value)); \
	prop->setAttribute(QLatin1String("minimum"), 0); \
	prop->setAttribute(QLatin1String("maximum"), 100000); \
	prop->setAttribute(QLatin1String("singleStep"), 100); \
	ui->propertyBrowser->addProperty(prop);

#define PROP_TEXT_EFFECT(value) { \
	prop = segmentsVariantManager->addProperty(QtVariantPropertyManager::enumTypeId(), propTextEffect); \
	QStringList enumNames; \
	enumNames << "None" << "Fade" << "FadeAcross"; \
	prop->setAttribute(QLatin1String("enumNames"), enumNames); \
	prop->setValue(value); \
	ui->propertyBrowser->addProperty(prop); \
}

#define PROP_BREAK_EFFECT(value) { \
	prop = segmentsVariantManager->addProperty(QtVariantPropertyManager::enumTypeId(), propBreakEffect); \
	QStringList enumNames; \
	enumNames << "None" << "Fade" << "Scroll Left"; \
	prop->setAttribute(QLatin1String("enumNames"), enumNames); \
	prop->setValue(value); \
	ui->propertyBrowser->addProperty(prop); \
}

void CutsceneWidget::fillPropertyEditor()
{
	ui->propertyBrowser->clear();
	ui->propertyBrowser->show();
	ui->richTextEditor->hide();

	ui->propertyBrowser->setFactoryForManager(segmentsVariantManager, variantFactory);
	ui->propertyBrowser->setPropertiesWithoutValueMarked(true);

	// If no item is selected, there is nothing to load
	if (selectedIndex < 0)
		return;

	segmentsVariantManager->blockSignals(true);

	QtVariantProperty *prop;
	auto segment = m_cutscene->internalSegments()[selectedIndex];
	auto type = segment->type();

	if (type == NovelTea::CutsceneSegment::Text)
	{
		auto textSegment = static_cast<NovelTea::CutsceneTextSegment*>(segment.get());

		ui->richTextEditor->setFormattingEnabled(true);
		prop = segmentsVariantManager->addProperty(QtVariantPropertyManager::richTextTypeId(), propText);
		prop->setAttribute("richTextEditor", QVariant::fromValue(ui->richTextEditor));
		prop->setValue(QVariant::fromValue(textSegment->getActiveText()));
		ui->propertyBrowser->addProperty(prop);

		PROP_TEXT_EFFECT(textSegment->getTransition())

		prop = segmentsVariantManager->addProperty(QVariant::Bool, propBeginNewLine);
		prop->setValue(textSegment->getBeginWithNewLine());
		ui->propertyBrowser->addProperty(prop);

		PROP_TIME_MS(propDuration, segment->getDuration())
		PROP_TIME_MS(propDelay, segment->getDelay())

		prop = segmentsVariantManager->addProperty(QVariant::Point, propOffset);
		prop->setValue(QPoint(textSegment->getOffsetX(), textSegment->getOffsetY()));
		ui->propertyBrowser->addProperty(prop);
	}
	else if (type == NovelTea::CutsceneSegment::PageBreak)
	{
		auto pageBreakSegment = static_cast<NovelTea::CutscenePageBreakSegment*>(segment.get());
		PROP_BREAK_EFFECT(pageBreakSegment->getTransition())
		PROP_TIME_MS(propDuration, segment->getDuration())
		PROP_TIME_MS(propDelay, segment->getDelay())
	}
	else if (type == NovelTea::CutsceneSegment::Page)
	{
		auto pageSegment = static_cast<NovelTea::CutscenePageSegment*>(segment.get());

		auto t = std::make_shared<NovelTea::ActiveText>();
		t->setText(pageSegment->getText());
		ui->richTextEditor->setFormattingEnabled(false);
		prop = segmentsVariantManager->addProperty(QtVariantPropertyManager::richTextTypeId(), propText);
		prop->setAttribute("richTextEditor", QVariant::fromValue(ui->richTextEditor));
		prop->setValue(QVariant::fromValue(t));
		ui->propertyBrowser->addProperty(prop);

		PROP_TEXT_EFFECT(pageSegment->getTextEffect())
		PROP_BREAK_EFFECT(pageSegment->getBreakEffect())

		prop = segmentsVariantManager->addProperty(QVariant::String, propTextDelimiter);
		prop->setValue(EditorUtils::escape(QString::fromStdString(pageSegment->getTextDelimiter())));
		ui->propertyBrowser->addProperty(prop);

		prop = segmentsVariantManager->addProperty(QVariant::String, propBreakDelimiter);
		prop->setValue(EditorUtils::escape(QString::fromStdString(pageSegment->getBreakDelimiter())));
		ui->propertyBrowser->addProperty(prop);

		PROP_TIME_MS(propTextDuration, pageSegment->getTextDuration())
		PROP_TIME_MS(propTextDelay, pageSegment->getTextDelay())
		PROP_TIME_MS(propBreakDuration, pageSegment->getTextDuration())
		PROP_TIME_MS(propBreakDelay, pageSegment->getTextDuration())

		prop = segmentsVariantManager->addProperty(QVariant::Bool, propBeginNewLine);
		prop->setValue(pageSegment->getBeginWithNewLine());
		ui->propertyBrowser->addProperty(prop);

		prop = segmentsVariantManager->addProperty(QVariant::Point, propOffset);
		prop->setValue(QPoint(pageSegment->getOffsetX(), pageSegment->getOffsetY()));
		ui->propertyBrowser->addProperty(prop);
	}

	prop = segmentsVariantManager->addProperty(QVariant::Bool, propWaitForClick);
	prop->setValue(segment->getWaitForClick());
	ui->propertyBrowser->addProperty(prop);

	prop = segmentsVariantManager->addProperty(QVariant::Bool, propCanSkip);
	prop->setValue(segment->getCanSkip());
	ui->propertyBrowser->addProperty(prop);

	segmentsVariantManager->blockSignals(false);
}

void CutsceneWidget::fillSettingsPropertyEditor()
{
	ui->actionSelect->blockSignals(true);
	settingsVariantManager->blockSignals(true);
	ui->propertyBrowserSettings->clear();
	ui->propertyBrowserSettings->setFactoryForManager(settingsVariantManager, variantFactory);
	ui->propertyBrowserSettings->setPropertiesWithoutValueMarked(true);

	QtVariantProperty *prop;

	prop = settingsVariantManager->addProperty(QVariant::Bool, propFullScreen);
	prop->setValue(m_cutscene->getFullScreen());
	ui->propertyBrowserSettings->addProperty(prop);

	prop = settingsVariantManager->addProperty(QVariant::Bool, propCanFastForward);
	prop->setValue(m_cutscene->getCanFastForward());
	ui->propertyBrowserSettings->addProperty(prop);

	prop = settingsVariantManager->addProperty(QVariant::Double, propSpeedFactor);
	prop->setValue(m_cutscene->getSpeedFactor());
	ui->propertyBrowserSettings->addProperty(prop);

	ui->actionSelect->setValue(m_cutscene->getNextEntity());

	ui->actionSelect->blockSignals(false);
	settingsVariantManager->blockSignals(false);
}

void CutsceneWidget::checkIndexChange()
{
	int index = ui->listWidget->currentRow();
	if (selectedIndex != index)
	{
		selectedIndex = index;

		updateLoopValues();

		if (!m_cutscenePlaying && !m_segmentLooping)
			ui->horizontalSlider->setValue(m_loopEndMs);

		fillPropertyEditor();
	}

	ui->actionRemoveSegment->setEnabled(index >= 0);
}

void CutsceneWidget::addItem(std::shared_ptr<NovelTea::CutsceneSegment> segment, bool addToInternalObject, int index)
{
	auto type = segment->type();
	QListWidgetItem *item = nullptr;
	QString text;

	if (type == NovelTea::CutsceneSegment::Text)
	{
		auto seg = static_cast<NovelTea::CutsceneTextSegment*>(segment.get());
		text = QString::fromStdString(seg->getActiveText()->toPlainText(" | "));
		item = new QListWidgetItem(QIcon::fromTheme("format"), text);
	}
	else if (type == NovelTea::CutsceneSegment::Page)
	{
		auto seg = static_cast<NovelTea::CutscenePageSegment*>(segment.get());
		text = QString::fromStdString(seg->getText());
		item = new QListWidgetItem(QIcon::fromTheme("document-new"), text.replace("\n", " | "));
	}
	else if (type == NovelTea::CutsceneSegment::PageBreak)
	{
		item = new QListWidgetItem(QIcon::fromTheme("stop"), "Page Break");
	}

	if (!item)
		return;

	if (index < 0)
	{
		ui->listWidget->addItem(item);
		if (addToInternalObject)
			m_cutscene->addSegment(segment);
	}
	else
	{
		ui->listWidget->insertItem(index, item);
		if (addToInternalObject) {
			m_cutscene->internalSegments().insert(m_cutscene->internalSegments().begin() + index, segment);
			m_cutscene->updateSegments();
		}
	}

	if (addToInternalObject)
		updateCutscene();
}

void CutsceneWidget::saveData() const
{
	if (m_cutscene)
	{
		m_cutscene->setProperties(ui->propertyEditor->getValue());
		Proj.set<NovelTea::Cutscene>(m_cutscene, idName());
	}
}

void CutsceneWidget::loadData()
{
	m_cutscene = Proj.get<NovelTea::Cutscene>(idName());
//	ui->listWidget->model()->disconnect();
	ui->listWidget->clear();

	qDebug() << "Loading cutscene data... " << QString::fromStdString(idName());

	if (m_cutscene)
		for (auto &seg : m_cutscene->internalSegments())
			addItem(seg, false);
	else
	{
		// Cutscene is new, so show it as modified
		setModified();
		m_cutscene = std::make_shared<NovelTea::Cutscene>();
	}

	ui->propertyEditor->setValue(m_cutscene->getProperties());
	fillSettingsPropertyEditor();
	updateCutscene();

	MODIFIER(ui->listWidget->model(), &QAbstractItemModel::dataChanged);
	MODIFIER(ui->listWidget->model(), &QAbstractItemModel::rowsRemoved);
	MODIFIER(ui->listWidget->model(), &QAbstractItemModel::rowsInserted);
	MODIFIER(ui->propertyEditor, &PropertyEditor::valueChanged);
}

void CutsceneWidget::segmentPropertyChanged(QtProperty *property, const QVariant &value)
{
	if (selectedIndex < 0)
		return;

	setModified();

	auto propertyName = property->propertyName();
	auto segment = m_cutscene->internalSegments()[selectedIndex];
	auto type = segment->type();
	qDebug() << "propChanged " << propertyName;

	if (type == NovelTea::CutsceneSegment::Text)
	{
		auto textSegment = static_cast<NovelTea::CutsceneTextSegment*>(segment.get());

		if (propertyName == propText)
		{
			auto activeText = value.value<std::shared_ptr<NovelTea::ActiveText>>();
			ui->listWidget->currentItem()->setText(QString::fromStdString(activeText->toPlainText(" | ")));
			textSegment->setActiveText(activeText);
		}
		else if (propertyName == propBeginNewLine)
			textSegment->setBeginWithNewLine(value.toBool());
		else if (propertyName == propTextEffect)
			textSegment->setTransition(value.toInt());
		else if (propertyName == propOffset) {
			auto point = value.toPoint();
			textSegment->setOffsetX(point.x());
			textSegment->setOffsetY(point.y());
		}
	}
	else if (type == NovelTea::CutsceneSegment::PageBreak)
	{
		auto pageBreakSegment = static_cast<NovelTea::CutscenePageBreakSegment*>(segment.get());

		if (propertyName == propBreakEffect)
			pageBreakSegment->setTransition(value.toInt());
	}
	else if (type == NovelTea::CutsceneSegment::Page)
	{
		auto pageSegment = static_cast<NovelTea::CutscenePageSegment*>(segment.get());

		if (propertyName == propText){
			auto activeText = value.value<std::shared_ptr<NovelTea::ActiveText>>();
			ui->listWidget->currentItem()->setText(QString::fromStdString(activeText->toPlainText(" | ")));
			pageSegment->setText(activeText->toPlainText());
		}
		else if (propertyName == propTextDelimiter)
			pageSegment->setTextDelimiter(EditorUtils::unescape(value.toString()).toStdString());
		else if (propertyName == propTextEffect)
			pageSegment->setTextEffect(value.toInt());
		else if (propertyName == propTextDuration)
			pageSegment->setTextDuration(value.toInt());
		else if (propertyName == propTextDelay)
			pageSegment->setTextDelay(value.toInt());
		else if (propertyName == propBreakDelimiter)
			pageSegment->setBreakDelimiter(EditorUtils::unescape(value.toString()).toStdString());
		else if (propertyName == propBreakEffect)
			pageSegment->setBreakEffect(value.toInt());
		else if (propertyName == propBreakDuration)
			pageSegment->setBreakDuration(value.toInt());
		else if (propertyName == propBreakDelay)
			pageSegment->setBreakDelay(value.toInt());
		else if (propertyName == propBeginNewLine)
			pageSegment->setBeginWithNewLine(value.toBool());
		else if (propertyName == propOffset) {
			auto point = value.toPoint();
			pageSegment->setOffsetX(point.x());
			pageSegment->setOffsetY(point.y());
		}
	}

	if (type != NovelTea::CutsceneSegment::Page)
	{
		if (propertyName == propDuration)
			segment->setDuration(value.toInt());
		else if (propertyName == propDelay)
			segment->setDelay(value.toInt());
	}

	if (propertyName == propWaitForClick)
		segment->setWaitForClick(value.toBool());
	else if (propertyName == propCanSkip)
		segment->setCanSkip(value.toBool());

	updateCutscene();
}

void CutsceneWidget::settingPropertyChanged(QtProperty *property, const QVariant &value)
{
	auto propertyName = property->propertyName();
	if (propertyName == propFullScreen)
		m_cutscene->setFullScreen(value.toBool());
	else if (propertyName == propCanFastForward)
		m_cutscene->setCanFastForward(value.toBool());
	else if (propertyName == propSpeedFactor)
		m_cutscene->setSpeedFactor(value.toFloat());

	setModified();
	updateCutscene();
}

void CutsceneWidget::on_rowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
	auto &segments = m_cutscene->internalSegments();
	segments.insert(segments.begin() + destinationRow, segments[sourceStart]);
	segments.erase(segments.begin() + ((sourceStart < destinationRow) ? sourceStart : sourceStart+1));
	m_cutscene->updateSegments();
//	qDebug() << "start:" << sourceStart << " dest:" << destinationRow;
	setModified();
	updateCutscene();
}

void CutsceneWidget::on_actionAddText_triggered()
{
	addItem(std::make_shared<NovelTea::CutsceneTextSegment>(), true, ui->listWidget->currentRow());
}

void CutsceneWidget::on_actionAddPage_triggered()
{
	addItem(std::make_shared<NovelTea::CutscenePageSegment>(), true, ui->listWidget->currentRow());
}

void CutsceneWidget::on_actionAddPageBreak_triggered()
{
	addItem(std::make_shared<NovelTea::CutscenePageBreakSegment>(), true, ui->listWidget->currentRow());
}

void CutsceneWidget::timerEvent(QTimerEvent *event)
{
	auto currentValue = ui->horizontalSlider->value();

	if (m_cutscenePlaying)
	{
		if (currentValue >= ui->horizontalSlider->maximum())
		{
			ui->actionStop->trigger();
			return;
		}
	}
	else if (m_segmentLooping)
	{
		checkIndexChange();
		if (currentValue >= m_loopEndMs || currentValue < m_loopStartMs)
		{
			ui->horizontalSlider->setValue(m_loopStartMs);
			return;
		}
		if (m_loopStartMs == m_loopEndMs)
			return;
	}
	else
	{
		checkIndexChange();
		return;
	}

	auto timeMs = NovelTea::Engine::getSystemTimeMs();
	auto deltaMs = timeMs - m_lastTimeMs;
	auto jdata = json({"event","update", "delta",deltaMs});
	ui->preview->processData(jdata);

	ui->horizontalSlider->setValue(currentValue + deltaMs);
	m_lastTimeMs = timeMs;
}

void CutsceneWidget::showEvent(QShowEvent *event)
{
	EditorTabWidget::showEvent(event);
	m_lastTimeMs = NovelTea::Engine::getSystemTimeMs();
	timerId = startTimer(50);
}

void CutsceneWidget::hideEvent(QHideEvent *event)
{
	EditorTabWidget::hideEvent(event);
	killTimer(timerId);
}

void CutsceneWidget::updateCutscene()
{
	auto oldValue = ui->horizontalSlider->value();
	auto oldMax = ui->horizontalSlider->maximum();
	auto newMax = m_cutscene->getDelayMs() / m_cutscene->getSpeedFactor();
	auto newValue = static_cast<float>(oldValue) / oldMax * newMax;
	ui->horizontalSlider->setMaximum(newMax);

	auto jdata = json({"event","cutscene", "cutscene",m_cutscene->toJson()});
	ui->preview->processData(jdata);

	if (oldValue > 0)
	{
		if (oldMax != newMax)
		{
			ui->horizontalSlider->setValue(newValue);
//			std::cout << "oldMax: " << oldMax << " newMax: " << newMax << std::endl;
		}
		else
		{
			jdata = json({"event","setPlaybackTime", "value",oldValue});
			ui->preview->processData(jdata);
		}
	}

	updateLoopValues();
}

void CutsceneWidget::updateLoopValues()
{
	if (selectedIndex < 0)
		return;

	auto segment = m_cutscene->internalSegments()[selectedIndex];
	auto max = ui->horizontalSlider->maximum();

	auto endMs = std::min(segment->getDuration(), segment->getDelay());
	m_loopStartMs = m_cutscene->getDelayMs(selectedIndex) / m_cutscene->getSpeedFactor();
	m_loopEndMs = m_loopStartMs + endMs / m_cutscene->getSpeedFactor();
	if (m_loopEndMs > m_loopStartMs)
		m_loopEndMs--;
	if (m_loopEndMs > max)
		m_loopEndMs = max;
}

void CutsceneWidget::on_actionRemoveSegment_triggered()
{
	auto &segments = m_cutscene->internalSegments();
	segments.erase(segments.begin() + selectedIndex);
	delete ui->listWidget->takeItem(selectedIndex);
	// Trigger change even for unselected index (-1)
	selectedIndex = -2;
	checkIndexChange();
	// Force a redraw
	ui->listWidget->hide();
	ui->listWidget->show();
	m_cutscene->updateSegments();
	updateCutscene();
}

void CutsceneWidget::on_horizontalSlider_valueChanged(int value)
{
	if (!m_segmentLooping)
	{
		int duration = 0;
		for (size_t i = 0; i < m_cutscene->internalSegments().size(); ++i)
		{
			auto segment = m_cutscene->internalSegments()[i];
			duration += segment->getDelay();
			if (duration / m_cutscene->getSpeedFactor() > value)
			{
				if (selectedIndex != i)
				{
					ui->listWidget->setCurrentRow(i);
					selectedIndex = i;
					updateLoopValues();
					fillPropertyEditor();
				}
				break;
			}
		}
	}
	else if (value > m_loopEndMs)
		value = m_loopEndMs;

	if (m_cutscenePlaying)
		return;

	json data = json({"event","setPlaybackTime", "value",value});
	ui->preview->processData(data);
}

void CutsceneWidget::on_actionPlayPause_toggled(bool checked)
{
	if (checked)
	{
		if (m_segmentLooping)
			ui->actionLoop->setChecked(false);
		if (ui->horizontalSlider->value() == ui->horizontalSlider->maximum())
			ui->horizontalSlider->setValue(0);
		m_lastTimeMs = NovelTea::Engine::getSystemTimeMs();
		ui->actionPlayPause->setIcon(QIcon::fromTheme("media-playback-pause"));
	}
	else
	{
		ui->actionPlayPause->setIcon(QIcon::fromTheme("media-playback-start"));
	}

	ui->horizontalSlider->setEnabled(!checked);
	m_cutscenePlaying = checked;
}

void CutsceneWidget::on_actionStop_triggered()
{
	if (!m_cutscenePlaying)
		return;
	ui->actionPlayPause->setChecked(false);
}

void CutsceneWidget::on_actionLoop_toggled(bool checked)
{
	if (m_cutscenePlaying)
		ui->actionPlayPause->setChecked(false);
	checkIndexChange();
	ui->horizontalSlider->setEnabled(!checked);
	m_segmentLooping = checked;

	if (checked)
		m_lastTimeMs = NovelTea::Engine::getSystemTimeMs();
	else
		ui->horizontalSlider->setValue(m_loopEndMs);
}

void CutsceneWidget::on_listWidget_clicked(const QModelIndex &index)
{
	checkIndexChange();
}
