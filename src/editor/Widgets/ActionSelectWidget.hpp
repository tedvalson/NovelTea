#ifndef ACTIONSELECTWIDGET_HPP
#define ACTIONSELECTWIDGET_HPP

#include <QWidget>
#include <QAbstractItemModel>
#include <json.hpp>

namespace Ui {
class ActionSelectWidget;
}

class ActionSelectWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ActionSelectWidget(QWidget *parent = 0);
	~ActionSelectWidget();

	void setModel(QAbstractItemModel *model);

	void setValue(nlohmann::json value);
	nlohmann::json getValue() const;

private:
	Ui::ActionSelectWidget *ui;
	QAbstractItemModel *itemModel;
};

#endif // ACTIONSELECTWIDGET_HPP
