#include "CutsceneWidget.hpp"
#include "ui_CutsceneWidget.h"
#include "EditorUtils.hpp"
#include <NovelTea/Engine.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/CutscenePageBreakSegment.hpp>
#include <NovelTea/States/StateEditor.hpp>
#include <QToolButton>
#include <QDebug>

Q_DECLARE_METATYPE(std::shared_ptr<NovelTea::ActiveText>)

#define TEXT_TEXT "Text"
#define TEXT_NEWLINE "Start New Line"
#define TRANSITION_EFFECT "Transition Effect"
#define TRANSITION_DURATION "Transition Duration"
#define SEGMENT_DELAY "Delay"
#define WAIT_FOR_CLICK "Wait for click"
#define SCRIPT_OVERRIDE_NAME "Variable or Function"
#define FULLSCREEN "Full Screen"
#define CAN_FAST_FORWARD "Can Fast-Forward"
#define SPEED_FACTOR "Speed Factor"

CutsceneWidget::CutsceneWidget(const std::string &idName, QWidget *parent) :
	EditorTabWidget(parent),
	ui(new Ui::CutsceneWidget),
	menuAdd(nullptr),
	itemModel(new QStandardItemModel(0, 2, parent)),
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

	itemModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Action"));
	itemModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Value"));
	ui->treeView->setModel(itemModel);
	ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->treeView->header()->setSectionHidden(2, true);
	ui->treeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui->treeView->header()->setSectionResizeMode(1, QHeaderView::Stretch);
	ui->treeView->header()->setSectionResizeMode(2, QHeaderView::Fixed);

	ui->richTextEditor->hide();
}

CutsceneWidget::~CutsceneWidget()
{
	delete variantFactory;
	delete settingsVariantManager;
	delete segmentsVariantManager;
	delete menuAdd;
	delete itemModel;
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
	menuAdd->addAction(ui->actionAddPageBreak);

	// Attach the menu to the Add toolbutton
	ui->actionAddSegment->setMenu(menuAdd);
	auto buttonAdd = static_cast<QToolButton*>(ui->toolBarEntities->widgetForAction(ui->actionAddSegment));
	buttonAdd->setPopupMode(QToolButton::InstantPopup);
}

void CutsceneWidget::fillPropertyEditor()
{
	ui->propertyBrowser->clear();
	ui->propertyBrowser->show();
	ui->richTextEditor->hide();
	ui->richTextEditor->disconnect();

	ui->propertyBrowser->setFactoryForManager(segmentsVariantManager, variantFactory);
	ui->propertyBrowser->setPropertiesWithoutValueMarked(true);

	// If no item is selected, there is nothing to load
	if (selectedIndex < 0)
		return;

	segmentsVariantManager->disconnect();

	QtVariantProperty *prop, *subProp;
	auto segment = m_cutscene->segments()[selectedIndex];
//	auto segment = itemModel->index(selectedIndex, 2).data().value<std::shared_ptr<NovelTea::CutsceneSegment>>();
	auto type = segment->type();

	if (type == NovelTea::ID::cutsceneSegText)
	{
		auto textSegment = static_cast<NovelTea::CutsceneTextSegment*>(segment.get());
		prop = segmentsVariantManager->addProperty(QtVariantPropertyManager::richTextTypeId(), TEXT_TEXT);
		prop->setAttribute("richTextEditor", QVariant::fromValue(ui->richTextEditor));
		prop->setValue(QVariant::fromValue(textSegment->getActiveText()));
		ui->propertyBrowser->addProperty(prop);

		prop = segmentsVariantManager->addProperty(QtVariantPropertyManager::enumTypeId(), TRANSITION_EFFECT);
		QStringList enumNames;
		enumNames << "None" << "Fade";
		prop->setAttribute(QLatin1String("enumNames"), enumNames);
		prop->setValue(textSegment->getTransition());
		ui->propertyBrowser->addProperty(prop);

		prop = segmentsVariantManager->addProperty(QVariant::Bool, TEXT_NEWLINE);
		prop->setValue(textSegment->getBeginWithNewLine());
		ui->propertyBrowser->addProperty(prop);

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
	}
	else if (type == NovelTea::ID::cutsceneSegBreak)
	{
		auto pageBreakSegment = static_cast<NovelTea::CutscenePageBreakSegment*>(segment.get());

		prop = segmentsVariantManager->addProperty(QtVariantPropertyManager::enumTypeId(), TRANSITION_EFFECT);
		QStringList enumNames;
		enumNames << "None" << "Fade" << "Scroll Left";
		prop->setAttribute(QLatin1String("enumNames"), enumNames);
		prop->setValue(pageBreakSegment->getTransition());
		ui->propertyBrowser->addProperty(prop);
	}

	prop = segmentsVariantManager->addProperty(QVariant::Int, TRANSITION_DURATION);
	prop->setValue(static_cast<unsigned int>(segment->getDuration()));
	prop->setAttribute(QLatin1String("minimum"), 0);
	prop->setAttribute(QLatin1String("maximum"), 100000);
	prop->setAttribute(QLatin1String("singleStep"), 100);
	ui->propertyBrowser->addProperty(prop);

	prop = segmentsVariantManager->addProperty(QVariant::Int, SEGMENT_DELAY);
	prop->setValue(static_cast<unsigned int>(segment->getDelay()));
	prop->setAttribute(QLatin1String("minimum"), 0);
	prop->setAttribute(QLatin1String("maximum"), 100000);
	prop->setAttribute(QLatin1String("singleStep"), 100);
	ui->propertyBrowser->addProperty(prop);

	prop = segmentsVariantManager->addProperty(QVariant::Bool, WAIT_FOR_CLICK);
	prop->setValue(segment->getWaitForClick());
	subProp = segmentsVariantManager->addProperty(QVariant::String, SCRIPT_OVERRIDE_NAME);
	subProp->setValue(QString::fromStdString(segment->getScriptOverrideName()));
	prop->addSubProperty(subProp);
	ui->propertyBrowser->addProperty(prop);

	connect(segmentsVariantManager, &QtVariantPropertyManager::valueChanged, this, &CutsceneWidget::segmentPropertyChanged);
}

