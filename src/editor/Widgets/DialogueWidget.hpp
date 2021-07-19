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

protected:
	void timerEvent(QTimerEvent*) override;

private slots:
	void on_treeView_pressed(const QModelIndex &index);
	void on_actionAddObject_triggered();
	void on_actionDelete_triggered();
	void on_actionPlayFromHere_triggered();
	void on_radioText_toggled(bool checked);
	void on_plainTextEdit_textChanged();
	void on_checkBoxConditional_toggled(bool checked);
	void on_checkBoxScript_toggled(bool checked);
	void on_actionCut_triggered();
	void on_actionCopy_triggered();
	void on_actionPaste_triggered();
	void on_actionPasteAsLink_triggered();
	void on_actionMoveUp_triggered();
	void on_actionMoveDown_triggered();
	void on_lineEditDefaultName_textChanged(const QString &arg1);
	void on_treeView_expanded(const QModelIndex &index);
	void on_treeView_collapsed(const QModelIndex &index);
	void on_checkBoxShowDisabled_toggled(bool checked);

private:
	void fillItemSettings();
	void checkIndexChange();
	void saveData() const override;
	void loadData() override;

	Ui::DialogueWidget *ui;

	DialogueTreeModel *m_treeModel;
	QMenu *m_menuTreeView;

	QModelIndex m_cutIndex;
	QModelIndex m_copyIndex;

	DialogueTreeItem *m_selectedItem;

	std::shared_ptr<NovelTea::Dialogue> m_dialogue;
};

#endif // DIALOGUEWIDGET_HPP
