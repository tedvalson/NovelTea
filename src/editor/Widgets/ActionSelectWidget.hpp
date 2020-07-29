#ifndef ACTIONSELECTWIDGET_HPP
#define ACTIONSELECTWIDGET_HPP

#include <QWidget>
#include <NovelTea/json.hpp>

namespace Ui {
class ActionSelectWidget;
}

class ActionSelectWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ActionSelectWidget(QWidget *parent = 0);
	~ActionSelectWidget();

	void setValue(sj::JSON value);
	sj::JSON getValue() const;

private slots:
	void on_pushButton_clicked();

signals:
	void valueChanged(sj::JSON value);

private:
	Ui::ActionSelectWidget *ui;
	sj::JSON m_value;
};

#endif // ACTIONSELECTWIDGET_HPP
