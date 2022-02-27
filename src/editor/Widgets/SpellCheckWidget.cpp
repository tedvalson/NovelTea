#include "SpellCheckWidget.hpp"
#include "MainWindow.hpp"
#include "ui_SpellCheckWidget.h"
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
#include <QTextCursor>

namespace {
enum DataType {
	TabType = Qt::UserRole,
	EntityId = Qt::UserRole + 1,
	ContextPosition,
	ContextString,
};
}

SpellCheckWidget::SpellCheckWidget(QWidget *parent)
: EditorTabWidget(parent)
, ui(new Ui::SpellCheckWidget)
, m_selectedItem(nullptr)
{
	m_idName = "";
	m_hunspell = MainWindow::instance().getHunspell();
	ui->setupUi(this);
	load();

	ui->widget->setVisible(false);
	startTimer(100);
}

SpellCheckWidget::~SpellCheckWidget()
{
	delete ui;
}

QString SpellCheckWidget::tabText() const
{
	return "Spell Check";
}

EditorTabWidget::Type SpellCheckWidget::getType() const
{
	return EditorTabWidget::SpellCheck;
}

void SpellCheckWidget::on_buttonSearchAgain_clicked()
{
	loadData();
}

void SpellCheckWidget::saveData() const
{
	//
}

void SpellCheckWidget::loadData()
{
	auto checkProps = true;
	ui->treeWidget->clear();
	checkEntities(NovelTea::Cutscene::id, "Cutscenes", checkProps);
	checkEntities(NovelTea::Dialogue::id, "Dialogues", checkProps);
	checkEntities(NovelTea::Object::id, "Objects", checkProps);
	checkEntities(NovelTea::Room::id, "Rooms", checkProps);
}

void SpellCheckWidget::processPropJson(QTreeWidgetItem *treeItem, const sj::JSON &jprops)
{
	for (auto &jprop : jprops.ObjectRange())
	{
		auto ok = false;
		auto val = jprop.second.ToString(ok);
		if (ok)
			processString(treeItem, QString::fromStdString(val));
	}
}

void SpellCheckWidget::processJavascript(QTreeWidgetItem *treeItem, const QString &s)
{
	QString v;
	QRegExp rx("((\"|')(?:\\\\.|[^\\\\])*\\2)");
	rx.setMinimal(true); // Make the regex lazy
	int pos = 0;    // where we are in the string
	while (pos >= 0) {
		pos = rx.indexIn(s, pos);
		if (pos >= 0) {
			pos += rx.matchedLength();
			v += rx.cap(1);
		}
	}
	processString(treeItem, v);
}

void SpellCheckWidget::processString(QTreeWidgetItem *treeItem, const QString &s)
{
	auto text = s;
	text.replace("\\n", "  ");
	text.replace("\\t", "  ");

	QStringList words;
	auto w = text.split(QRegExp("\\b"));

	for (int i = 1; i < w.size(); i+=2)
	{
		// Contractions
		if (w[i+1] == "'")
		{
			if (i+2 < w.size())
				words.push_back(w[i] + "'" + w[i+2]);
			i += 2;
		} else
			words.push_back(w[i]);
	}

	int info;
	QMap<QString, int> counts;

	for (auto str : words)
	{
		auto word = str.toStdString();
		if (!m_hunspell->spell(word, &info))
		{
			auto subItem = new QTreeWidgetItem(treeItem);
			subItem->setText(0,  str);
			subItem->setData(0, ContextString, s);

			int pos = counts.value(str, 0) + 1;
			counts[str] = pos;
			subItem->setData(0, ContextPosition, pos);
		}
	}
}

