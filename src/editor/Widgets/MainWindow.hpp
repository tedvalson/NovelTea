#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "EditorTabWidget.hpp"
#include "TreeModel.hpp"
#include <QMainWindow>
#include <QSortFilterProxyModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	static MainWindow &instance();

	bool loadProject(const QString &filename);
	bool reloadProject();
	bool closeProject();

	void addEditorTab(EditorTabWidget *widget, bool checkForExisting = false);
	int getEditorTabIndex(EditorTabWidget::Type type, const std::string &idName) const;
	void warnIfInvalid() const;

	QAbstractItemModel *getItemModel() const;

protected:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void closeEvent(QCloseEvent *event);
	bool reallyWantToClose();
	void readSettings();
	void writeSettings();

public slots:
	void refreshTabs();

private slots:
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

private:
	static MainWindow *_instance;
    Ui::MainWindow *ui;
	TreeModel *treeModel;
	QSortFilterProxyModel *proxyModel;
	QMenu *menuTreeView;
	std::string selectedIdName;
	EditorTabWidget::Type selectedType;
};

#endif // MAINWINDOW_H
