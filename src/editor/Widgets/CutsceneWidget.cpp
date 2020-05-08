#include "CutsceneWidget.hpp"
#include "ui_CutsceneWidget.h"
#include "EditorUtils.hpp"
#include <NovelTea/Engine.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/CutscenePageBreakSegment.hpp>
#include <QToolButton>
#include <QDebug>

Q_DECLARE_METATYPE(std::shared_ptr<NovelTea::ActiveText>)

#define TEXT_TEXT "Text"
#define TEXT_NEWLINE "Start New Line"
#define TRANSITION_EFFECT "Transition Effect"
#define TRANSITION_DURATION "Transition Duration"
#define SEGMENT_DELAY "Delay"
#define SCRIPT_OVERRIDE "Script Override"
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
	variantManager(new QtVariantPropertyManager),
	variantFactory(new QtVariantEditorFactory),
	m_cutscenePlaying(false)
{
	_idName = idName;
	ui->setupUi(this);
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
	delete variantManager;
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

	ui->propertyBrowser->setFactoryForManager(variantManager, variantFactory);
	ui->propertyBrowser->setPropertiesWithoutValueMarked(true);

	// If no item is selected, there is nothing to load
	if (selectedIndex < 0)
		return;

	variantManager->disconnect();

	QtVariantProperty *prop, *subProp;
	auto segment = m_cutscene->segments()[selectedIndex];
	auto type = segment->type();

	if (type == NT_CUTSCENE_TEXT)
	{
		auto textSegment = static_cast<NovelTea::CutsceneTextSegment*>(segment.get());
		prop = variantManager->addProperty(QtVariantPropertyManager::richTextTypeId(), TEXT_TEXT);
		prop->setAttribute("richTextEditor", QVariant::fromValue(ui->richTextEditor));
		prop->setValue(QVariant::fromValue(textSegment->getActiveText()));
		ui->propertyBrowser->addProperty(prop);

		prop = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(), TRANSITION_EFFECT);
		QStringList enumNames;
		enumNames << "Fade" << "Scroll Up" << "Scroll Down" << "Nothing";
		prop->setAttribute(QLatin1String("enumNames"), enumNames);
		prop->setValue(textSegment->getTransition());
		ui->propertyBrowser->addProperty(prop);

		prop = variantManager->addProperty(QVariant::Bool, TEXT_NEWLINE);
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
	else if (type == NT_CUTSCENE_BREAK)
	{
		auto pageBreakSegment = static_cast<NovelTea::CutscenePageBreakSegment*>(segment.get());

		prop = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(), TRANSITION_EFFECT);
		QStringList enumNames;
		enumNames << "Fade" << "Scroll Up" << "Scroll Down" << "Nothing";
		prop->setAttribute(QLatin1String("enumNames"), enumNames);
		prop->setValue(pageBreakSegment->getTransition());
		ui->propertyBrowser->addProperty(prop);
	}

	prop = variantManager->addProperty(QVariant::Int, TRANSITION_DURATION);
	prop->setValue(segment->getDuration());
	prop->setAttribute(QLatin1String("minimum"), 0);
	prop->setAttribute(QLatin1String("maximum"), 100000);
	prop->setAttribute(QLatin1String("singleStep"), 100);
	ui->propertyBrowser->addProperty(prop);

	prop = variantManager->addProperty(QVariant::Int, SEGMENT_DELAY);
	prop->setValue(segment->getDelay());
	prop->setAttribute(QLatin1String("minimum"), 0);
	prop->setAttribute(QLatin1String("maximum"), 100000);
	prop->setAttribute(QLatin1String("singleStep"), 100);
	ui->propertyBrowser->addProperty(prop);

	prop = variantManager->addProperty(QVariant::Bool, SCRIPT_OVERRIDE);
	prop->setValue(segment->getScriptOverride());
	subProp = variantManager->addProperty(QVariant::String, SCRIPT_OVERRIDE_NAME);
	subProp->setValue(QString::fromStdString(segment->getScriptOverrideName()));
	prop->addSubProperty(subProp);
	ui->propertyBrowser->addProperty(prop);

	connect(variantManager, &QtVariantPropertyManager::valueChanged, this, &CutsceneWidget::propertyChanged);
}

