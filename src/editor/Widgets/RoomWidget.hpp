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

protected:
	void timerEvent(QTimerEvent *event);

public slots:
	void refreshObjectList();
	void refreshObjectColors();
	void renamed(NovelTea::EntityType entityType, const std::string &oldName, const std::string &newName);

private slots:
	void on_actionAddObject_triggered();
	void on_actionRemoveObject_triggered();
	void on_scriptEdit_textChanged();
	void on_listWidget_itemPressed(QListWidgetItem *item);
	void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
	void on_actionView_Edit_triggered();
	void on_listWidget_currentRowChanged(int currentRow);
	void on_checkBoxBeforeEnter_toggled(bool checked);
	void on_checkBoxAfterEnter_toggled(bool checked);
	void on_checkBoxBeforeLeave_toggled(bool checked);
	void on_checkBoxAfterLeave_toggled(bool checked);

private:
	void updateAll();
	void updateRoom() const;
	void updatePreview();

	void savePathDirection(json &path, const QCheckBox *checkBox, const ActionSelectWidget *actionSelect) const;
	void loadPathDirection(const json &path, QCheckBox *checkBox, ActionSelectWidget *actionSelect);

	void saveData() const override;
	void loadData() override;

	Ui::RoomWidget *ui;
	bool m_previewNeedsUpdate;

	QMenu *m_objectMenu;

	std::shared_ptr<NovelTea::Room> m_room;
};

#endif // ROOMWIDGET_HPP
