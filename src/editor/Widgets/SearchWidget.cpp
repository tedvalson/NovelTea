#include "SearchWidget.hpp"
#include "ui_SearchWidget.h"
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Room.hpp>

SearchWidget::SearchWidget(const std::string &searchTerm, QWidget *parent)
: EditorTabWidget(parent)
, ui(new Ui::SearchWidget)
, m_searchTerm(searchTerm)
{
	m_idName = "Search: " + searchTerm;
	ui->setupUi(this);
	load();
}

SearchWidget::~SearchWidget()
{
	delete ui;
}

QString SearchWidget::tabText() const
{
	return QString::fromStdString(idName());
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
	ui->treeWidget->clear();

	searchRooms();
}

void SearchWidget::searchRooms()
{
	auto item = new QTreeWidgetItem;
	item->setText(0, "Rooms");

	auto rooms = ProjData[NovelTea::Room::id];
	for (auto &roomMap : rooms.ObjectRange())
	{
		auto room = std::make_shared<NovelTea::Room>();
		room->fromJson(roomMap.second);

		auto subItem = new QTreeWidgetItem(item);
		subItem->setText(0, QString::fromStdString(room->getId()));
	}

	ui->treeWidget->addTopLevelItem(item);
}
