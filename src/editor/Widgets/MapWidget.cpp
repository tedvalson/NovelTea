#include "MapWidget.hpp"
#include "ui_MapWidget.h"
#include "MainWindow.hpp"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Map.hpp>
#include <iostream>
#include <QDebug>

MapWidget::MapWidget(const std::string &idName, QWidget *parent)
: EditorTabWidget(parent)
, ui(new Ui::MapWidget)
{
	m_idName = idName;
	ui->setupUi(this);
	load();
}

MapWidget::~MapWidget()
{
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

void MapWidget::on_pushButton_clicked()
{
}
