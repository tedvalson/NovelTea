#include "RoomWidget.hpp"
#include "ui_RoomWidget.h"
#include "MainWindow.hpp"
#include "ObjectWidget.hpp"
#include "Wizard/WizardPageActionSelect.hpp"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/States/StateEditor.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Player.hpp>
#include <QWizard>
#include <QDebug>

RoomWidget::RoomWidget(const std::string &idName, QWidget *parent)
	: EditorTabWidget(parent)
	, ui(new Ui::RoomWidget)
	, m_objectMenu(new QMenu)
{
	_idName = idName;
	ui->setupUi(this);
	ui->preview->setMode(NovelTea::StateEditorMode::Room);
	load();

	m_objectMenu->addAction(ui->actionView_Edit);
	connect(ui->listWidget->model(), &QAbstractItemModel::dataChanged, this, &RoomWidget::onListViewChanged);
}

RoomWidget::~RoomWidget()
{
	delete m_objectMenu;
	delete ui;
}

QString RoomWidget::tabText() const
{
	return QString::fromStdString(idName());
}

EditorTabWidget::Type RoomWidget::getType() const
{
	return EditorTabWidget::Room;
}

void RoomWidget::onListViewChanged()
{
	updateRoom();
	updatePreview();
}

void RoomWidget::updateRoom() const
{
	if (!m_room)
		return;
	auto objectList = m_room->getObjectList();
	std::vector<NovelTea::Room::RoomObject> objects;
	objectList->clear();
	for (int i = 0; i < ui->listWidget->count(); ++i)
	{
		auto item = ui->listWidget->item(i);
		if (item->checkState() == Qt::Checked)
			objectList->addId(item->text().toStdString());
		objects.push_back({item->text().toStdString(), item->checkState() == Qt::Checked});
	}
	m_room->setObjects(objects);
	m_room->setDescription(ui->scriptEdit->toPlainText().toStdString());
	m_room->setProperties(ui->propertyEditor->getValue());
}

void RoomWidget::updatePreview()
{
	json jdata;
	jdata["event"] = "text";
	jdata["data"] = ui->scriptEdit->toPlainText().toStdString();

	if (ui->scriptEdit->checkErrors<std::string>())
	{
		// Reset any changes made by previous script execution
		Save.reset();
		if (m_room)
		{
			m_room->getObjectList()->saveChanges();
			// Save room in case changes aren't yet saved to project
			Save.set<NovelTea::Room>(m_room);
			// Force reloading of room data we just saved in player
			NovelTea::Player::instance().setRoomId(m_room->getId());
		}

		ui->preview->processData(jdata);
	}
}

void RoomWidget::saveData() const
{
	if (m_room)
	{
		updateRoom();
		Proj.set<NovelTea::Room>(m_room, idName());
	}
}

void RoomWidget::loadData()
{
	m_room = Proj.get<NovelTea::Room>(idName());
	ui->listWidget->clear();

	qDebug() << "Loading room data... " << QString::fromStdString(idName());

	if (!m_room)
	{
		// Room is new, so show it as modified
		setModified();
		m_room = std::make_shared<NovelTea::Room>();
	}

	for (auto &roomObject : m_room->getObjects())
	{
		auto item = new QListWidgetItem(QString::fromStdString(roomObject.idName));
		item->setCheckState(roomObject.placeInRoom ? Qt::Checked : Qt::Unchecked);
		ui->listWidget->addItem(item);
	}

	ui->propertyEditor->setValue(m_room->getProperties());
	ui->scriptEdit->setPlainText(QString::fromStdString(m_room->getDescription()));

	MODIFIER(ui->listWidget->model(), &QAbstractItemModel::dataChanged);
	MODIFIER(ui->listWidget->model(), &QAbstractItemModel::rowsInserted);
	MODIFIER(ui->listWidget->model(), &QAbstractItemModel::rowsRemoved);
	MODIFIER(ui->scriptEdit, &ScriptEdit::textChanged);
	MODIFIER(ui->propertyEditor, &PropertyEditor::valueChanged);
}

void RoomWidget::on_actionAddObject_triggered()
{
	QWizard wizard;
	auto page = new WizardPageActionSelect;

	page->setFilterRegExp("Objects");
	page->allowCustomScript(false);

	wizard.addPage(page);

	if (wizard.exec() == QDialog::Accepted)
	{
		auto jval = page->getValue();
		auto idName = QString::fromStdString(jval[NovelTea::ID::entityId]);
		if (jval[NovelTea::ID::entityType] == NovelTea::EntityType::Object)
		{
			// Check if object already exists
			for (int i = 0; i < ui->listWidget->count(); ++i)
			{
				auto item = ui->listWidget->item(i);
				if (item->text() == idName)
					return;
			}

			auto item = new QListWidgetItem(idName);
			item->setCheckState(Qt::Checked);
			ui->listWidget->addItem(item);
		}
	}
}

void RoomWidget::on_actionRemoveObject_triggered()
{
	delete ui->listWidget->currentItem();
}

void RoomWidget::on_scriptEdit_textChanged()
{
	updatePreview();
}

void RoomWidget::on_listWidget_itemPressed(QListWidgetItem *item)
{
	if (QApplication::mouseButtons() != Qt::RightButton)
		return;
	m_objectMenu->popup(QCursor::pos());
}

void RoomWidget::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
	ui->actionView_Edit->trigger();
}

void RoomWidget::on_actionView_Edit_triggered()
{
	auto item = ui->listWidget->currentItem();
	auto w = new ObjectWidget(item->text().toStdString());
	MainWindow::instance().addEditorTab(w, true);
}

void RoomWidget::on_listWidget_currentRowChanged(int currentRow)
{
	ui->actionRemoveObject->setEnabled(currentRow >= 0);
}
