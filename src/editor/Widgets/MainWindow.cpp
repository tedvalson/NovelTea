#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "TreeItem.hpp"
#include "ActionWidget.hpp"
#include "CutsceneWidget.hpp"
#include "DialogueWidget.hpp"
#include "RoomWidget.hpp"
#include "ObjectWidget.hpp"
#include "ScriptWidget.hpp"
#include "VerbWidget.hpp"
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
	menuTreeView(new QMenu),
	selectedType(EditorTabWidget::Invalid)
{
	ui->setupUi(this);

	ui->treeView->setModel(treeModel);

	menuTreeView->addAction(ui->actionOpen);
	menuTreeView->addAction(ui->actionRename);
	menuTreeView->addAction(ui->actionDelete);

	readSettings();

	// Load testing project
	loadProject("/home/android/test.ntp");
}

MainWindow::~MainWindow()
{
	delete menuTreeView;
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

bool MainWindow::loadProject(const QString &filename)
{
	if (closeProject() && !filename.isEmpty())
	{
		if (Proj.loadFromFile(filename.toStdString()))
		{
			treeModel->loadProject(Proj);
			warnIfInvalid();
			setWindowTitle(QString::fromStdString(ProjData[NovelTea::ID::projectName]) + " - NovelTea Editor");
			return true;
		}
	}

	return false;
}

bool MainWindow::reloadProject()
{
	loadProject(QString::fromStdString(Proj.filename()));
}

bool MainWindow::closeProject()
{
	if (!Proj.isLoaded())
		return true;

	if (!reallyWantToClose())
		return false;

	auto count = ui->tabWidget->count();
	for (int i = 0; i < count; ++i)
		delete ui->tabWidget->widget(0);

	Proj.closeProject();
	treeModel->loadProject(Proj);
	setWindowTitle("NovelTea Editor");

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
		// TODO: add type properly for context menu?
		auto parent = treeModel->index(type);
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

json &MainWindow::getDataFromTabType(EditorTabWidget::Type type)
{
	if (type == EditorTabWidget::Action)
		return ProjData[NovelTea::Action::id];
	else if (type == EditorTabWidget::Cutscene)
		return ProjData[NovelTea::Cutscene::id];
	else if (type == EditorTabWidget::Dialogue)
		return ProjData[NovelTea::Dialogue::id];
	else if (type == EditorTabWidget::Object)
		return ProjData[NovelTea::Object::id];
	else if (type == EditorTabWidget::Room)
		return ProjData[NovelTea::Room::id];
	else if (type == EditorTabWidget::Script)
		return ProjData[NovelTea::Script::id];
	else if (type == EditorTabWidget::Verb)
		return ProjData[NovelTea::Verb::id];
	else
		throw std::exception();
}

QAbstractItemModel *MainWindow::getItemModel() const
{
	return treeModel;
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
	auto proxyIndex = ui->treeView->mapToSource(index);
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
	auto proxyIndex = ui->treeView->mapToSource(index);
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
	loadProject(fileName);
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
		auto &j = getDataFromTabType(selectedType);

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
	else if (selectedType == EditorTabWidget::Action)
		addEditorTab(new ActionWidget(selectedIdName));
	else if (selectedType == EditorTabWidget::Cutscene)
		addEditorTab(new CutsceneWidget(selectedIdName));
	else if (selectedType == EditorTabWidget::Room)
		addEditorTab(new RoomWidget(selectedIdName));
	else if (selectedType == EditorTabWidget::Object)
		addEditorTab(new ObjectWidget(selectedIdName));
	else if (selectedType == EditorTabWidget::Verb)
		addEditorTab(new VerbWidget(selectedIdName));
}

void MainWindow::on_actionDelete_triggered()
{
	// TODO: You sure you want to remove this?
	QModelIndex index = ui->treeView->selectionModel()->currentIndex();

	auto &j = getDataFromTabType(selectedType);
	j.erase(selectedIdName);
	Proj.saveToFile();

	auto proxyIndex = ui->treeView->mapToSource(index);
	treeModel->removeRow(proxyIndex.row(), proxyIndex.parent());
}

void MainWindow::on_actionCloseProject_triggered()
{
	closeProject();
}