void SpellCheckWidget::checkEntities(const std::string &entityId, const QString &name, bool checkProps)
{
	std::shared_ptr<NovelTea::Entity> entity = nullptr;
	auto parentItem = new QTreeWidgetItem;
	auto count = 0;
	auto entities = ProjData[entityId];
	for (auto &entityMap : entities.ObjectRange())
	{
		auto item = new QTreeWidgetItem;

		if (entityId == NovelTea::Cutscene::id) {
			entity = std::make_shared<NovelTea::Cutscene>();
			item->setData(0, TabType, EditorTabWidget::Cutscene);
		}
		else if (entityId == NovelTea::Dialogue::id) {
			entity = std::make_shared<NovelTea::Dialogue>();
			item->setData(0, TabType, EditorTabWidget::Dialogue);
		}
		else if (entityId == NovelTea::Object::id) {
			entity = std::make_shared<NovelTea::Object>();
			item->setData(0, TabType, EditorTabWidget::Object);
		}
		else if (entityId == NovelTea::Room::id) {
			entity = std::make_shared<NovelTea::Room>();
			item->setData(0, TabType, EditorTabWidget::Room);
		}
		else
			throw std::exception();

		entity->fromJson(entityMap.second);
		item->setData(0, EntityId, QString::fromStdString(entity->getId()));

		if (entityId == NovelTea::Cutscene::id) {
			auto cutscene = std::static_pointer_cast<NovelTea::Cutscene>(entity);
			auto &segments = cutscene->segments();
			for (auto &segment : segments) {
				if (segment->type() != NovelTea::CutsceneSegment::Text)
					continue;
				auto textSeg = std::static_pointer_cast<NovelTea::CutsceneTextSegment>(segment);
				auto text = QString::fromStdString(textSeg->getActiveText()->toPlainText(" "));
				processString(item, text);
			}
		}
		else if (entityId == NovelTea::Dialogue::id) {
			auto dialogue = std::static_pointer_cast<NovelTea::Dialogue>(entity);
			auto segments = dialogue->segments();
			for (auto &segment : segments) {
				auto type = segment->getType();
				if (type != NovelTea::DialogueSegment::Text && type != NovelTea::DialogueSegment::Option)
					continue;
				auto text = QString::fromStdString(segment->getTextRaw());
				if (!segment->getScriptedText())
					processString(item, text);
				else {
					processJavascript(item, text);
				}
			}
		}
		else if (entityId == NovelTea::Room::id) {
			auto room = std::static_pointer_cast<NovelTea::Room>(entity);
			processJavascript(item, QString::fromStdString(room->getDescriptionRaw()));
		}

		if (checkProps) {
			auto props = entity->getProperties();
			processPropJson(item, props);
		}

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

void SpellCheckWidget::on_treeWidget_activated(const QModelIndex &index)
{
	if (!index.parent().parent().isValid())
		return;
	auto type = static_cast<EditorTabWidget::Type>(index.parent().data(TabType).toInt());
	auto idName = index.parent().data(EntityId).toString().toStdString();
	MainWindow::instance().addEditorTab(type, idName);
}

void SpellCheckWidget::timerEvent(QTimerEvent *)
{
	checkIndexChange();
}

void SpellCheckWidget::checkIndexChange()
{
	auto index = ui->treeWidget->currentIndex();
	auto selectedItem = ui->treeWidget->currentItem();
	if (!index.isValid() || !index.parent().parent().isValid())
		selectedItem = nullptr;

	if (m_selectedItem != selectedItem)
	{
		m_selectedItem = selectedItem;
		fillWordInfo();
	}
}

void SpellCheckWidget::fillWordInfo()
{
	ui->widget->setVisible(m_selectedItem != nullptr);

	if (m_selectedItem)
	{
		auto word = m_selectedItem->text(0).toStdString();
		auto suggestions = m_hunspell->suggest(word);
		ui->listWidget->clear();
		for (auto &suggestion : suggestions)
		{
			ui->listWidget->addItem(QString::fromStdString(suggestion));
		}

		QTextCursor c;
		QTextCharFormat fmt;
		fmt.setBackground(QBrush(Qt::yellow));
		fmt.setAnchor(true);
		fmt.setAnchorName("x");
		auto pos = m_selectedItem->data(0, ContextPosition).toInt();
		ui->textEdit->setText(m_selectedItem->data(0, ContextString).toString());
		while (pos-- > 0)
			c = ui->textEdit->document()->find(QRegExp(QString::fromStdString("\\b"+word+"\\b")), c);
		c.mergeCharFormat(fmt);
		ui->textEdit->scrollToAnchor("x");
	}
}
