#ifndef SCRIPTWIDGET_HPP
#define SCRIPTWIDGET_HPP

#include "EditorTabWidget.hpp"
#include <NovelTea/Script.hpp>
#include <QWidget>
#include <NovelTea/json.hpp>

using json = sj::JSON;

namespace Ui {
class ScriptWidget;
}

class ScriptWidget : public EditorTabWidget
{
	Q_OBJECT
public:
	explicit ScriptWidget(const std::string &idName, QWidget *parent = 0);
	virtual ~ScriptWidget();

	QString tabText() const override;
	Type getType() const override;

private slots:

private:
	void saveData() const override;
	void loadData() override;

	Ui::ScriptWidget *ui;

	std::shared_ptr<NovelTea::Script> m_script;
};

#endif // SCRIPTWIDGET_HPP
