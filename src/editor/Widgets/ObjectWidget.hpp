#ifndef OBJECTWIDGET_HPP
#define OBJECTWIDGET_HPP

#include "EditorTabWidget.hpp"
#include <QWidget>
#include <QMenu>
#include <QStandardItemModel>
#include <json.hpp>
#include "QtPropertyBrowser/qtpropertymanager.h"
#include "QtPropertyBrowser/qtvariantproperty.h"
#include "QtPropertyBrowser/qttreepropertybrowser.h"

using json = nlohmann::json;

namespace Ui {
class ObjectWidget;
}
namespace NovelTea {
class Object;
}

class ObjectWidget : public EditorTabWidget
{
	Q_OBJECT
public:
	explicit ObjectWidget(const std::string &idName, QWidget *parent = 0);
	virtual ~ObjectWidget();

	QString tabText() const override;
	Type getType() const override;

private slots:
	void on_actionRemoveObject_triggered();

private:
	void fillPropertyEditor();

	void saveData() const override;
	void loadData() override;

	void propertyChanged(QtProperty *property, const QVariant &value);

	Ui::ObjectWidget *ui;

	QStandardItemModel *itemModel;

	QtVariantPropertyManager *variantManager;
	QtVariantEditorFactory *variantFactory;

	std::shared_ptr<NovelTea::Object> m_object;
};

#endif // OBJECTWIDGET_HPP
