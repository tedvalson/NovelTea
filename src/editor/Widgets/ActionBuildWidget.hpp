#ifndef ACTIONBUILDWIDGET_HPP
#define ACTIONBUILDWIDGET_HPP

#include <QWidget>
#include <QComboBox>
#include <NovelTea/json.hpp>

namespace Ui {
class ActionBuildWidget;
}

class ActionBuildWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ActionBuildWidget(QWidget *parent = 0);
	~ActionBuildWidget();

	void setValue(sj::JSON value);
	sj::JSON getValue() const;

public slots:
	void refresh();

private slots:
	void on_comboVerb_currentIndexChanged(const QString &value);
	void comboBox_currentIndexChanged(const QString &value);

signals:
	void valueChanged(sj::JSON value);

private:
	bool isValid() const;

	Ui::ActionBuildWidget *ui;
	sj::JSON m_value;

	std::vector<QComboBox*> m_comboBoxes;
	QStringList m_objectStrings;
};

#endif // ACTIONBUILDWIDGET_HPP
