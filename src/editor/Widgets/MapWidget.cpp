#include "MapWidget.hpp"
#include "ui_MapWidget.h"
#include "MainWindow.hpp"
#include "Map/Node.hpp"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Map.hpp>
#include <QInputDialog>
#include <iostream>
#include <QDebug>

MapWidget::MapWidget(const std::string &idName, QWidget *parent)
: EditorTabWidget(parent)
, ui(new Ui::MapWidget)
{
	m_idName = idName;
	ui->setupUi(this);
	load();

	ui->preview->setFPS(0);
	ui->flowView->setScene(&m_scene);

	ui->sidebar->hide();

	m_menu = new QMenu;
	m_menu->addAction(ui->actionChangeRoomName);
	m_menu->addAction(ui->actionEditScript);

	connect(ui->flowView->scene(), &FlowScene::nodeContextMenu, this, &MapWidget::nodeContextMenu);
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
    if (m_map)
	{
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
}

void MapWidget::on_actionChangeRoomName_triggered()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Change Name"),
			tr("Room Name:"), QLineEdit::Normal, m_node->name(), &ok);
	if (ok)
		m_node->setName(text);
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
