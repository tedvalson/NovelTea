#include "MapWidget.hpp"
#include "ui_MapWidget.h"
#include "MainWindow.hpp"
#include "Map/Node.hpp"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Map.hpp>
#include <NovelTea/States/StateEditor.hpp>
#include <QInputDialog>
#include <iostream>
#include <QDebug>

MapWidget::MapWidget(const std::string &idName, QWidget *parent)
: EditorTabWidget(parent)
, ui(new Ui::MapWidget)
{
	m_idName = idName;
	ui->setupUi(this);
	ui->flowView->setScene(&m_scene);
	load();

	ui->preview->setMode(NovelTea::StateEditorMode::Map);
	ui->preview->setFPS(0);

	ui->sidebar->hide();

	m_menu = new QMenu;
	m_menu->addAction(ui->actionChangeRoomName);
	m_menu->addAction(ui->actionEditScript);

	FlowScene* scene = ui->flowView->scene();
	connect(scene, &FlowScene::nodeContextMenu, this, &MapWidget::nodeContextMenu);
}

MapWidget::~MapWidget()
{
	delete m_menu;
	delete ui;
}

QString MapWidget::tabText() const
{
	return QString::fromStdString(idName());
}

EditorTabWidget::Type MapWidget::getType() const
{
    return EditorTabWidget::Map;
}

void MapWidget::saveData() const
{
	auto map = ui->flowView->scene()->toMapEntity();
	if (map)
	{
		map->setId(m_map->getId());
		map->setParentId(m_map->getParentId());
		m_map = map;
		m_map->setProperties(ui->propertyEditor->getValue());
        Proj.set<NovelTea::Map>(m_map, idName());
	}
}

void MapWidget::loadData()
{
	m_map = Proj.get<NovelTea::Map>(idName());

    if (!m_map)
	{
		// Object is new, so show it as modified
		setModified();
        m_map = std::make_shared<NovelTea::Map>();
	}

	FlowScene* scene = ui->flowView->scene();
	scene->clearScene();
	std::vector<Node*> nodes;

	for (auto& room : m_map->getRooms())
	{
		auto& r = room->rect;
		auto& node = scene->createNode();
		auto& ngo = node.nodeGraphicsObject();
		node.setName(QString::fromStdString(room->name));
		node.setHeight(Node::snapValue * r.height);
		node.setWidth(Node::snapValue * r.width);
		ngo.setPos(Node::snapValue * r.left, Node::snapValue * r.top);
		nodes.push_back(&node);
	}
	for (auto& c : m_map->getConnections())
	{
		auto& nodeStart = *nodes[c->roomStart];
		auto& nodeEnd = *nodes[c->roomEnd];
		QPoint portStart(c->portStart.x, c->portStart.y);
		QPoint portEnd(c->portEnd.x, c->portEnd.y);
		scene->createConnection(nodeStart, portStart, nodeEnd, portEnd);
	}

	MODIFIER(scene, &FlowScene::nodeCreated);
	MODIFIER(scene, &FlowScene::nodeDeleted);
	MODIFIER(scene, &FlowScene::nodeResized);
	MODIFIER(scene, &FlowScene::nodeMoved);
	MODIFIER(scene, &FlowScene::connectionCreated);
	MODIFIER(scene, &FlowScene::connectionDeleted);
}

void MapWidget::on_actionChangeRoomName_triggered()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Change Name"),
			tr("Room Name:"), QLineEdit::Normal, m_node->name(), &ok);
	if (ok) {
		m_node->setName(text);
		setModified();
	}
}

void MapWidget::on_actionEditScript_triggered()
{
	ui->sidebar->show();
}

void MapWidget::on_toolButton_clicked()
{
	ui->sidebar->hide();
}

void MapWidget::nodeContextMenu(Node &n, const QPointF &pos)
{
	m_node = &n;
	m_menu->exec(QCursor::pos());
}

void MapWidget::on_tabWidget_currentChanged(int index)
{
	if (ui->tabWidget->currentWidget() == ui->tabPreview)
	{
		m_map = ui->flowView->scene()->toMapEntity();
		auto jdata = json({"event","map", "map",m_map->toJson()});
		ui->preview->processData(jdata);
	}
}
