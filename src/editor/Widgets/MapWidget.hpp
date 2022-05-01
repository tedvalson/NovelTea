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
	void on_actionChangeRoomName_triggered();
	void on_actionEditScript_triggered();
	void on_toolButton_clicked();
	void nodeContextMenu(Node& n, const QPointF& pos);
	void on_tabWidget_currentChanged(int index);

private:
	void saveData() const override;
	void loadData() override;

	Ui::MapWidget *ui;
	QMenu *m_menu;

	Node *m_node;

	mutable std::shared_ptr<NovelTea::Map> m_map;
	FlowScene m_scene;
};

#endif // MapWIDGET_HPP
