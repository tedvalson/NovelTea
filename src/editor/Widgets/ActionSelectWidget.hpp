#ifndef ACTIONSELECTWIDGET_HPP
#define ACTIONSELECTWIDGET_HPP

#include <QWidget>
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

	void setValue(nlohmann::json value);
	nlohmann::json getValue() const;

private slots:
	void on_comboAction_currentIndexChanged(int index);

signals:
	void valueChanged();

private:
	Ui::ActionSelectWidget *ui;
};

#endif // ACTIONSELECTWIDGET_HPP
