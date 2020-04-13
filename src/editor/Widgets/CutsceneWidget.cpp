#include "CutsceneWidget.hpp"
#include "ui_CutsceneWidget.h"
#include "Utils.hpp"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/CutscenePageBreakSegment.hpp>
#include <QToolButton>
#include <QDebug>

Q_DECLARE_METATYPE(NovelTea::ActiveText)
Q_DECLARE_METATYPE(std::shared_ptr<NovelTea::CutsceneSegment>)

#define TEXT_TEXT "Text"
#define TEXT_NEWLINE "Start New Line"
#define TRANSITION_EFFECT "Transition Effect"
#define TRANSITION_DURATION "Transition Duration"
#define SCRIPT_OVERRIDE "Script Override"
#define SCRIPT_OVERRIDE_NAME "Variable or Function"

CutsceneWidget::CutsceneWidget(const std::string &idName, QWidget *parent) :
	EditorTabWidget(parent),
	ui(new Ui::CutsceneWidget),
	menuAdd(nullptr),
	itemModel(new QStandardItemModel(0, 3, parent)),
	selectedIndex(-1),
	variantManager(new QtVariantPropertyManager),
	variantFactory(new QtVariantEditorFactory)
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

	ui->propertyBrowser->setFactoryForManager(variantManager, variantFactory);
	ui->propertyBrowser->setPropertiesWithoutValueMarked(true);

	ui->richTextEditor->hide();

	startTimer(200);
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
	variantManager->disconnect();
	ui->propertyBrowser->clear();
	ui->propertyBrowser->show();
	ui->richTextEditor->hide();
	ui->richTextEditor->disconnect();

	// If no item is selected, there is nothing to load
	if (selectedIndex < 0)
		return;

	QtVariantProperty *prop, *subProp;
	auto segment = itemModel->index(selectedIndex, 2).data().value<std::shared_ptr<NovelTea::CutsceneSegment>>();
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

		prop = variantManager->addProperty(QVariant::Int, TRANSITION_DURATION);
		prop->setValue(textSegment->getTransitionDuration());
		prop->setAttribute(QLatin1String("minimum"), 0);
		prop->setAttribute(QLatin1String("maximum"), 10000);
		prop->setAttribute(QLatin1String("singleStep"), 10);
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
		prop = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(), TRANSITION_EFFECT);
		QStringList enumNames;
		enumNames << "Fade" << "Scroll Up" << "Scroll Down" << "Nothing";
		prop->setAttribute(QLatin1String("enumNames"), enumNames);
		prop->setValue(0);
		ui->propertyBrowser->addProperty(prop);
	}

	prop = variantManager->addProperty(QVariant::Bool, SCRIPT_OVERRIDE);
	prop->setValue(segment->getScriptOverride());
	subProp = variantManager->addProperty(QVariant::String, SCRIPT_OVERRIDE_NAME);
	subProp->setValue(QString::fromStdString(segment->getScriptOverrideName()));
	prop->addSubProperty(subProp);
	ui->propertyBrowser->addProperty(prop);

	connect(variantManager, &QtVariantPropertyManager::valueChanged, this, &CutsceneWidget::propertyChanged);
}

void CutsceneWidget::checkIndexChange()
{
	int index = ui->treeView->currentIndex().row();
	if (selectedIndex != index)
	{
		selectedIndex = index;

		json data = json::object();
		data["type"] = "test";
		data["str"] = "hello! " + std::to_string(index);
		auto resp = ui->preview->processData(data);

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
		text = QString::fromStdString(seg->getActiveText().toPlainText());
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
	}
	else
		itemModel->insertRow(index, item);

	itemModel->setData(itemModel->index(index, 1), text);
	itemModel->setData(itemModel->index(index, 2), QVariant::fromValue(segment));
}

void CutsceneWidget::saveData() const
{
	auto &j = Proj.data()[NT_CUTSCENES][idName()];
	auto a = json::array();
	for (int i = 0; i < itemModel->rowCount(); ++i)
	{
		auto d = itemModel->index(i, 2).data().value<std::shared_ptr<NovelTea::CutsceneSegment>>();
		a.push_back(*d);
	}
	std::cout << "saveData()" << std::endl << a << std::endl;
	j = a;
}

void CutsceneWidget::loadData()
{
	itemModel->disconnect();
	itemModel->removeRows(0, itemModel->rowCount());

	qDebug() << "Loading cutscene data... " << QString::fromStdString(idName());

	if (auto cutscene = Proj.cutscene(idName()))
		for (auto &seg : cutscene->segments())
			addItem(seg);
	else
		setModified();

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
	auto segment = itemModel->index(selectedIndex, 2).data().value<std::shared_ptr<NovelTea::CutsceneSegment>>();
	auto type = segment->type();
	qDebug() << "propChanged " << propertyName;

	if (type == NovelTea::CutsceneSegment::Text)
	{
		auto textSegment = static_cast<NovelTea::CutsceneTextSegment*>(segment.get());

		if (propertyName == TEXT_TEXT)
		{
			auto activeText = value.value<NovelTea::ActiveText>();
			ui->richTextEditor->setValue(activeText);
			itemModel->setData(itemModel->index(selectedIndex, 1), QString::fromStdString(activeText.toPlainText()));
			textSegment->setActiveText(activeText);
		}
		else if (propertyName == TEXT_NEWLINE)
			textSegment->setBeginWithNewLine(value.toBool());
		else if (propertyName == TRANSITION_EFFECT)
			textSegment->setTransition(value.toInt());
		else if (propertyName == TRANSITION_DURATION)
			textSegment->setTransitionDuration(value.toInt());
	}
	else if (type == NovelTea::CutsceneSegment::PageBreak)
	{
		auto pageBreakSegment = static_cast<NovelTea::CutscenePageBreakSegment*>(segment.get());

	}

	if (propertyName == SCRIPT_OVERRIDE)
		segment->setScriptOverride(value.toBool());
	else if (propertyName == SCRIPT_OVERRIDE_NAME)
		segment->setScriptOverrideName(value.toString().toStdString());

//	itemModel->setData(itemModel->index(selectedIndex, 2), QVariant::fromValue(data));

	auto d = json::object({{"event", "update"}});
	d["data"] = d;
	auto resp = ui->preview->processData(d);
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
	checkIndexChange();
}

void CutsceneWidget::on_actionRemoveSegment_triggered()
{
	itemModel->removeRow(selectedIndex);
	// Trigger change even for unselected index (-1)
	selectedIndex = -2;
	checkIndexChange();
}
