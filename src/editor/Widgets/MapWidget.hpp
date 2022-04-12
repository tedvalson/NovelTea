#ifndef MAPWIDGET_HPP
#define MAPWIDGET_HPP

#include "EditorTabWidget.hpp"
#include <NovelTea/Map.hpp>
#include <QWidget>
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
	void on_pushButton_clicked();

private:
	void saveData() const override;
	void loadData() override;

	Ui::MapWidget *ui;

	std::shared_ptr<NovelTea::Map> m_map;
};

#endif // MapWIDGET_HPP
