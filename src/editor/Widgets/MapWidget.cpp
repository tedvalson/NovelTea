#include "MapWidget.hpp"
#include "ui_MapWidget.h"
#include "MainWindow.hpp"
#include "Map/Node.hpp"
#include "../Wizard/WizardPageActionSelect.hpp"
#include <NovelTea/Game.hpp>
#include <NovelTea/Map.hpp>
#include <NovelTea/States/StateEditor.hpp>
#include <QInputDialog>
#include <iostream>
#include <QDebug>

MapWidget::MapWidget(const std::string &idName, QWidget *parent)
: EditorTabWidget(parent)
, ui(new Ui::MapWidget)
, m_node(nullptr)
, m_pendingNode(nullptr)
, m_connection(nullptr)
, m_pendingConnection(nullptr)
{
	m_idName = idName;
	ui->setupUi(this);
	ui->flowView->setScene(&m_scene);
	load();

	ui->preview->setMode(NovelTea::StateEditorMode::Map);
	ui->preview->setFPS(2);

	ui->sidebar->hide();

	m_menu = new QMenu;
	m_menu->addAction(ui->actionChangeRoomName);
	m_menu->addAction(ui->actionEditScript);

	FlowScene* scene = ui->flowView->scene();
	connect(scene, &FlowScene::nodeContextMenu, this, &MapWidget::nodeContextMenu);
	connect(scene, &FlowScene::connectionContextMenu, this, &MapWidget::connectionContextMenu);
	connect(scene, &FlowScene::selectionChanged, this, &MapWidget::selectionChanged);
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
	updateSelectedObject();
	updateMap();
	if (m_map)
		Proj->set(m_map, idName());
}

void MapWidget::loadData()
{
	m_map = Proj->get<NovelTea::Map>(idName(), getContext());

    if (!m_map)
	{
		// Object is new, so show it as modified
		setModified();
		m_map = std::make_shared<NovelTea::Map>(getContext());
	}

	ui->scriptRoomDefault->setPlainText(QString::fromStdString(m_map->getDefaultRoomScript()));
	ui->scriptPathDefault->setPlainText(QString::fromStdString(m_map->getDefaultPathScript()));

	FlowScene* scene = ui->flowView->scene();
	scene->clearScene();
	std::vector<Node*> nodes;

	for (auto& room : m_map->getRooms())
	{
		auto& r = room->rect;
		auto& node = scene->createNode();
		auto& ngo = node.nodeGraphicsObject();
		node.setName(QString::fromStdString(room->name));
		node.setScript(room->script);
		node.setRoomIds(room->roomIds);
		node.setHeight(Node::snapValue * r.height);
		node.setWidth(Node::snapValue * r.width);
		node.setStyle(room->style);
		ngo.setPos(Node::snapValue * r.left, Node::snapValue * r.top);
		nodes.push_back(&node);
	}
	for (auto& c : m_map->getConnections())
	{
		auto& nodeStart = *nodes[c->roomStart];
		auto& nodeEnd = *nodes[c->roomEnd];
		QPoint portStart(c->portStart.x, c->portStart.y);
		QPoint portEnd(c->portEnd.x, c->portEnd.y);
		auto connection = scene->createConnection(nodeStart, portStart, nodeEnd, portEnd);
		connection->setScript(c->script);
		connection->setStyle(c->style);
	}

	MODIFIER(scene, &FlowScene::nodeCreated);
	MODIFIER(scene, &FlowScene::nodeDeleted);
	MODIFIER(scene, &FlowScene::nodeResized);
	MODIFIER(scene, &FlowScene::nodeMoved);
	MODIFIER(scene, &FlowScene::connectionCreated);
	MODIFIER(scene, &FlowScene::connectionDeleted);
	MODIFIER(ui->scriptEdit, &ScriptEdit::textChanged);
	MODIFIER(ui->scriptRoomDefault, &ScriptEdit::textChanged);
	MODIFIER(ui->scriptPathDefault, &ScriptEdit::textChanged);
}

void MapWidget::nodeContextMenu(Node &n, const QPointF &pos)
{
	m_pendingNode = &n;
	ui->actionChangeRoomName->setVisible(true);
	m_menu->exec(QCursor::pos());
}

void MapWidget::connectionContextMenu(Connection &c, const QPointF &pos)
{
	m_pendingConnection = &c;
	ui->actionChangeRoomName->setVisible(false);
	m_menu->exec(QCursor::pos());
}

