#ifndef ROOMWIDGET_HPP
#define ROOMWIDGET_HPP

#include "EditorTabWidget.hpp"
#include <NovelTea/Room.hpp>
#include <QWidget>
#include <QMenu>
#include <QStandardItemModel>
#include <QListWidgetItem>
#include <NovelTea/json.hpp>

using json = sj::JSON;

namespace Ui {
class RoomWidget;
}

class QCheckBox;
class ActionSelectWidget;

class RoomWidget : public EditorTabWidget
{
	Q_OBJECT
public:
	explicit RoomWidget(const std::string &idName, QWidget *parent = 0);
	virtual ~RoomWidget();

	QString tabText() const override;
	Type getType() const override;

private slots:
	void on_actionAddObject_triggered();
	void on_actionRemoveObject_triggered();
	void on_scriptEdit_textChanged();
	void on_listWidget_itemPressed(QListWidgetItem *item);
	void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
	void on_actionView_Edit_triggered();
	void on_listWidget_currentRowChanged(int currentRow);

private:
	void onListViewChanged();
	void updateRoom() const;
	void updatePreview();

	void savePathDirection(json &path, const QCheckBox *checkBox, const ActionSelectWidget *actionSelect) const;
	void loadPathDirection(const json &path, QCheckBox *checkBox, ActionSelectWidget *actionSelect);

	void saveData() const override;
	void loadData() override;

	Ui::RoomWidget *ui;

	QStandardItemModel *itemModel;
	QMenu *m_objectMenu;

	std::shared_ptr<NovelTea::Room> m_room;
};

#endif // ROOMWIDGET_HPP
