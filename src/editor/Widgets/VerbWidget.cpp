#include "VerbWidget.hpp"
#include "ui_VerbWidget.h"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Verb.hpp>
#include <QDebug>

namespace {
	const auto propName = "Name";
}

VerbWidget::VerbWidget(const std::string &idName, QWidget *parent)
	: EditorTabWidget(parent)
	, ui(new Ui::VerbWidget)
	, variantManager(new QtVariantPropertyManager)
	, variantFactory(new QtVariantEditorFactory)
{
	_idName = idName;
	ui->setupUi(this);
	load();
}

VerbWidget::~VerbWidget()
{
	delete variantFactory;
	delete variantManager;
	delete ui;
}

QString VerbWidget::tabText() const
{
	return QString::fromStdString(idName());
}

EditorTabWidget::Type VerbWidget::getType() const
{
	return EditorTabWidget::Verb;
}

void VerbWidget::fillPropertyEditor()
{
	variantManager->disconnect();
	ui->propertyBrowser->clear();
	ui->propertyBrowser->setFactoryForManager(variantManager, variantFactory);
	ui->propertyBrowser->setPropertiesWithoutValueMarked(true);

	QtVariantProperty *prop;

	prop = variantManager->addProperty(QVariant::String, propName);
	prop->setValue(QString::fromStdString(m_verb->getName()));
	ui->propertyBrowser->addProperty(prop);

	connect(variantManager, &QtVariantPropertyManager::valueChanged, this, &VerbWidget::propertyChanged);
}

void VerbWidget::saveData() const
{
	if (m_verb)
		ProjData[NovelTea::ID::verbs][idName()] = *m_verb;
}

void VerbWidget::loadData()
{
	m_verb = Proj.verb(idName());

	qDebug() << "Loading verb data... " << QString::fromStdString(idName());

	if (!m_verb)
	{
		// Object is new, so show it as modified
		setModified();
		m_verb = std::make_shared<NovelTea::Verb>();
	}

	fillPropertyEditor();
}

void VerbWidget::propertyChanged(QtProperty *property, const QVariant &value)
{
	auto propertyName = property->propertyName();
	if (propertyName == propName)
		m_verb->setName(value.toString().toStdString());

	setModified();
}

void VerbWidget::on_actionRemoveObject_triggered()
{
}