void CutsceneWidget::fillSettingsPropertyEditor()
{
	ui->actionSelect->disconnect();
	settingsVariantManager->disconnect();
	ui->propertyBrowserSettings->clear();
	ui->propertyBrowserSettings->setFactoryForManager(settingsVariantManager, variantFactory);
	ui->propertyBrowserSettings->setPropertiesWithoutValueMarked(true);

	QtVariantProperty *prop;

	prop = settingsVariantManager->addProperty(QVariant::Bool, FULLSCREEN);
	prop->setValue(m_cutscene->getFullScreen());
	ui->propertyBrowserSettings->addProperty(prop);

	prop = settingsVariantManager->addProperty(QVariant::Bool, CAN_FAST_FORWARD);
	prop->setValue(m_cutscene->getCanFastForward());
	ui->propertyBrowserSettings->addProperty(prop);

	prop = settingsVariantManager->addProperty(QVariant::Double, SPEED_FACTOR);
	prop->setValue(m_cutscene->getSpeedFactor());
	ui->propertyBrowserSettings->addProperty(prop);

	ui->actionSelect->setValue(m_cutscene->getNextEntity());

	connect(settingsVariantManager, &QtVariantPropertyManager::valueChanged, this, &CutsceneWidget::settingPropertyChanged);
	connect(ui->actionSelect, &ActionSelectWidget::valueChanged, [this](){
		setModified();
		m_cutscene->setNextEntity(ui->actionSelect->getValue());
	});
}

void CutsceneWidget::checkIndexChange()
{

	int index = ui->treeView->currentIndex().row();
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
	QStandardItem *item = nullptr;
	QString text;

	if (type == NovelTea::CutsceneSegment::Text)
	{
		auto seg = static_cast<NovelTea::CutsceneTextSegment*>(segment.get());
		item = new QStandardItem(QIcon::fromTheme("font"), "Text");
		text = QString::fromStdString(seg->getActiveText()->toPlainText());
	}
	else if (type == NovelTea::CutsceneSegment::PageBreak)
	{
		item = new QStandardItem(QIcon::fromTheme("document-new"), "Page Break");
	}

	if (!item)
		return;

	if (index < 0)
	{
		index = itemModel->rowCount();
		itemModel->appendRow(item);
		if (addToInternalObject)
			m_cutscene->segments().push_back(segment);
	}
	else
	{
		itemModel->insertRow(index, item);
		if (addToInternalObject)
			m_cutscene->segments().insert(m_cutscene->segments().begin() + index, segment);
	}

	itemModel->setData(itemModel->index(index, 1), text);
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
	itemModel->disconnect();
	itemModel->removeRows(0, itemModel->rowCount());

	qDebug() << "Loading cutscene data... " << QString::fromStdString(idName());

	if (m_cutscene)
		for (auto &seg : m_cutscene->segments())
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

	MODIFIER(itemModel, &QStandardItemModel::dataChanged);
	MODIFIER(itemModel, &QStandardItemModel::rowsRemoved);
	MODIFIER(itemModel, &QStandardItemModel::rowsInserted);
	MODIFIER(ui->propertyEditor, &PropertyEditor::valueChanged);
}

