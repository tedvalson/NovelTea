#include "WizardPageProject.hpp"
#include "ui_WizardPageProject.h"
#include "Widgets/MainWindow.hpp"
#include <QMessageBox>
#include <QDir>

WizardPageProject::WizardPageProject(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::WizardPageProject)
{
	ui->setupUi(this);

	registerField("projectName*", ui->lineEditName);
	registerField("projectAuthor*", ui->lineEditAuthor);

	QDir dir(QCoreApplication::applicationDirPath());
	if (dir.cd("../res/templates"))
	{
		dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
		dir.setSorting(QDir::Name | QDir::IgnoreCase);

		for (auto &fileInfo : dir.entryInfoList()) {
			auto item = new QListWidgetItem(fileInfo.fileName());
			item->setData(Qt::UserRole, fileInfo.absoluteFilePath());
			ui->listWidgetTemplates->addItem(item);
		}
	}

	if (ui->listWidgetTemplates->count() == 0)
		ui->groupBoxTemplates->setVisible(false);
}

WizardPageProject::~WizardPageProject()
{
	delete ui;
}

int WizardPageProject::nextId() const
{
	return -1;
}

bool WizardPageProject::validatePage()
{
	auto mainWindow = qobject_cast<MainWindow*>(wizard()->parent());
	if (mainWindow)
	{
		auto project = std::make_shared<NovelTea::ProjectData>();
		if (ui->groupBoxTemplates->isChecked()) {
			auto item = ui->listWidgetTemplates->currentItem();
			if (!item) {
				QMessageBox::warning(this, "No Template Selected", "Please select a template to use.");
				return false;
			}
			auto filename = item->data(Qt::UserRole).toString().toStdString();
			if (!project->loadFromFile(filename)) {
				QMessageBox::critical(this, "Template Error", "Failed to load the selected template. It is invalid.");
				return false;
			}
		}

		project->data()[NovelTea::ID::projectName] = field("projectName").toString().toStdString();
		project->data()[NovelTea::ID::projectAuthor] = field("projectAuthor").toString().toStdString();
		project->clearFilename();
		auto success = mainWindow->loadProject(project);
		if (success)
			mainWindow->addEditorTab(EditorTabWidget::Settings, "");
		return success;
	}

	return false;
}
