#include "ObjectWidget.hpp"
#include "ui_ObjectWidget.h"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Object.hpp>
#include <QDebug>

ObjectWidget::ObjectWidget(const std::string &idName, QWidget *parent)
	: EditorTabWidget(parent)
	, ui(new Ui::ObjectWidget)
	, variantManager(new QtVariantPropertyManager)
	, variantFactory(new QtVariantEditorFactory)
{
	_idName = idName;
	ui->setupUi(this);
	load();
}

ObjectWidget::~ObjectWidget()
{
	delete variantFactory;
	delete variantManager;
	delete ui;
}

QString ObjectWidget::tabText() const
{
	return QString::fromStdString(idName());
}

EditorTabWidget::Type ObjectWidget::getType() const
{
	return EditorTabWidget::Object;
}

void ObjectWidget::fillPropertyEditor()
{
	variantManager->disconnect();
	ui->propertyBrowser->clear();
	ui->propertyBrowser->setFactoryForManager(variantManager, variantFactory);
	ui->propertyBrowser->setPropertiesWithoutValueMarked(true);

	QtVariantProperty *prop;

	connect(variantManager, &QtVariantPropertyManager::valueChanged, this, &ObjectWidget::propertyChanged);
}

void ObjectWidget::saveData() const
{
	if (m_object)
	{
		m_object->setProperties(ui->propertyEditor->getValue());
		Proj.set<NovelTea::Object>(m_object, idName());
	}
}

void ObjectWidget::loadData()
{
	m_object = Proj.get<NovelTea::Object>(idName());

	qDebug() << "Loading object data... " << QString::fromStdString(idName());

	if (!m_object)
	{
		// Object is new, so show it as modified
		setModified();
		m_object = std::make_shared<NovelTea::Object>();
	}

	ui->propertyEditor->setValue(m_object->getProperties());
	fillPropertyEditor();

	MODIFIER(ui->propertyEditor, &PropertyEditor::valueChanged);
}

void ObjectWidget::propertyChanged(QtProperty *property, const QVariant &value)
{
	auto propertyName = property->propertyName();

	setModified();
}

void ObjectWidget::on_actionRemoveObject_triggered()
{
}
