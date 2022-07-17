#include "WizardPageRoom.hpp"
#include "ui_WizardPageRoom.h"
#include "Widgets/MainWindow.hpp"
#include "Widgets/RoomWidget.hpp"

WizardPageRoom::WizardPageRoom(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::WizardPageRoom)
{
	ui->setupUi(this);
	registerField("roomName*", ui->lineEditName);

//	auto jcutscenes = Proj.data()[NT_CUTSCENES];
//	for (int i = 1; i < INT_MAX; ++i)
//	{
//		auto name = "Cutscene " + std::to_string(i);
//		if (!jcutscenes.contains(name))
//		{
//			ui->lineEditName->setText(QString::fromStdString(name));
//			break;
//		}
//	}
}

WizardPageRoom::~WizardPageRoom()
{
	delete ui;
}

int WizardPageRoom::nextId() const
{
	return -1;
}

bool WizardPageRoom::validatePage()
{
	auto name = field("roomName").toString();
	auto mainWindow = qobject_cast<MainWindow*>(wizard()->parent());
	if (mainWindow && mainWindow->validateEntityName(name, EditorTabWidget::Room))
	{
		auto w = new RoomWidget(name.toStdString());
		mainWindow->addEditorTab(w);
	}
	return true;
}
