#ifndef ACTIONBUILDWIDGET_HPP
#define ACTIONBUILDWIDGET_HPP

#include <QWidget>
#include <QComboBox>
#include <json.hpp>

namespace Ui {
class ActionBuildWidget;
}

class ActionBuildWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ActionBuildWidget(QWidget *parent = 0);
	~ActionBuildWidget();

	void setValue(nlohmann::json value);
	nlohmann::json getValue() const;

public slots:
	void refresh();

private slots:
	void on_comboVerb_currentIndexChanged(const QString &value);
	void comboBox_currentIndexChanged(const QString &value);

signals:
	void valueChanged(nlohmann::json value);

private:
	bool isValid() const;

	Ui::ActionBuildWidget *ui;
	nlohmann::json m_value;

	std::vector<QComboBox*> m_comboBoxes;
	QStringList m_objectStrings;
};

#endif // ACTIONBUILDWIDGET_HPP
