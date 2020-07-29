#include "PropertyEditor.hpp"
#include "ui_PropertyEditor.h"
#include <QToolButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>
#include <iostream>

PropertyEditor::PropertyEditor(QWidget *parent)
: QWidget(parent)
, ui(new Ui::PropertyEditor)
, m_menuAdd(new QMenu)
, m_variantManager(new QtVariantPropertyManager)
, m_variantFactory(new QtVariantEditorFactory)
, m_value(sj::Object())
{
	ui->setupUi(this);

	ui->propertyBrowser->setFactoryForManager(m_variantManager, m_variantFactory);
	ui->propertyBrowser->setPropertiesWithoutValueMarked(true);

	m_menuAdd->addAction(ui->actionAddTextProperty);
	m_menuAdd->addAction(ui->actionAddNumberProperty);
	m_menuAdd->addAction(ui->actionAddBooleanProperty);

	// Attach the menu to the Add toolbutton
	ui->actionAddProperty->setMenu(m_menuAdd);
	auto buttonAdd = static_cast<QToolButton*>(ui->toolBar->widgetForAction(ui->actionAddProperty));
	buttonAdd->setPopupMode(QToolButton::InstantPopup);

	connect(m_variantManager, &QtVariantPropertyManager::valueChanged, this, &PropertyEditor::update);
}

PropertyEditor::~PropertyEditor()
{
	delete m_variantFactory;
	delete m_variantManager;
	delete m_menuAdd;
	delete ui;
}

void PropertyEditor::setValue(sj::JSON value)
{
	if (getValue() != value)
	{
		if (value.JSONType() != json::Class::Object)
			return;

		QtVariantProperty *prop;
		ui->propertyBrowser->clear();
		m_variantManager->clear();

		for (auto &item : value.ObjectRange())
		{
			auto &jval = item.second;
			auto val = QVariant();
			auto type = QVariant::String;
			if (jval.JSONType() == json::Class::String) {
				val = QString::fromStdString(jval.ToString());
			} else if (jval.JSONType() == json::Class::Boolean) {
				type = QVariant::Bool;
				val = jval.ToBool();
			} else if (jval.JSONType() == json::Class::Floating || jval.JSONType() == json::Class::Integral) {
				type = QVariant::Double;
				val = static_cast<double>(jval.ToFloat());
			} else
				continue;

			prop = m_variantManager->addProperty(type, QString::fromStdString(item.first));
			prop->setValue(val);
			ui->propertyBrowser->addProperty(prop);
		}

		m_value = value;
		emit valueChanged(value);
	}
}

sj::JSON PropertyEditor::getValue() const
{
	return m_value;
}

void PropertyEditor::addProperty(int type)
{
	auto propName = QInputDialog::getText(this, "Add Property", "Property Name:");
	if (!propName.isEmpty())
	{
		for (auto &prop : ui->propertyBrowser->properties())
			if (propName == prop->propertyName())
			{
				QMessageBox::critical(this, "Duplicate Property Name", "A property with that name already exists.");
				return;
			}

		QtVariantProperty *prop;
		prop = m_variantManager->addProperty(type, propName);
		ui->propertyBrowser->addProperty(prop);
		update();
	}
}

void PropertyEditor::addProperty(int type, const QString &name, const QVariant &value)
{
	QtVariantProperty *prop;
	prop = m_variantManager->addProperty(type, name);
	prop->setValue(value);
	ui->propertyBrowser->addProperty(prop);
}

void PropertyEditor::update()
{
	m_value = sj::Object();
	for (auto &prop : ui->propertyBrowser->properties())
	{
		auto name = prop->propertyName().toStdString();
		auto type = m_variantManager->valueType(prop);
		auto value = m_variantManager->value(prop);
		if (type == QVariant::String)
			m_value[name] = value.toString().toStdString();
		else if (type == QVariant::Double)
			m_value[name] = value.toDouble();
		else if (type == QVariant::Bool)
			m_value[name] = value.toBool();
	}
	emit valueChanged(m_value);
}

void PropertyEditor::on_actionRemoveProperty_triggered()
{
	ui->propertyBrowser->removeProperty(ui->propertyBrowser->currentItem()->property());
	update();
}

void PropertyEditor::on_actionAddTextProperty_triggered()
{
	addProperty(QVariant::String);
}

void PropertyEditor::on_actionAddNumberProperty_triggered()
{
	addProperty(QVariant::Double);
}

void PropertyEditor::on_actionAddBooleanProperty_triggered()
{
	addProperty(QVariant::Bool);
}

void PropertyEditor::on_propertyBrowser_currentItemChanged(QtBrowserItem *item)
{
	ui->actionRemoveProperty->setEnabled(item);
}
