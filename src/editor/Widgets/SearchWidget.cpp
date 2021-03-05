#include "SearchWidget.hpp"
#include "MainWindow.hpp"
#include "ui_SearchWidget.h"
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/CutsceneTextSegment.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/DialogueSegment.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/Script.hpp>
#include <NovelTea/Verb.hpp>

SearchWidget::SearchWidget(const std::string &searchTerm, QWidget *parent)
: EditorTabWidget(parent)
, ui(new Ui::SearchWidget)
, m_searchTerm(searchTerm)
{
	m_idName = searchTerm;
	ui->setupUi(this);
	load();
}

SearchWidget::~SearchWidget()
{
	delete ui;
}

QString SearchWidget::tabText() const
{
	return QString::fromStdString("Search: " + m_searchTerm);
}

EditorTabWidget::Type SearchWidget::getType() const
{
	return EditorTabWidget::Search;
}

void SearchWidget::on_buttonSearchAgain_clicked()
{
	loadData();
}

void SearchWidget::saveData() const
{
	//
}

void SearchWidget::loadData()
{
	auto caseSensitive = true;
	ui->treeWidget->clear();

	searchProjectSettings(caseSensitive);
	searchTests(caseSensitive);
	searchEntities(NovelTea::Action::id, "Actions", caseSensitive);
	searchEntities(NovelTea::Cutscene::id, "Cutscenes", caseSensitive);
	searchEntities(NovelTea::Dialogue::id, "Dialogues", caseSensitive);
	searchEntities(NovelTea::Object::id, "Objects", caseSensitive);
	searchEntities(NovelTea::Room::id, "Rooms", caseSensitive);
	searchEntities(NovelTea::Script::id, "Scripts", caseSensitive);
	searchEntities(NovelTea::Verb::id, "Verbs", caseSensitive);
}

void SearchWidget::processString(QTreeWidgetItem *treeItem, const std::string &value, bool caseSensitive)
{
	auto searchTerm = QString::fromStdString(m_searchTerm);
	auto val = QString::fromStdString(value);
	auto lines = val.split("\n", QString::SkipEmptyParts);
	for (auto &line : lines)
	{
		int start = 0;
		if (line.indexOf(searchTerm, start, caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive) != -1)
		{
			auto subItem = new QTreeWidgetItem(treeItem);
			subItem->setText(0,  line);
		}
	}
}

void SearchWidget::searchEntities(const std::string &entityId, const QString &name, bool caseSensitive)
{
	std::shared_ptr<NovelTea::Entity> entity = nullptr;
	auto parentItem = new QTreeWidgetItem;
	auto count = 0;
	auto entities = ProjData[entityId];
	for (auto &entityMap : entities.ObjectRange())
	{
		auto item = new QTreeWidgetItem;

		if (entityId == NovelTea::Action::id) {
			entity = std::make_shared<NovelTea::Action>();
			item->setData(0, Qt::UserRole, EditorTabWidget::Action);
		}
		else if (entityId == NovelTea::Cutscene::id) {
			entity = std::make_shared<NovelTea::Cutscene>();
			item->setData(0, Qt::UserRole, EditorTabWidget::Cutscene);
		}
		else if (entityId == NovelTea::Dialogue::id) {
			entity = std::make_shared<NovelTea::Dialogue>();
			item->setData(0, Qt::UserRole, EditorTabWidget::Dialogue);
		}
		else if (entityId == NovelTea::Object::id) {
			entity = std::make_shared<NovelTea::Object>();
			item->setData(0, Qt::UserRole, EditorTabWidget::Object);
		}
		else if (entityId == NovelTea::Room::id) {
			entity = std::make_shared<NovelTea::Room>();
			item->setData(0, Qt::UserRole, EditorTabWidget::Room);
		}
		else if (entityId == NovelTea::Script::id) {
			entity = std::make_shared<NovelTea::Script>();
			item->setData(0, Qt::UserRole, EditorTabWidget::Script);
		}
		else if (entityId == NovelTea::Verb::id) {
			entity = std::make_shared<NovelTea::Verb>();
			item->setData(0, Qt::UserRole, EditorTabWidget::Verb);
		}
		else
			throw std::exception();

		entity->fromJson(entityMap.second);
		item->setData(0, Qt::UserRole+1, QString::fromStdString(entity->getId()));

		if (entityId == NovelTea::Action::id) {
			auto action = std::static_pointer_cast<NovelTea::Action>(entity);
			processString(item, action->getScript(), caseSensitive);
		}
		else if (entityId == NovelTea::Cutscene::id) {
			auto cutscene = std::static_pointer_cast<NovelTea::Cutscene>(entity);
			auto &segments = cutscene->segments();
			for (auto &segment : segments) {
				if (segment->type() != NovelTea::CutsceneSegment::Text)
					continue;
				auto textSeg = std::static_pointer_cast<NovelTea::CutsceneTextSegment>(segment);
				processString(item, textSeg->getActiveText()->toPlainText(), caseSensitive);
			}
		}
		else if (entityId == NovelTea::Dialogue::id) {
			auto dialogue = std::static_pointer_cast<NovelTea::Dialogue>(entity);
			auto segments = dialogue->segments();
			for (auto &segment : segments) {
				auto type = segment->getType();
				if (type != NovelTea::DialogueSegment::Text && type != NovelTea::DialogueSegment::Option)
					continue;
				processString(item, segment->getTextRaw(), caseSensitive);
				processString(item, segment->getScript(), caseSensitive);
				processString(item, segment->getConditionScript(), caseSensitive);
			}
			processString(item, dialogue->getDefaultName(), caseSensitive);
		}
		else if (entityId == NovelTea::Object::id) {
			auto object = std::static_pointer_cast<NovelTea::Object>(entity);
			processString(item, object->getName(), caseSensitive);
		}
		else if (entityId == NovelTea::Room::id) {
			auto room = std::static_pointer_cast<NovelTea::Room>(entity);
			processString(item, room->getDescriptionRaw(), caseSensitive);
			processString(item, room->getPaths().dump(), caseSensitive);
			processString(item, room->getScriptBeforeLeave(), caseSensitive);
			processString(item, room->getScriptBeforeEnter(), caseSensitive);
			processString(item, room->getScriptAfterLeave(), caseSensitive);
			processString(item, room->getScriptAfterEnter(), caseSensitive);
			for (auto &obj : room->getObjects())
				processString(item, obj.idName, caseSensitive);
		}
		else if (entityId == NovelTea::Script::id) {
			auto script = std::static_pointer_cast<NovelTea::Script>(entity);
			processString(item, script->getContent(), caseSensitive);
		}
		else if (entityId == NovelTea::Verb::id) {
			auto verb = std::static_pointer_cast<NovelTea::Verb>(entity);
			processString(item, verb->getName(), caseSensitive);
			processString(item, verb->getScriptDefault(), caseSensitive);
			processString(item, verb->getScriptConditional(), caseSensitive);
		}

		auto props = entity->getProperties();
		processString(item, props.dump(), caseSensitive);

		if (item->childCount() == 0) {
			delete item;
			continue;
		}

		count += item->childCount();
		item->setText(0, QString::fromStdString(entity->getId() + " (%1)").arg(item->childCount()));
		parentItem->addChild(item);
	}

	if (parentItem->childCount() == 0) {
		delete parentItem;
		return;
	}

	parentItem->setText(0, QString("%1 (%2)").arg(name).arg(count));
	ui->treeWidget->addTopLevelItem(parentItem);
}

