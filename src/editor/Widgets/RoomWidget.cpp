#include "RoomWidget.hpp"
#include "ui_RoomWidget.h"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Room.hpp>
#include <QDebug>

RoomWidget::RoomWidget(const std::string &idName, QWidget *parent)
	: EditorTabWidget(parent)
	, ui(new Ui::RoomWidget)
	, variantManager(new QtVariantPropertyManager)
	, variantFactory(new QtVariantEditorFactory)
{
	_idName = idName;
	ui->setupUi(this);
	load();
}

RoomWidget::~RoomWidget()
{
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
		ProjData[NovelTea::ID::rooms][idName()] = *m_room;
}

void RoomWidget::loadData()
{
	m_room = Proj.room(idName());
	ui->listWidget->disconnect();
	ui->listWidget->clear();

	qDebug() << "Loading room data... " << QString::fromStdString(idName());

	if (!m_room)
	{
		// Room is new, so show it as modified
		setModified();
		m_room = std::make_shared<NovelTea::Room>();
	}

	fillPropertyEditor();

	MODIFIER(ui->listWidget->model(), &QAbstractItemModel::dataChanged);
	MODIFIER(ui->listWidget->model(), &QAbstractItemModel::rowsInserted);
	MODIFIER(ui->listWidget->model(), &QAbstractItemModel::rowsRemoved);
}

void RoomWidget::propertyChanged(QtProperty *property, const QVariant &value)
{
	auto propertyName = property->propertyName();

	setModified();
}

void RoomWidget::on_actionRemoveObject_triggered()
{
}
