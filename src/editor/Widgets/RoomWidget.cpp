#include "RoomWidget.hpp"
#include "ui_RoomWidget.h"
#include "MainWindow.hpp"
#include "ObjectWidget.hpp"
#include "Wizard/WizardPageActionSelect.hpp"
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/States/StateEditor.hpp>
#include <NovelTea/Game.hpp>
#include <QDebug>

RoomWidget::RoomWidget(const std::string &idName, QWidget *parent)
	: EditorTabWidget(parent)
	, ui(new Ui::RoomWidget)
	, m_objectMenu(new QMenu)
{
	m_idName = idName;
	ui->setupUi(this);
	ui->preview->setMode(NovelTea::StateEditorMode::Room);
	ui->preview->setFPS(0.f);
	m_objectMenu->addAction(ui->actionView_Edit);

	ui->toolBarEntities->insertWidget(ui->actionAddObject, ui->widget);

	ui->scriptEditBeforeEnter->hide();
	ui->scriptEditAfterEnter->hide();
	ui->scriptEditBeforeLeave->hide();
	ui->scriptEditAfterLeave->hide();

	load();
	connect(ui->listWidget->model(), &QAbstractItemModel::dataChanged, this, &RoomWidget::onListViewChanged);
	connect(&MainWindow::instance(), &MainWindow::renamed, [this](){
		m_room = Proj.get<NovelTea::Room>(this->idName());
		refreshObjectList();
		ui->scriptEdit->blockSignals(true);
		ui->scriptEdit->setPlainText(QString::fromStdString(m_room->getDescriptionRaw()));
		ui->scriptEdit->blockSignals(false);
	});
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

void RoomWidget::refreshObjectList()
{
	ui->listWidget->model()->blockSignals(true);
	ui->listWidget->clear();
	for (auto &roomObject : m_room->getObjects())
	{
		auto item = new QListWidgetItem(QString::fromStdString(roomObject.idName));
		item->setCheckState(roomObject.placeInRoom ? Qt::Checked : Qt::Unchecked);
		ui->listWidget->addItem(item);
	}
	ui->listWidget->model()->blockSignals(false);
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

#define SET_SCRIPT(name) \
	if (ui->checkBox##name->isChecked()) \
		m_room->setScript##name(ui->scriptEdit##name->toPlainText().toStdString()); \
	else \
		m_room->setScript##name("");

	SET_SCRIPT(BeforeEnter);
	SET_SCRIPT(AfterEnter);
	SET_SCRIPT(BeforeLeave);
	SET_SCRIPT(AfterLeave);

	m_room->setName(ui->lineNameEdit->text().toStdString());
	m_room->setPaths(jpaths);
	m_room->setObjects(objects);
	m_room->setDescriptionRaw(ui->scriptEdit->toPlainText().toStdString());
	m_room->setProperties(ui->propertyEditor->getValue());
}

void RoomWidget::updatePreview()
{
	auto script = ui->scriptEdit->toPlainText().toStdString();
	script = "room=Game.room;text='';\n" + script + "\nreturn text;";
	if (ui->scriptEdit->checkErrors<std::string>(script))
	{
		json jdata;
		jdata["event"] = "text";

		// Reset any changes made by previous script execution
		GSave->reset();
		if (m_room)
		{
			m_room->setDescriptionRaw(script);
			m_room->getObjectList()->saveChanges();
			// Save room in case changes aren't yet saved to project
			GSave->set(m_room);
			// Force reloading of room data we just saved in player
			ActiveGame->setRoomId(m_room->getId());
		}

		ui->preview->processData(jdata);
	}

	ui->preview->repaint();
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

	qDebug() << "Loading room data... " << QString::fromStdString(idName());

	if (!m_room)
	{
		// Room is new, so show it as modified
		setModified();
		m_room = std::make_shared<NovelTea::Room>();
	}

	refreshObjectList();

	ui->lineNameEdit->setText(QString::fromStdString(m_room->getName()));
	ui->propertyEditor->setValue(m_room->getProperties());
	ui->scriptEdit->setPlainText(QString::fromStdString(m_room->getDescriptionRaw()));

	ui->scriptEditBeforeEnter->setPlainText(QString::fromStdString(m_room->getScriptBeforeEnter()));
	ui->scriptEditAfterEnter->setPlainText(QString::fromStdString(m_room->getScriptAfterEnter()));
	ui->scriptEditBeforeLeave->setPlainText(QString::fromStdString(m_room->getScriptBeforeLeave()));
	ui->scriptEditAfterLeave->setPlainText(QString::fromStdString(m_room->getScriptAfterLeave()));
	ui->checkBoxBeforeEnter->setChecked(!ui->scriptEditBeforeEnter->toPlainText().isEmpty());
	ui->checkBoxAfterEnter->setChecked(!ui->scriptEditAfterEnter->toPlainText().isEmpty());
	ui->checkBoxBeforeLeave->setChecked(!ui->scriptEditBeforeLeave->toPlainText().isEmpty());
	ui->checkBoxAfterLeave->setChecked(!ui->scriptEditAfterLeave->toPlainText().isEmpty());

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
	MODIFIER(ui->lineNameEdit, &QLineEdit::textChanged);
	MODIFIER(ui->scriptEdit, &ScriptEdit::textChanged);
	MODIFIER(ui->propertyEditor, &PropertyEditor::valueChanged);

	MODIFIER(ui->checkBoxBeforeEnter, &QCheckBox::toggled);
	MODIFIER(ui->scriptEditBeforeEnter, &ScriptEdit::textChanged);
	MODIFIER(ui->checkBoxAfterEnter, &QCheckBox::toggled);
	MODIFIER(ui->scriptEditAfterEnter, &ScriptEdit::textChanged);
	MODIFIER(ui->checkBoxBeforeLeave, &QCheckBox::toggled);
	MODIFIER(ui->scriptEditBeforeLeave, &ScriptEdit::textChanged);
	MODIFIER(ui->checkBoxAfterLeave, &QCheckBox::toggled);
	MODIFIER(ui->scriptEditAfterLeave, &ScriptEdit::textChanged);
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

void RoomWidget::on_checkBoxBeforeEnter_toggled(bool checked)
{
	ui->scriptEditBeforeEnter->setVisible(checked);
}

void RoomWidget::on_checkBoxAfterEnter_toggled(bool checked)
{
	ui->scriptEditAfterEnter->setVisible(checked);
}

void RoomWidget::on_checkBoxBeforeLeave_toggled(bool checked)
{
	ui->scriptEditBeforeLeave->setVisible(checked);
}

void RoomWidget::on_checkBoxAfterLeave_toggled(bool checked)
{
	ui->scriptEditAfterLeave->setVisible(checked);
}