void MapWidget::selectionChanged()
{
	m_pendingConnection = nullptr;
	m_pendingNode = nullptr;
	ui->listRooms->hide();
	ui->toolBar->hide();
	ui->labelPathStyle->hide();
	ui->labelRoomStyle->hide();
	ui->comboPathStyle->hide();
	ui->comboRoomStyle->hide();
	ui->sidebar->hide();
}

void MapWidget::on_actionChangeRoomName_triggered()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Change Name"),
			tr("Room Name:"), QLineEdit::Normal, m_pendingNode->name(), &ok);
	if (ok) {
		m_pendingNode->setName(text);
		setModified();
	}
}

void MapWidget::on_actionEditScript_triggered()
{
	// TODO: check for deleted objects / invalid ptr
	updateSelectedObject();
	ui->scriptEdit->blockSignals(true);
	ui->listRooms->blockSignals(true);

	m_node = m_pendingNode;
	m_connection = m_pendingConnection;

	ui->sidebar->show();
	if (m_node) {
		ui->listRooms->clear();
		for (auto& roomId : m_node->getRoomIds())
			ui->listRooms->addItem(QString::fromStdString(roomId));
		ui->scriptEdit->setPlainText(QString::fromStdString(m_node->getScript()));
		ui->comboRoomStyle->setCurrentIndex(static_cast<int>(m_node->getStyle()));
		ui->listRooms->show();
		ui->labelRoomStyle->show();
		ui->comboRoomStyle->show();
		ui->toolBar->show();
	}
	else if (m_connection) {
		ui->labelPathStyle->show();
		ui->comboPathStyle->show();
		ui->scriptEdit->setPlainText(QString::fromStdString(m_connection->getScript()));
	}

	ui->scriptEdit->blockSignals(false);
	ui->listRooms->blockSignals(false);
}

void MapWidget::on_toolButton_clicked()
{
	ui->sidebar->hide();
}

void MapWidget::on_tabWidget_currentChanged(int index)
{
	if (ui->tabWidget->currentWidget() == ui->tabPreview)
	{
		updateSelectedObject();
		updateMap();
		auto jdata = json({"event","map", "map",m_map->toJson()});
		ui->preview->processData(jdata);
	}
}

void MapWidget::on_actionAttachRoom_triggered()
{
	QWizard wizard;
	auto page = new WizardPageActionSelect;

	page->setFilterRegExp("Rooms");
	page->allowCustomScript(false);

	wizard.addPage(page);

	if (wizard.exec() == QDialog::Accepted)
	{
		auto jval = page->getValue();
		auto idName = QString::fromStdString(jval[NovelTea::ID::selectEntityId].ToString());
		// Check if object already exists
		for (int i = 0; i < ui->listRooms->count(); ++i)
		{
			auto item = ui->listRooms->item(i);
			if (item->text() == idName)
				return;
		}

		auto item = new QListWidgetItem(idName);
		ui->listRooms->addItem(item);
		setModified();
	}
}

void MapWidget::on_actionDetachRoom_triggered()
{
	delete ui->listRooms->currentItem();
	setModified();
}

void MapWidget::on_listRooms_currentRowChanged(int currentRow)
{
	ui->actionDetachRoom->setEnabled(currentRow >= 0);
}

void MapWidget::updateMap() const
{
	auto map = ui->flowView->scene()->toMapEntity(getContext());
	if (map)
	{
		map->setId(m_map->getId());
		map->setParentId(m_map->getParentId());
		m_map = map;
		m_map->setProperties(ui->propertyEditor->getValue());
		m_map->setDefaultRoomScript(ui->scriptRoomDefault->toPlainText().toStdString());
		m_map->setDefaultPathScript(ui->scriptPathDefault->toPlainText().toStdString());
	}
}

void MapWidget::updateSelectedObject() const
{
	if (m_node) {
		std::vector<std::string> roomIds;
		for (int i = 0; i < ui->listRooms->count(); ++i)
			roomIds.push_back(ui->listRooms->item(i)->text().toStdString());
		m_node->setScript(ui->scriptEdit->toPlainText().toStdString());
		m_node->setStyle(static_cast<NovelTea::RoomStyle>(ui->comboRoomStyle->currentIndex()));
		m_node->setRoomIds(roomIds);
	}
	else if (m_connection) {
		m_connection->setScript(ui->scriptEdit->toPlainText().toStdString());
	}
}
