#ifndef VERBWIDGET_HPP
#define VERBWIDGET_HPP

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
class VerbWidget;
}
namespace NovelTea {
class Verb;
}

class VerbWidget : public EditorTabWidget
{
	Q_OBJECT
public:
	explicit VerbWidget(const std::string &idName, QWidget *parent = 0);
	virtual ~VerbWidget();

	QString tabText() const override;
	Type getType() const override;

private slots:
	void on_actionRemoveObject_triggered();

private:
	void fillPropertyEditor();

	void saveData() const override;
	void loadData() override;

	void propertyChanged(QtProperty *property, const QVariant &value);

	Ui::VerbWidget *ui;

	QStandardItemModel *itemModel;

	QtVariantPropertyManager *variantManager;
	QtVariantEditorFactory *variantFactory;

	std::shared_ptr<NovelTea::Verb> m_verb;
};

#endif // VERBWIDGET_HPP
