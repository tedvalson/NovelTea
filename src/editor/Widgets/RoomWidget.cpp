#include "RoomWidget.hpp"
#include "ui_RoomWidget.h"
#include "MainWindow.hpp"
#include "ObjectWidget.hpp"
#include "Wizard/WizardPageActionSelect.hpp"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/States/StateEditor.hpp>
#include <NovelTea/Game.hpp>
#include <QWizard>
#include <QDebug>

RoomWidget::RoomWidget(const std::string &idName, QWidget *parent)
	: EditorTabWidget(parent)
	, ui(new Ui::RoomWidget)
	, m_objectMenu(new QMenu)
{
	m_idName = idName;
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

	auto jpaths = sj::Array();
	savePathDirection(jpaths[0], ui->checkBoxNorthwest, ui->selectNorthwest);
	savePathDirection(jpaths[1], ui->checkBoxNorth,     ui->selectNorth);
	savePathDirection(jpaths[2], ui->checkBoxNortheast, ui->selectNortheast);
	savePathDirection(jpaths[3], ui->checkBoxWest,      ui->selectWest);
	savePathDirection(jpaths[4], ui->checkBoxEast,      ui->selectEast);
	savePathDirection(jpaths[5], ui->checkBoxSouthwest, ui->selectSouthwest);
	savePathDirection(jpaths[6], ui->checkBoxSouth,     ui->selectSouth);
	savePathDirection(jpaths[7], ui->checkBoxSoutheast, ui->selectSoutheast);

	m_room->setPaths(jpaths);
	m_room->setObjects(objects);
	m_room->setDescription(ui->scriptEdit->toPlainText().toStdString());
	m_room->setProperties(ui->propertyEditor->getValue());
}

void RoomWidget::updatePreview()
{
	if (ui->scriptEdit->checkErrors<std::string>())
	{
		json jdata;
		jdata["event"] = "text";
		jdata["data"] = ui->scriptEdit->toPlainText().toStdString();

		// Reset any changes made by previous script execution
		GSave.reset();
		if (m_room)
		{
			m_room->getObjectList()->saveChanges();
			// Save room in case changes aren't yet saved to project
			GSave.set(m_room);
			// Force reloading of room data we just saved in player
			ActiveGame->setRoomId(m_room->getId());
		}

		ui->preview->processData(jdata);
	}
}

void RoomWidget::savePathDirection(json &path, const QCheckBox *checkBox, const ActionSelectWidget *actionSelect) const
{
	path[0] = checkBox->isChecked();
	path[1] = actionSelect->getValue();
}

void RoomWidget::loadPathDirection(const json &path, QCheckBox *checkBox, ActionSelectWidget *actionSelect)
{
	checkBox->setChecked(path[0].ToBool());
	actionSelect->setValue(path[1]);
	MODIFIER(actionSelect, &ActionSelectWidget::valueChanged);
	MODIFIER(checkBox, &QCheckBox::toggled);
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

	auto paths = m_room->getPaths();
	loadPathDirection(paths[0], ui->checkBoxNorthwest, ui->selectNorthwest);
	loadPathDirection(paths[1], ui->checkBoxNorth,     ui->selectNorth);
	loadPathDirection(paths[2], ui->checkBoxNortheast, ui->selectNortheast);
	loadPathDirection(paths[3], ui->checkBoxWest,      ui->selectWest);
	loadPathDirection(paths[4], ui->checkBoxEast,      ui->selectEast);
	loadPathDirection(paths[5], ui->checkBoxSouthwest, ui->selectSouthwest);
	loadPathDirection(paths[6], ui->checkBoxSouth,     ui->selectSouth);
	loadPathDirection(paths[7], ui->checkBoxSoutheast, ui->selectSoutheast);

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
		auto idName = QString::fromStdString(jval[NovelTea::ID::selectEntityId].ToString());
		auto type = static_cast<NovelTea::EntityType>(jval[NovelTea::ID::selectEntityType].ToInt());
		if (type == NovelTea::EntityType::Object)
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
