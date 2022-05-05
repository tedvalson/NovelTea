#ifndef MAPWIDGET_HPP
#define MAPWIDGET_HPP

#include "EditorTabWidget.hpp"
#include "Map/FlowScene.hpp"
#include <NovelTea/Map.hpp>
#include <QWidget>
#include <QMenu>
#include <NovelTea/json.hpp>

using json = sj::JSON;

namespace Ui {
class MapWidget;
}

class MapWidget : public EditorTabWidget
{
	Q_OBJECT
public:
	explicit MapWidget(const std::string &idName, QWidget *parent = 0);
	virtual ~MapWidget();

	QString tabText() const override;
	Type getType() const override;

private slots:
	void nodeContextMenu(Node& n, const QPointF& pos);
	void connectionContextMenu(Connection& c, const QPointF& pos);
	void selectionChanged();
	void on_actionChangeRoomName_triggered();
	void on_actionEditScript_triggered();
	void on_toolButton_clicked();
	void on_tabWidget_currentChanged(int index);
	void on_actionAttachRoom_triggered();
	void on_actionDetachRoom_triggered();
	void on_listRooms_currentRowChanged(int currentRow);

private:
	void updateSelectedObject() const;

	void saveData() const override;
	void loadData() override;

private:
	Ui::MapWidget *ui;
	QMenu *m_menu;

	Node *m_node;
	Connection *m_connection;

	mutable std::shared_ptr<NovelTea::Map> m_map;
	FlowScene m_scene;
};

#endif // MapWIDGET_HPP
