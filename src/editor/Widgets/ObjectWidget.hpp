#ifndef OBJECTWIDGET_HPP
#define OBJECTWIDGET_HPP

#include "EditorTabWidget.hpp"
#include <NovelTea/Object.hpp>
#include <QWidget>
#include <QMenu>
#include <QStandardItemModel>
#include <NovelTea/json.hpp>

using json = sj::JSON;

namespace Ui {
class ObjectWidget;
}

class QtVariantEditorFactory;
class QtVariantPropertyManager;
class QtProperty;

class ObjectWidget : public EditorTabWidget
{
	Q_OBJECT
public:
	explicit ObjectWidget(const std::string &idName, QWidget *parent = 0);
	virtual ~ObjectWidget();

	QString tabText() const override;
	Type getType() const override;

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