void SearchWidget::searchTests(bool caseSensitive)
{
	auto parentItem = new QTreeWidgetItem;
	auto count = 0;
	auto tests = ProjData[NovelTea::ID::tests];
	for (auto &test : tests.ObjectRange()) {
		auto item = new QTreeWidgetItem;
		item->setData(0, Qt::UserRole, EditorTabWidget::Verb);

		processString(item, test.second.dump(), caseSensitive);

		if (item->childCount() == 0) {
			delete item;
			continue;
		}

		count += item->childCount();
		item->setText(0, QString::fromStdString(test.first + " (%1)").arg(item->childCount()));
		parentItem->addChild(item);
	}

	if (parentItem->childCount() == 0) {
		delete parentItem;
		return;
	}

	parentItem->setText(0, QString("Tests (%2)").arg(count));
	ui->treeWidget->addTopLevelItem(parentItem);
}

void SearchWidget::searchProjectSettings(bool caseSensitive)
{
	auto count = 0;
	auto item = new QTreeWidgetItem;
	item->setData(0, Qt::UserRole, EditorTabWidget::Settings);

	processString(item, ProjData[NovelTea::ID::scriptBeforeAction].ToString(), caseSensitive);
	processString(item, ProjData[NovelTea::ID::scriptAfterAction].ToString(), caseSensitive);
	processString(item, ProjData[NovelTea::ID::scriptUndefinedAction].ToString(), caseSensitive);
	processString(item, ProjData[NovelTea::ID::scriptBeforeLeave].ToString(), caseSensitive);
	processString(item, ProjData[NovelTea::ID::scriptBeforeEnter].ToString(), caseSensitive);
	processString(item, ProjData[NovelTea::ID::scriptAfterLeave].ToString(), caseSensitive);
	processString(item, ProjData[NovelTea::ID::scriptAfterEnter].ToString(), caseSensitive);

	if (item->childCount() == 0) {
		delete item;
		return;
	}

	auto parentItem = new QTreeWidgetItem;
	count += item->childCount();
	item->setText(0, QString::fromStdString("Scripts (%1)").arg(item->childCount()));
	parentItem->addChild(item);

	parentItem->setText(0, QString("Project Settings (%2)").arg(count));
	ui->treeWidget->addTopLevelItem(parentItem);
}

void SearchWidget::on_treeWidget_activated(const QModelIndex &index)
{
	if (!index.parent().parent().isValid())
		return;
	auto type = static_cast<EditorTabWidget::Type>(index.parent().data(Qt::UserRole).toInt());
	auto idName = index.parent().data(Qt::UserRole+1).toString().toStdString();
	MainWindow::instance().addEditorTab(type, idName);
}
