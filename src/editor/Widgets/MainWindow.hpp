#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "EditorTabWidget.hpp"
#include "TreeModel.hpp"
#include <QMainWindow>
#include <QProcess>

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
	bool loadProject(std::shared_ptr<NovelTea::ProjectData> project);
	bool reloadProject();
	void saveProject();
	bool closeProject();

	std::shared_ptr<NovelTea::ProjectData> getProject() const { return m_project; }
	std::shared_ptr<NovelTea::ProjectData> getProjectBackup() const { return m_projectBackup; }

	void addEditorTab(EditorTabWidget *widget, bool checkForExisting = true);
	void addEditorTab(EditorTabWidget::Type type, const std::string &idName);
	int getEditorTabIndex(EditorTabWidget::Type type, const std::string &idName) const;
	void warnIfInvalid() const;

	bool validateEntityName(const QString &entityIdName, EditorTabWidget::Type type, bool checkForCollision = true);

	void launchPreview(NovelTea::EntityType entityType, const std::string &entityId, json jMetaData);
	void launchPreview();

	std::string getEntityIdFromTabType(EditorTabWidget::Type type);

	QAbstractItemModel *getItemModel() const;

protected:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void closeEvent(QCloseEvent *event);
	bool reallyWantToClose();
	void readSettings();
	void writeSettings();

	void onProcessStandardError();
	void onProcessStandardOutput();

private:
	QAction *makeColorAction(const QString &string, const QColor &color);
	void setColorOfSelected(const QColor &color);
	void createMenus();
	void updateRecentProjectList();

signals:
	void renamed(NovelTea::EntityType entityType, const std::string &oldValue, const std::string &newValue);
	void entityColorChanged(NovelTea::EntityType entityType, const std::string &entityId, const QColor &color);

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
	void on_actionShaders_triggered();
	void on_actionClearList_triggered();
	void on_actionCustomColor_triggered();
	void on_actionClearColor_triggered();
	void on_actionSpellCheck_triggered();
	void on_actionCopyAs_triggered();
	void on_actionViewToolBar_toggled(bool checked);
	void on_actionViewStatusBar_toggled(bool checked);

private:
	static MainWindow *_instance;
    Ui::MainWindow *ui;
	TreeModel *treeModel;
	QMenu *menuTreeView;
	std::string selectedIdName;
	EditorTabWidget::Type selectedType;

	QStringList m_recentProjects;
	QAction *m_recentProjectActions[MaxRecentProjects];

	std::shared_ptr<NovelTea::ProjectData> m_project;
	std::shared_ptr<NovelTea::ProjectData> m_projectBackup;

	QProcess m_process;
};

#endif // MAINWINDOW_H
