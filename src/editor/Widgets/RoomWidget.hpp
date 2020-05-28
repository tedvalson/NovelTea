#ifndef ROOMWIDGET_HPP
#define ROOMWIDGET_HPP

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
class RoomWidget;
}
namespace NovelTea {
class Room;
}

class RoomWidget : public EditorTabWidget
{
	Q_OBJECT
public:
	explicit RoomWidget(const std::string &idName, QWidget *parent = 0);
	virtual ~RoomWidget();

	QString tabText() const override;
	Type getType() const override;

private slots:
	void on_actionRemoveObject_triggered();

private:
	void fillPropertyEditor();

	void saveData() const override;
	void loadData() override;

	void propertyChanged(QtProperty *property, const QVariant &value);

	Ui::RoomWidget *ui;

	QStandardItemModel *itemModel;

	QtVariantPropertyManager *variantManager;
	QtVariantEditorFactory *variantFactory;

	std::shared_ptr<NovelTea::Room> m_room;
};

#endif // ROOMWIDGET_HPP