void CutsceneWidget::segmentPropertyChanged(QtProperty *property, const QVariant &value)
{
	if (selectedIndex < 0)
		return;

	setModified();

	auto propertyName = property->propertyName();
	auto segment = m_cutscene->segments()[selectedIndex];
	auto type = segment->type();
	qDebug() << "propChanged " << propertyName;

	if (type == NovelTea::CutsceneSegment::Text)
	{
		auto textSegment = static_cast<NovelTea::CutsceneTextSegment*>(segment.get());

		if (propertyName == TEXT_TEXT)
		{
			auto activeText = value.value<std::shared_ptr<NovelTea::ActiveText>>();
			ui->richTextEditor->setValue(activeText);
			itemModel->setData(itemModel->index(selectedIndex, 1), QString::fromStdString(activeText->toPlainText()));
			textSegment->setActiveText(activeText);
		}
		else if (propertyName == TEXT_NEWLINE)
			textSegment->setBeginWithNewLine(value.toBool());
		else if (propertyName == TRANSITION_EFFECT)
			textSegment->setTransition(value.toInt());
	}
	else if (type == NovelTea::CutsceneSegment::PageBreak)
	{
		auto pageBreakSegment = static_cast<NovelTea::CutscenePageBreakSegment*>(segment.get());

		if (propertyName == TRANSITION_EFFECT)
			pageBreakSegment->setTransition(value.toInt());
	}

	if (propertyName == TRANSITION_DURATION)
		segment->setDuration(value.toInt());
	else if (propertyName == SEGMENT_DELAY)
		segment->setDelay(value.toInt());
	else if (propertyName == WAIT_FOR_CLICK)
		segment->setWaitForClick(value.toBool());
	else if (propertyName == SCRIPT_OVERRIDE_NAME)
		segment->setScriptOverrideName(value.toString().toStdString());

	updateCutscene();
}

void CutsceneWidget::settingPropertyChanged(QtProperty *property, const QVariant &value)
{
	auto propertyName = property->propertyName();
	if (propertyName == FULLSCREEN)
		m_cutscene->setFullScreen(value.toBool());
	else if (propertyName == CAN_FAST_FORWARD)
		m_cutscene->setCanFastForward(value.toBool());
	else if (propertyName == SPEED_FACTOR)
		m_cutscene->setSpeedFactor(value.toFloat());

	setModified();
	updateCutscene();
}

void CutsceneWidget::on_actionAddText_triggered()
{
	addItem(std::make_shared<NovelTea::CutsceneTextSegment>());
}

void CutsceneWidget::on_actionAddPageBreak_triggered()
{
	addItem(std::make_shared<NovelTea::CutscenePageBreakSegment>());
}

void CutsceneWidget::on_treeView_pressed(const QModelIndex &index)
{
	checkIndexChange();
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
			std::cout << "oldMax: " << oldMax << " newMax: " << newMax << std::endl;
			ui->horizontalSlider->setValue(newValue);
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

	auto segment = m_cutscene->segments()[selectedIndex];
	auto max = ui->horizontalSlider->maximum();

	m_loopStartMs = m_cutscene->getDelayMs(selectedIndex) / m_cutscene->getSpeedFactor();
	m_loopEndMs = m_loopStartMs + segment->getDuration() / m_cutscene->getSpeedFactor();
	if (m_loopEndMs > max)
		m_loopEndMs = max;
}

void CutsceneWidget::on_actionRemoveSegment_triggered()
{
	auto &segments = m_cutscene->segments();
	segments.erase(segments.begin() + selectedIndex);
	itemModel->removeRow(selectedIndex);
	// Trigger change even for unselected index (-1)
	selectedIndex = -2;
	checkIndexChange();
}

void CutsceneWidget::on_horizontalSlider_valueChanged(int value)
{
	if (!m_segmentLooping)
	{
		int duration = 0;
		for (size_t i = 0; i < m_cutscene->segments().size(); ++i)
		{
			auto segment = m_cutscene->segments()[i];
			duration += segment->getDelay();
			if (duration / m_cutscene->getSpeedFactor() > value)
			{
				if (selectedIndex != i)
				{
					ui->treeView->setCurrentIndex(itemModel->index(i,0));
					selectedIndex = i;
					updateLoopValues();
					fillPropertyEditor();
				}
				break;
			}
		}
	}

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
