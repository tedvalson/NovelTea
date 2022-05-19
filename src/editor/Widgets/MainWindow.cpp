#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "TreeItem.hpp"
#include "ActionWidget.hpp"
#include "CutsceneWidget.hpp"
#include "DialogueWidget.hpp"
#include "RoomWidget.hpp"
#include "MapWidget.hpp"
#include "ObjectWidget.hpp"
#include "ScriptWidget.hpp"
#include "VerbWidget.hpp"
#include "SearchWidget.hpp"
#include "SpellCheckWidget.hpp"
#include "TestsWidget.hpp"
#include "ProjectSettingsWidget.hpp"
#include "NovelTeaWidget.hpp"
#include "Wizard/WizardPageActionSelect.hpp"
#include <NovelTea/ProjectData.hpp>
#include <QFileDialog>
#include <QColorDialog>
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

	createMenus();

	readSettings();

	m_process.setInputChannelMode(QProcess::ForwardedInputChannel);
	connect(&m_process, &QProcess::readyReadStandardOutput, this, &MainWindow::onProcessStandardOutput);
	connect(&m_process, &QProcess::readyReadStandardError, this, &MainWindow::onProcessStandardError);
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
		NovelTea::ProjectData project;
		if (project.loadFromFile(filename.toStdString()))
		{
			m_recentProjects.removeAll(filename);
			m_recentProjects.prepend(filename);
			updateRecentProjectList();

			return loadProject(project);
		}
	}

	return false;
}

bool MainWindow::loadProject(const NovelTea::ProjectData &project)
{
	if (!closeProject())
		return false;

	Proj = project;
	treeModel->loadProject(Proj);
	warnIfInvalid();
	setWindowTitle(QString::fromStdString(ProjData[NovelTea::ID::projectName].ToString()) + " - NovelTea Editor");

	for (auto &jtab : ProjData[NovelTea::ID::openTabs].ArrayRange())
		addEditorTab(static_cast<EditorTabWidget::Type>(jtab[0].ToInt()), jtab[1].ToString());
	auto index = ProjData[NovelTea::ID::openTabIndex].ToInt();
	ui->tabWidget->setCurrentIndex(index);

	return true;
}

bool MainWindow::reloadProject()
{
	return loadProject(QString::fromStdString(Proj.filename()));
}

void MainWindow::saveProject()
{
	auto jtabs = sj::Array();
	auto count = ui->tabWidget->count();
	ProjData[NovelTea::ID::openTabIndex] = ui->tabWidget->currentIndex();
	for (int i = 0; i < count; ++i) {
		auto tab = qobject_cast<EditorTabWidget*>(ui->tabWidget->widget(i));
		jtabs.append(sj::Array(static_cast<int>(tab->getType()), tab->idName()));
	}
	ProjData[NovelTea::ID::openTabs] = jtabs;
	ProjData[NovelTea::ID::entityColors] = treeModel->getColorJSON();
	Proj.saveToFile();
}

bool MainWindow::closeProject()
{
	if (!Proj.isLoaded())
		return true;

	if (!reallyWantToClose())
		return false;

	saveProject();
	auto count = ui->tabWidget->count();
	for (int i = 0; i < count; ++i)
		delete qobject_cast<EditorTabWidget*>(ui->tabWidget->widget(0));

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
		auto parent = treeModel->index(type);
		if (treeModel->insertRow(0, parent))
		{
			treeModel->setData(treeModel->index(0, 0, parent), QString::fromStdString(widget->idName()));
			treeModel->setData(treeModel->index(0, 1, parent), static_cast<int>(EditorTabWidget::tabTypeToEntityType(type)));
		}
		widget->save();
		Proj.saveToFile();
	}

	auto index = ui->tabWidget->addTab(widget, widget->getIcon(), widget->tabText());
	ui->tabWidget->setCurrentIndex(index);
	refreshTabs();

	connect(widget, &EditorTabWidget::modified, this, &MainWindow::refreshTabs);
	connect(widget, &EditorTabWidget::saved, this, &MainWindow::refreshTabs);
	connect(widget, &EditorTabWidget::loaded, this, &MainWindow::refreshTabs);
}

