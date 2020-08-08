#ifndef DIALOGUEWIDGET_HPP
#define DIALOGUEWIDGET_HPP

#include "EditorTabWidget.hpp"
#include "DialogueTreeModel.hpp"
#include <NovelTea/Dialogue.hpp>
#include <QWidget>
#include <QMenu>
#include <NovelTea/json.hpp>

using json = sj::JSON;

namespace Ui {
class DialogueWidget;
}

class DialogueWidget : public EditorTabWidget
{
	Q_OBJECT
public:
	explicit DialogueWidget(const std::string &idName, QWidget *parent = 0);
	virtual ~DialogueWidget();

	QString tabText() const override;
	Type getType() const override;

private slots:
	void on_treeView_pressed(const QModelIndex &index);
	void on_actionAddObject_triggered();

private:
	void saveData() const override;
	void loadData() override;

	Ui::DialogueWidget *ui;

	DialogueTreeModel *m_treeModel;
	DialogueTreeItem *m_selectedItem;
	QMenu *m_menuTreeView;

	std::shared_ptr<NovelTea::Dialogue> m_dialogue;
};

#endif // DIALOGUEWIDGET_HPP
