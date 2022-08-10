#ifndef ACTIONWIDGET_HPP
#define ACTIONWIDGET_HPP

#include "EditorTabWidget.hpp"
#include <NovelTea/Action.hpp>
#include <QWidget>
#include <QComboBox>
#include <NovelTea/json.hpp>

using json = sj::JSON;

namespace Ui {
class ActionWidget;
}
class ActionBuildWidget;

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
	ActionBuildWidget *m_actionBuilder;

	std::vector<QComboBox*> m_comboBoxes;
	QStringList m_objectStrings;

	std::shared_ptr<NovelTea::Action> m_action;
};

#endif // ACTIONWIDGET_HPP