void MainWindow::addEditorTab(EditorTabWidget::Type type, const std::string &idName)
{
	if (type == EditorTabWidget::Action)
		addEditorTab(new ActionWidget(idName));
	else if (type == EditorTabWidget::Cutscene)
		addEditorTab(new CutsceneWidget(idName));
	else if (type == EditorTabWidget::Dialogue)
		addEditorTab(new DialogueWidget(idName));
	else if (type == EditorTabWidget::Map)
		addEditorTab(new MapWidget(idName));
	else if (type == EditorTabWidget::Object)
		addEditorTab(new ObjectWidget(idName));
	else if (type == EditorTabWidget::Room)
		addEditorTab(new RoomWidget(idName));
	else if (type == EditorTabWidget::Script)
		addEditorTab(new ScriptWidget(idName));
	else if (type == EditorTabWidget::Verb)
		addEditorTab(new VerbWidget(idName));
	else if (type == EditorTabWidget::Settings)
		addEditorTab(new ProjectSettingsWidget);
	else if (type == EditorTabWidget::Tests)
		addEditorTab(new TestsWidget);
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

void MainWindow::launchPreview(NovelTea::EntityType entityType, const std::string &entityId, json jMetaData)
{
	if (Proj.filename().empty()) {
		QMessageBox::warning(this, "Cannot Play", "You need to save the project before you can play it.");
		return;
	}
	if (m_process.state() == QProcess::Running) {
		auto r = QMessageBox::warning(this, "Already Running", "Game preview is already running.\nWould you like to close it and proceed?",
				QMessageBox::Close | QMessageBox::Cancel);
		if (r == QMessageBox::Cancel)
			return;
		m_process.terminate();
		m_process.waitForFinished();
	}

	auto launcherPath = QCoreApplication::applicationDirPath() + "/NovelTeaLauncher";
	QStringList args;
	args << QString::fromStdString(Proj.filename());
	if (entityType != NovelTea::EntityType::Invalid)
	{
		args << "entity";
		args << QString::number(static_cast<int>(entityType));
		args << QString::fromStdString(entityId);
		if (entityType == NovelTea::EntityType::Dialogue)
		{
			if (jMetaData.size() == 2)
				args << QString::number(jMetaData[1].ToInt());
		}
	}
	m_process.start(launcherPath, args);
}

void MainWindow::launchPreview()
{
	launchPreview(NovelTea::EntityType::Invalid, "", json());
}

std::string MainWindow::getEntityIdFromTabType(EditorTabWidget::Type type)
{
	if (type == EditorTabWidget::Action)
		return NovelTea::Action::id;
	else if (type == EditorTabWidget::Cutscene)
		return NovelTea::Cutscene::id;
	else if (type == EditorTabWidget::Dialogue)
		return NovelTea::Dialogue::id;
	else if (type == EditorTabWidget::Map)
		return NovelTea::Map::id;
	else if (type == EditorTabWidget::Object)
		return NovelTea::Object::id;
	else if (type == EditorTabWidget::Room)
		return NovelTea::Room::id;
	else if (type == EditorTabWidget::Script)
		return NovelTea::Script::id;
	else if (type == EditorTabWidget::Verb)
		return NovelTea::Verb::id;
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

	m_recentProjects = settings.value("recentProjects").toStringList();
	updateRecentProjectList();
}

void MainWindow::writeSettings()
{
	QSettings settings;
	settings.beginGroup("window");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("state", saveState());
	settings.setValue("splitter", ui->splitter->saveState());
	settings.endGroup();

	settings.setValue("recentProjects", m_recentProjects);
}

void MainWindow::onProcessStandardError()
{
	std::cout << "ERROR: " << QString(m_process.readAllStandardError()).toStdString();
}

void MainWindow::onProcessStandardOutput()
{
	std::cout << "OUT: " << QString(m_process.readAllStandardOutput()).toStdString();
}

QAction *MainWindow::makeColorAction(const QString &string, const QColor &color)
{
	QPixmap pixmap(32, 32);
	pixmap.fill(color);
	auto action = new QAction(QIcon(pixmap), string, this);
	action->setData(color);
	connect(action, SIGNAL(triggered()), this, SLOT(actionSetColorTriggered()));
	return action;
}

void MainWindow::setColorOfSelected(const QColor &color)
{
	auto entityType = EditorTabWidget::tabTypeToEntityType(selectedType);
	auto selectedIndex = ui->treeView->mapToSource(ui->treeView->currentIndex());
	treeModel->setColor(selectedIndex, color);
	saveProject();
	emit entityColorChanged(entityType, selectedIdName, color);
}

void MainWindow::createMenus()
{
	auto menuSetColor = new QMenu("Set Color");
	menuSetColor->addAction(ui->actionClearColor);
	menuSetColor->addSeparator();
	menuSetColor->addAction(makeColorAction("Red", "#FF5555"));
	menuSetColor->addAction(makeColorAction("Green", Qt::green));
	menuSetColor->addAction(makeColorAction("Blue", "#5555FF"));
	menuSetColor->addAction(makeColorAction("Cyan", Qt::cyan));
	menuSetColor->addAction(makeColorAction("Magenta", Qt::magenta));
	menuSetColor->addAction(makeColorAction("Yellow", Qt::yellow));
	menuSetColor->addAction(makeColorAction("Gray", Qt::gray));
	menuSetColor->addAction(makeColorAction("Light Gray", Qt::lightGray));
	menuSetColor->addSeparator();
	menuSetColor->addAction(ui->actionCustomColor);

	menuTreeView->addAction(ui->actionOpen);
	menuTreeView->addAction(ui->actionRename);
	menuTreeView->addMenu(menuSetColor);
	menuTreeView->addAction(ui->actionSelectParent);
	menuTreeView->addAction(ui->actionClearParentSelection);
	menuTreeView->addAction(ui->actionDelete);

	for (int i = 0; i < MaxRecentProjects; ++i) {
		auto action = new QAction(this);
		ui->menuRecentProjects->insertAction(ui->actionClearList, action);
		connect(action, SIGNAL(triggered()), this, SLOT(openRecentProject()));
		m_recentProjectActions[i] = action;
	}
	ui->menuRecentProjects->insertSeparator(ui->actionClearList);
}

void MainWindow::updateRecentProjectList()
{
	while (m_recentProjects.size() > MaxRecentProjects)
		m_recentProjects.removeLast();

	int count = qMin(m_recentProjects.size(), static_cast<int>(MaxRecentProjects));

	for (int i = 0; i < count; ++i) {
		auto action = m_recentProjectActions[i];
		auto text = tr("&%1 %2").arg(i + 1).arg(m_recentProjects[i]);
		action->setText(text);
		action->setData(m_recentProjects[i]);
		action->setVisible(true);
	}
	for (int i = count; i < MaxRecentProjects; ++i)
		m_recentProjectActions[i]->setVisible(false);

	ui->menuRecentProjects->setEnabled(count > 0);
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

void MainWindow::actionSetColorTriggered()
{
	auto action = qobject_cast<QAction*>(sender());
	if (action)
		setColorOfSelected(action->data().value<QColor>());
}

void MainWindow::openRecentProject()
{
	auto action = qobject_cast<QAction*>(sender());
	if (action)
		loadProject(action->data().toString());
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{

}

void MainWindow::on_treeView_activated(const QModelIndex &index)
{
	if (!index.parent().isValid())
		return;

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
	if (QApplication::mouseButtons() != Qt::RightButton)
		return;
	if (!index.parent().isValid())
		return;
	auto proxyIndex = ui->treeView->mapToSource(index);
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
	addEditorTab(new ProjectSettingsWidget);
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
	auto widget = ui->tabWidget->currentWidget();
	auto editorWidget = qobject_cast<EditorTabWidget*>(widget);
	if (editorWidget)
	{
		editorWidget->save();
		warnIfInvalid();
	}

	if (Proj.filename().empty())
	{
		ui->actionSaveAs->trigger();
		return;
	}

	Proj.saveToFile();
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

	m_recentProjects.removeAll(QString::fromStdString(Proj.filename()));
	m_recentProjects.prepend(fileName);
	updateRecentProjectList();

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
	for (int i = 0; i < ui->tabWidget->count(); ++i)
	{
		auto widget = qobject_cast<EditorTabWidget*>(ui->tabWidget->widget(i));
		if (widget && widget->isModified())
		{
			auto msg = "Before renaming entities, you need to save current changes.\nWould you like to do that now?";
			auto ret = QMessageBox::warning(this, "Save Project Changes?", QString::fromStdString(msg), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (ret != QMessageBox::Yes)
				return;
			saveProject();
			break;
		}
	}

	bool ok;
	QString text = QInputDialog::getText(this, tr("Rename"),
			tr("Please enter a new name:"), QLineEdit::Normal,
			QString::fromStdString(selectedIdName), &ok);
	if (ok && !text.isEmpty())
	{
		auto newName = text.toStdString();
		auto existingOldIndex = getEditorTabIndex(selectedType, selectedIdName);
		auto existingNewIndex = getEditorTabIndex(selectedType, newName);
		auto entityId = getEntityIdFromTabType(selectedType);
		auto entityType = EditorTabWidget::tabTypeToEntityType(selectedType);

		if (existingNewIndex >= 0 || ProjData[entityId].hasKey(newName))
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
		Proj.renameEntity(entityType, selectedIdName, newName);
		Proj.saveToFile();

		treeModel->rename(selectedType, QString::fromStdString(selectedIdName), QString::fromStdString(newName));

		emit renamed(entityType, selectedIdName, newName);

		saveProject();
	}
}

void MainWindow::on_actionOpen_triggered()
{
	int existingIndex = getEditorTabIndex(selectedType, selectedIdName);
	if (existingIndex >= 0)
		ui->tabWidget->setCurrentIndex(existingIndex);
	else
		addEditorTab(selectedType, selectedIdName);
}

void MainWindow::on_actionDelete_triggered()
{
	auto msg = "Are you sure you want to remove entity:\n\"" + selectedIdName + "\"";
	auto ret = QMessageBox::warning(this, "Delete Entity", QString::fromStdString(msg), QMessageBox::Cancel | QMessageBox::Ok, QMessageBox::Cancel);
	if (ret == QMessageBox::Cancel)
		return;

	QModelIndex index = ui->treeView->selectionModel()->currentIndex();

	auto &j = ProjData[getEntityIdFromTabType(selectedType)];
	j.erase(selectedIdName);
	Proj.saveToFile();

	auto proxyIndex = ui->treeView->mapToSource(index);
	treeModel->removeRow(proxyIndex.row(), proxyIndex.parent());
}

void MainWindow::on_actionCloseProject_triggered()
{
	closeProject();
}

void MainWindow::on_actionPlayGame_triggered()
{
	if (ui->tabWidget->currentIndex() != -1) {
		auto widget = qobject_cast<EditorTabWidget*>(ui->tabWidget->currentWidget());
		auto type = EditorTabWidget::tabTypeToEntityType(widget->getType());
		launchPreview(type, widget->idName(), json());
	} else
		launchPreview();
}

void MainWindow::on_actionSelectParent_triggered()
{
	QWizard wizard;
	auto page = new WizardPageActionSelect;

	if (selectedType == EditorTabWidget::Action)
		page->setFilterRegExp("Actions");
	else if (selectedType == EditorTabWidget::Cutscene)
		page->setFilterRegExp("Cutscenes");
	else if (selectedType == EditorTabWidget::Dialogue)
		page->setFilterRegExp("Dialogues");
	else if (selectedType == EditorTabWidget::Map)
		page->setFilterRegExp("Maps");
	else if (selectedType == EditorTabWidget::Object)
		page->setFilterRegExp("Objects");
	else if (selectedType == EditorTabWidget::Room)
		page->setFilterRegExp("Rooms");
	else if (selectedType == EditorTabWidget::Script)
		page->setFilterRegExp("Scripts");
	else if (selectedType == EditorTabWidget::Verb)
		page->setFilterRegExp("Verbs");

	page->allowCustomScript(false);
	wizard.addPage(page);

	if (wizard.exec() == QDialog::Accepted)
	{
		auto child = ui->treeView->mapToSource(ui->treeView->currentIndex());
		auto newParent = page->getSelectedIndex();
		auto newParentItem = static_cast<TreeItem*>(newParent.internalPointer());
		if (treeModel->changeParent(child, newParent))
		{
			auto newParentId = newParentItem->data(0).toString().toStdString();
			auto &j = ProjData[getEntityIdFromTabType(selectedType)];
			j[selectedIdName][NovelTea::ID::entityParentId] = newParentId;
			Proj.saveToFile();
		}
	}
}

void MainWindow::on_actionClearParentSelection_triggered()
{
	auto child = ui->treeView->mapToSource(ui->treeView->currentIndex());
	auto parent = child.parent();
	while (parent.parent().isValid())
		parent = parent.parent();
	if (treeModel->changeParent(child, parent))
	{
		auto &j = ProjData[getEntityIdFromTabType(selectedType)];
		j[selectedIdName][NovelTea::ID::entityParentId] = "";
		Proj.saveToFile();
	}
}

void MainWindow::on_actionTests_triggered()
{
	addEditorTab(new TestsWidget);
}

void MainWindow::on_actionSearch_triggered()
{
	QString searchTerm = QInputDialog::getText(this, tr("Search"),
			tr("Enter Search Term:"), QLineEdit::Normal);
	if (searchTerm.isEmpty())
		return;
	addEditorTab(new SearchWidget(searchTerm.toStdString()));
}

void MainWindow::on_actionClearList_triggered()
{
	m_recentProjects.clear();
	updateRecentProjectList();
}

void MainWindow::on_actionCustomColor_triggered()
{
	auto color = QColorDialog::getColor();
	setColorOfSelected(color);
}

void MainWindow::on_actionClearColor_triggered()
{
	setColorOfSelected(QColor());
}

void MainWindow::on_actionSpellCheck_triggered()
{
	addEditorTab(new SpellCheckWidget);
}
