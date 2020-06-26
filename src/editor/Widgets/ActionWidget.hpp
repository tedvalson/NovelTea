#ifndef ACTIONWIDGET_HPP
#define ACTIONWIDGET_HPP

#include "EditorTabWidget.hpp"
#include <QWidget>
#include <QComboBox>
#include <json.hpp>

using json = nlohmann::json;

namespace Ui {
class ActionWidget;
}
namespace NovelTea {
class Action;
}

class ActionWidget : public EditorTabWidget
{
	Q_OBJECT
public:
	explicit ActionWidget(const std::string &idName, QWidget *parent = 0);
	virtual ~ActionWidget();

	QString tabText() const override;
	Type getType() const override;

private slots:
	void on_pushButton_clicked();

private:
	void saveData() const override;
	void loadData() override;

	Ui::ActionWidget *ui;

	std::vector<QComboBox*> m_comboBoxes;
	QStringList m_objectStrings;

	std::shared_ptr<NovelTea::Action> m_action;
};

#endif // ACTIONWIDGET_HPP
