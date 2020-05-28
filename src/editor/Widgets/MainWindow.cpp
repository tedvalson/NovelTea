#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "TreeItem.hpp"
#include "CutsceneWidget.hpp"
#include "RoomWidget.hpp"
#include "ProjectSettingsWidget.hpp"
#include "NovelTeaWidget.hpp"
#include <NovelTea/ProjectData.hpp>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include <QInputDialog>
#include <QSettings>
#include "Wizard/Wizard.hpp"
#include "QtPropertyBrowser/qtpropertymanager.h"
#include "QtPropertyBrowser/qtvariantproperty.h"
#include "QtPropertyBrowser/qttreepropertybrowser.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
	ui(new Ui::MainWindow),
	treeModel(new TreeModel),
	proxyModel(new QSortFilterProxyModel),
	menuTreeView(new QMenu),
	selectedType(EditorTabWidget::Invalid)
{
	ui->setupUi(this);

	proxyModel->setSourceModel(treeModel);
	proxyModel->sort(0);
	ui->treeView->setModel(proxyModel);

	menuTreeView->addAction(ui->actionOpen);
	menuTreeView->addAction(ui->actionRename);
	menuTreeView->addAction(ui->actionTest_2);

	readSettings();

	// Load testing project
	Proj.loadFromFile("/home/android/test.ntp");
	treeModel->loadProject(Proj);
	warnIfInvalid();
}

MainWindow::~MainWindow()
{
	delete menuTreeView;
	delete proxyModel;
	delete treeModel;
	delete ui;
}

MainWindow *MainWindow::_instance = nullptr;

MainWindow &MainWindow::instance()
{
	if (!_instance)
		_instance = new MainWindow;
	return *_instance;
}

bool MainWindow::closeProject()
{
	if (!reallyWantToClose())
		return false;

	auto count = ui->tabWidget->count();
	for (int i = 0; i < count; ++i)
		delete ui->tabWidget->widget(0);

	Proj.closeProject();
	treeModel->loadProject(Proj);

	return true;
}

void MainWindow::addEditorTab(EditorTabWidget *widget, bool checkForExisting)
{
	if (checkForExisting)
	{
		auto index = getEditorTabIndex(widget->getType(), widget->idName());
		if (index >= 0)
		{
			ui->tabWidget->setCurrentIndex(index);
			delete widget;
			return;
		}
	}

	// If added widget is modified when added, then it's new
	if (widget->isModified())
	{
		auto type = widget->getType();
		int row = 0;
		if (type == EditorTabWidget::Room)
			row = 3;

		auto parent = treeModel->index(row, 0);
		if (treeModel->insertRow(0, parent))
		{
			treeModel->setData(treeModel->index(0, 0, parent), QString::fromStdString(widget->idName()));
			treeModel->setData(treeModel->index(0, 1, parent), type);
		}
		widget->save();
		Proj.saveToFile();
	}

	auto index = ui->tabWidget->addTab(widget, widget->tabText());
	ui->tabWidget->setCurrentIndex(index);
	refreshTabs();

	connect(widget, &EditorTabWidget::modified, this, &MainWindow::refreshTabs);
	connect(widget, &EditorTabWidget::saved, this, &MainWindow::refreshTabs);
	connect(widget, &EditorTabWidget::loaded, this, &MainWindow::refreshTabs);
}

int MainWindow::getEditorTabIndex(EditorTabWidget::Type type, const std::string &idName) const
{
	for (int i = 0; i < ui->tabWidget->count(); ++i)
		if (auto tabWidget = qobject_cast<EditorTabWidget*>(ui->tabWidget->widget(i)))
			if (tabWidget->getType() == type && tabWidget->idName() == idName)
				return i;
	return -1;
}

void MainWindow::warnIfInvalid() const
{
	std::string error;
	if (!Proj.isValid(error))
		QMessageBox::critical(this->parentWidget(), "Project is Invalid", QString::fromStdString(error));
}