void CutsceneWidget::fillSettingsPropertyEditor(std::shared_ptr<NovelTea::Cutscene> cutscene)
{
	variantManager->disconnect();
	ui->propertyBrowserSettings->clear();
	ui->propertyBrowserSettings->setFactoryForManager(variantManager, variantFactory);
	ui->propertyBrowserSettings->setPropertiesWithoutValueMarked(true);

	QtVariantProperty *prop;

	prop = variantManager->addProperty(QVariant::Bool, FULLSCREEN);
	prop->setValue(cutscene->getFullScreen());
	ui->propertyBrowserSettings->addProperty(prop);

	prop = variantManager->addProperty(QVariant::Bool, CAN_FAST_FORWARD);
	prop->setValue(cutscene->getCanFastForward());
	ui->propertyBrowserSettings->addProperty(prop);

	prop = variantManager->addProperty(QVariant::Double, SPEED_FACTOR);
	prop->setValue(cutscene->getSpeedFactor());
	ui->propertyBrowserSettings->addProperty(prop);

	connect(variantManager, &QtVariantPropertyManager::valueChanged, this, &CutsceneWidget::propertyChanged);
}

void CutsceneWidget::checkIndexChange()
{

	int index = ui->treeView->currentIndex().row();
	if (selectedIndex != index)
	{
		selectedIndex = index;

		updateLoopValues();

		if (!m_cutscenePlaying && !m_segmentLooping)
			ui->horizontalSlider->setValue(m_loopStartMs);

		fillPropertyEditor();
	}

	ui->actionRemoveSegment->setEnabled(index >= 0);
}

void CutsceneWidget::addItem(std::shared_ptr<NovelTea::CutsceneSegment> segment, int index)
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
		m_cutscene->segments().push_back(segment);
	}
	else
	{
		itemModel->insertRow(index, item);
		m_cutscene->segments().insert(m_cutscene->segments().begin() + index, segment);
	}

	itemModel->setData(itemModel->index(index, 1), text);
	updateCutscene();
}

void CutsceneWidget::saveData() const
{
	if (m_cutscene)
		Proj.data()[NT_CUTSCENES][idName()] = *m_cutscene;
}

void CutsceneWidget::loadData()
{
	m_cutscene = std::make_shared<NovelTea::Cutscene>();
	itemModel->disconnect();
	itemModel->removeRows(0, itemModel->rowCount());

	qDebug() << "Loading cutscene data... " << QString::fromStdString(idName());

	auto cutscene = Proj.cutscene(idName());
	if (cutscene)
		for (auto &seg : cutscene->segments())
			addItem(seg);
	else
		setModified(); // Cutscene is new, so show it as modified

	fillSettingsPropertyEditor(cutscene);

	MODIFIER(itemModel, &QStandardItemModel::dataChanged);
	MODIFIER(itemModel, &QStandardItemModel::rowsRemoved);
	MODIFIER(itemModel, &QStandardItemModel::rowsInserted);
}

void CutsceneWidget::propertyChanged(QtProperty *property, const QVariant &value)
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
	else if (propertyName == SCRIPT_OVERRIDE)
		segment->setScriptOverride(value.toBool());
	else if (propertyName == SCRIPT_OVERRIDE_NAME)
		segment->setScriptOverrideName(value.toString().toStdString());

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
	auto jdata = json::object({{"event","update"}, {"delta",deltaMs}});
	ui->preview->processData(jdata);

	ui->horizontalSlider->setValue(currentValue + deltaMs);
	m_lastTimeMs = timeMs;
}

void CutsceneWidget::showEvent(QShowEvent *)
{
	m_lastTimeMs = NovelTea::Engine::getSystemTimeMs();
	timerId = startTimer(50);
}

void CutsceneWidget::hideEvent(QHideEvent *)
{
	killTimer(timerId);
}

void CutsceneWidget::updateCutscene()
{
	ui->horizontalSlider->setMaximum(m_cutscene->getDelayMs());
	auto value = ui->horizontalSlider->value();

	auto jdata = json::object({{"event","cutscene"}, {"cutscene",*m_cutscene}});
	ui->preview->processData(jdata);

	if (value > 0)
	{
		jdata = json::object({{"event","setPlaybackTime"}, {"value",value}});
		ui->preview->processData(jdata);
	}

	updateLoopValues();
}

void CutsceneWidget::updateLoopValues()
{
	if (selectedIndex < 0)
		return;

	auto segment = m_cutscene->segments()[selectedIndex];
	auto max = ui->horizontalSlider->maximum();

	m_loopStartMs = m_cutscene->getDelayMs(selectedIndex);
	m_loopEndMs = m_loopStartMs + segment->getDuration();
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
			if (duration > value)
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

	json data = json::object({{"event","setPlaybackTime"}, {"value",value}});
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
	if (checked)
		m_lastTimeMs = NovelTea::Engine::getSystemTimeMs();
	checkIndexChange();
	ui->horizontalSlider->setEnabled(!checked);
	m_segmentLooping = checked;
}
