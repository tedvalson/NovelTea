#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "EditorTabWidget.hpp"
#include "TreeModel.hpp"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

	enum { MaxRecentProjects = 5 };

public:
	static MainWindow &instance();

	bool loadProject(const QString &filename);
	bool loadProject(const NovelTea::ProjectData &project);
	bool reloadProject();
	void saveProject();
	bool closeProject();

	void addEditorTab(EditorTabWidget *widget, bool checkForExisting = true);
	void addEditorTab(EditorTabWidget::Type type, const std::string &idName);
	int getEditorTabIndex(EditorTabWidget::Type type, const std::string &idName) const;
	void warnIfInvalid() const;

	std::string getEntityIdFromTabType(EditorTabWidget::Type type);

	QAbstractItemModel *getItemModel() const;

protected:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void closeEvent(QCloseEvent *event);
	bool reallyWantToClose();
	void readSettings();
	void writeSettings();

private:
	QAction *makeColorAction(const QString &string, const QColor &color);
	void setColorOfSelected(const QColor &color);
	void createMenus();
	void updateRecentProjectList();

signals:
	void renamed(NovelTea::EntityType entityType, const std::string &oldValue, const std::string &newValue);

public slots:
	void refreshTabs();

private slots:
	void actionSetColorTriggered();
	void openRecentProject();

	void on_treeView_clicked(const QModelIndex &index);
	void on_treeView_activated(const QModelIndex &index);
	void on_treeView_pressed(const QModelIndex &index);
	void on_actionNewProject_triggered();
	void on_tabWidget_tabCloseRequested(int index);
	void on_actionProjectSettings_triggered();
	void on_actionOpenProject_triggered();
	void on_actionSave_triggered();
	void on_actionSaveAs_triggered();
	void on_tabWidget_currentChanged(int index);
	void on_tabWidget_tabBarClicked(int index);
	void on_actionRename_triggered();
	void on_actionOpen_triggered();
	void on_actionDelete_triggered();
	void on_actionCloseProject_triggered();
	void on_actionPlayGame_triggered();
	void on_actionSelectParent_triggered();
	void on_actionClearParentSelection_triggered();
	void on_actionTests_triggered();
	void on_actionSearch_triggered();
	void on_actionClearList_triggered();
	void on_actionCustomColor_triggered();
	void on_actionClearColor_triggered();

private:
	static MainWindow *_instance;
    Ui::MainWindow *ui;
	TreeModel *treeModel;
	QMenu *menuTreeView;
	std::string selectedIdName;
	EditorTabWidget::Type selectedType;

	QStringList m_recentProjects;
	QAction *m_recentProjectActions[MaxRecentProjects];
};

#endif // MAINWINDOW_H
