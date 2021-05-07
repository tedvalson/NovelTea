#ifndef TESTSWIDGET_HPP
#define TESTSWIDGET_HPP

#include "EditorTabWidget.hpp"
#include <QWidget>
#include <QMenu>
#include <QStandardItemModel>
#include <NovelTea/json.hpp>
#include <NovelTea/States/StateEditor.hpp>

using json = sj::JSON;

namespace Ui {
class TestsWidget;
}

class TestsWidget : public EditorTabWidget
{
	Q_OBJECT
public:
	explicit TestsWidget(QWidget *parent = 0);
	virtual ~TestsWidget();

	QString tabText() const override;
	Type getType() const override;

	void loadTest(const std::string &testId);
	void addStep(const json &jstep, bool append = false);
	void addStepToList(const json &jstep, bool append = false);

protected:
	void resetListStyle();
	bool processCallbackData(const json &jdata);
	void processSteps(bool startRecording);
	void saveSettings() const;

public slots:
	void renamed(NovelTea::EntityType entityType, const std::string &oldName, const std::string &newName);

private slots:
	void on_actionAddTest_triggered();
	void on_actionRemoveTest_triggered();
	void on_actionRemoveStep_triggered();
	void on_actionAddStepAction_triggered();
	void on_actionAddStepWait_triggered();
	void on_actionAddStepDialogueOption_triggered();
	void on_listWidgetTests_currentRowChanged(int currentRow);
	void on_listWidgetSteps_currentRowChanged(int currentRow);
	void on_actionRunSteps_triggered();
	void on_actionRecordSteps_triggered();
	void on_actionAddObject_triggered();
	void on_actionRemoveObject_triggered();
	void on_listInventory_currentRowChanged(int currentRow);

private:
	void on_rowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
	void saveData() const override;
	void loadData() override;

	Ui::TestsWidget *ui;

	QMenu *m_menuAdd;

	mutable json m_json;
	int m_errorStepIndex;
	std::string m_selectedTestId;
	NovelTea::TestCallback m_callback;
};

#endif // TESTSWIDGET_HPP