QAbstractItemModel *MainWindow::getItemModel() const
{
	return proxyModel;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (closeProject())
	{
		writeSettings();
		event->accept();
	}
	else
		event->ignore();
}

bool MainWindow::reallyWantToClose()
{
	warnIfInvalid();

	std::vector<EditorTabWidget*> modifiedWidgets;
	QString detailedText = "The following have been modified:";
	for (int i = 0; i < ui->tabWidget->count(); ++i)
	{
		auto widget = qobject_cast<EditorTabWidget*>(ui->tabWidget->widget(i));
		if (widget && widget->isModified())
		{
			detailedText += "\n" + widget->tabText();
			modifiedWidgets.push_back(widget);
		}
	}
	if (!modifiedWidgets.empty())
	{
		QMessageBox msgBox;
		msgBox.setText("Your project has been modified.");
		msgBox.setInformativeText("Do you want to save your changes?");
		msgBox.setDetailedText(detailedText);
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();
		if (ret == QMessageBox::Save)
		{
			for (auto &widget : modifiedWidgets)
				widget->save();
			Proj.saveToFile();
		}
		else if (ret == QMessageBox::Cancel)
			return false;
	}

	return true;
}

void MainWindow::readSettings()
{
	QSettings settings;
	settings.beginGroup("window");
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("state").toByteArray());
	ui->splitter->restoreState(settings.value("splitter").toByteArray());
	settings.endGroup();
}

void MainWindow::writeSettings()
{
	QSettings settings;
	settings.beginGroup("window");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("state", saveState());
	settings.setValue("splitter", ui->splitter->saveState());
	settings.endGroup();
}

void MainWindow::refreshTabs()
{
	auto currentIndex = ui->tabWidget->currentIndex();
	for (int i = 0; i < ui->tabWidget->count(); ++i)
		if (auto widget = qobject_cast<EditorTabWidget*>(ui->tabWidget->widget(i)))
		{
			auto modified = widget->isModified();
			auto tabText = widget->tabText();
			if (modified)
				tabText += "*";
			ui->tabWidget->setTabText(i, tabText);
			if (i == currentIndex)
				ui->actionSave->setEnabled(modified);
		}
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{

}

void MainWindow::on_treeView_activated(const QModelIndex &index)
{
	auto proxyIndex = proxyModel->mapToSource(index);
	auto item = static_cast<TreeItem*>(proxyIndex.internalPointer());
	auto type = item->data(1);
	if (!type.isValid())
		return;

	selectedType = EditorTabWidget::entityTypeToTabType(static_cast<NovelTea::EntityType>(type.toInt()));
	selectedIdName = item->data(0).toString().toStdString();
	ui->actionOpen->trigger();
}

void MainWindow::on_treeView_pressed(const QModelIndex &index)
{
	qDebug() << "index:" << index.column() << index.row();
	auto proxyIndex = proxyModel->mapToSource(index);
	if (QApplication::mouseButtons() != Qt::RightButton)
		return;
	auto item = static_cast<TreeItem*>(proxyIndex.internalPointer());
	auto type = item->data(1);
	if (!type.isValid())
		return;

	selectedType = EditorTabWidget::entityTypeToTabType(static_cast<NovelTea::EntityType>(type.toInt()));
	selectedIdName = item->data(0).toString().toStdString();

	menuTreeView->popup(QCursor::pos());	
}

void MainWindow::on_actionNewProject_triggered()
{
	Wizard wizard(this);
	wizard.exec();
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
	auto widget = ui->tabWidget->widget(index);
	auto editorTabWidget = qobject_cast<EditorTabWidget*>(widget);

	if (editorTabWidget)
	{
		if (editorTabWidget->isModified())
		{
			QMessageBox msgBox;
			msgBox.setText("\"" + editorTabWidget->tabText() + "\" has been modified.");
			msgBox.setInformativeText("Do you want to save your changes?");
			msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
			msgBox.setDefaultButton(QMessageBox::Save);
			int ret = msgBox.exec();
			if (ret == QMessageBox::Save)
			{
				editorTabWidget->save();
				Proj.saveToFile();
			}
			else if (ret == QMessageBox::Cancel)
				return;
		}
	}

	delete widget;
}

void MainWindow::on_actionProjectSettings_triggered()
{
	auto w = new ProjectSettingsWidget;
	addEditorTab(w);
}

void MainWindow::on_actionOpenProject_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(this,
			tr("Open Project"),
			QString(),
			tr("NovelTea Projects (*.ntp);;All Files (*)"));
	if (!fileName.isEmpty() && closeProject())
	{
		if (Proj.loadFromFile(fileName.toStdString()))
		{
			treeModel->loadProject(Proj);
			warnIfInvalid();
		}
		else
			qDebug() << "Project load failed: " << fileName;
	}
}

