#include "RoomWidget.hpp"
#include "ui_RoomWidget.h"
#include "MainWindow.hpp"
#include "ObjectWidget.hpp"
#include "Wizard/WizardPageActionSelect.hpp"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/States/StateEditor.hpp>
#include <QWizard>
#include <QDebug>

RoomWidget::RoomWidget(const std::string &idName, QWidget *parent)
	: EditorTabWidget(parent)
	, ui(new Ui::RoomWidget)
	, variantManager(new QtVariantPropertyManager)
	, variantFactory(new QtVariantEditorFactory)
	, m_objectMenu(new QMenu)
{
	_idName = idName;
	ui->setupUi(this);
	ui->preview->setMode(NovelTea::StateEditorMode::Room);
	load();

	m_objectMenu->addAction(ui->actionView_Edit);
}

RoomWidget::~RoomWidget()
{
	delete m_objectMenu;
	delete variantFactory;
	delete variantManager;
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

void RoomWidget::fillPropertyEditor()
{
	variantManager->disconnect();
	ui->propertyBrowser->clear();
	ui->propertyBrowser->setFactoryForManager(variantManager, variantFactory);
	ui->propertyBrowser->setPropertiesWithoutValueMarked(true);

	QtVariantProperty *prop;

	connect(variantManager, &QtVariantPropertyManager::valueChanged, this, &RoomWidget::propertyChanged);
}

void RoomWidget::saveData() const
{
	if (m_room)
	{
		std::vector<NovelTea::Room::RoomObject> objects;
		for (int i = 0; i < ui->listWidget->count(); ++i)
		{
			auto item = ui->listWidget->item(i);
			objects.push_back({item->text().toStdString(), item->checkState() == Qt::Checked});
		}
		m_room->setObjects(objects);
		m_room->setDescription(ui->textEdit->toPlainText().toStdString());
		ProjData[NovelTea::ID::rooms][idName()] = *m_room;
	}
}

void RoomWidget::loadData()
{
	m_room = Proj.room(idName());
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

	ui->textEdit->setPlainText(QString::fromStdString(m_room->getDescription()));
	fillPropertyEditor();

	MODIFIER(ui->listWidget->model(), &QAbstractItemModel::dataChanged);
	MODIFIER(ui->listWidget->model(), &QAbstractItemModel::rowsInserted);
	MODIFIER(ui->listWidget->model(), &QAbstractItemModel::rowsRemoved);
	MODIFIER(ui->textEdit, &QPlainTextEdit::textChanged);
}

void RoomWidget::propertyChanged(QtProperty *property, const QVariant &value)
{
	auto propertyName = property->propertyName();

	setModified();
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

void RoomWidget::on_textEdit_textChanged()
{
	json jdata;
	jdata["event"] = "text";
	jdata["data"] = ui->textEdit->toPlainText().toStdString();

	if (ui->textEdit->checkErrors<std::string>())
		ui->preview->processData(jdata);
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