void MainWindow::on_actionSave_triggered()
{
	if (Proj.filename().empty())
	{
		ui->actionSaveAs->trigger();
		return;
	}

	auto widget = ui->tabWidget->currentWidget();
	auto editorWidget = qobject_cast<EditorTabWidget*>(widget);
	if (editorWidget)
	{
		editorWidget->save();
		Proj.saveToFile();
		warnIfInvalid();
	}
}

void MainWindow::on_actionSaveAs_triggered()
{
	warnIfInvalid();
	auto fileName = QFileDialog::getSaveFileName(this,
			tr("Save Project"),
			QString(),
			tr("NovelTea Projects (*.ntp);;All Files (*)"));
	if (fileName.isEmpty())
		return;
	if (!fileName.endsWith(".ntp"))
		fileName += ".ntp";
	Proj.saveToFile(fileName.toStdString());
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
	auto widget = ui->tabWidget->widget(index);
	auto editorWidget = qobject_cast<EditorTabWidget*>(widget);
	if (editorWidget)
	{
		ui->actionSave->setEnabled(editorWidget->isModified());
	}
	else
	{
		ui->actionSave->setEnabled(false);
	}
}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
	if (QApplication::mouseButtons() != Qt::RightButton)
		return;

	ui->menuFile->popup(QCursor::pos());
}

void MainWindow::on_actionRename_triggered()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Rename"),
			tr("Please enter a new name:"), QLineEdit::Normal,
			QString::fromStdString(selectedIdName), &ok);
	if (ok && !text.isEmpty())
	{
		auto newName = text.toStdString();
		auto existingOldIndex = getEditorTabIndex(selectedType, selectedIdName);
		auto existingNewIndex = getEditorTabIndex(selectedType, newName);
		auto &j = ProjData[NovelTea::ID::cutscenes];

		if (existingNewIndex >= 0 || j.contains(newName))
		{
			QMessageBox::critical(this, "Failed to rename",
				QString::fromStdString("\"" + newName + "\" already exists."));
			return;
		}

		// Rename the tab, if open
		if (existingOldIndex >= 0)
		{
			auto widget = qobject_cast<EditorTabWidget*>(ui->tabWidget->widget(existingOldIndex));
			widget->rename(newName);
			refreshTabs();
		}

		// Rename in project data, then save to file
		j[newName] = j[selectedIdName];
		j.erase(selectedIdName);
		Proj.saveToFile();

		treeModel->rename(EditorTabWidget::tabTypeToEntityType(selectedType), QString::fromStdString(selectedIdName), QString::fromStdString(newName));
	}
}

void MainWindow::on_actionOpen_triggered()
{
	int existingIndex = getEditorTabIndex(selectedType, selectedIdName);
	if (existingIndex >= 0)
		ui->tabWidget->setCurrentIndex(existingIndex);
	else if (selectedType == EditorTabWidget::Cutscene)
		addEditorTab(new CutsceneWidget(selectedIdName));
	else if (selectedType == EditorTabWidget::Room)
		addEditorTab(new RoomWidget(selectedIdName));
}

void MainWindow::on_actionTest_2_triggered()
{
	qDebug() << "test clicked";
	treeModel->update();
	proxyModel->sort(0);
}

void MainWindow::on_actionCloseProject_triggered()
{
	closeProject();
}
