#ifndef DIALOGUEWIDGET_HPP
#define DIALOGUEWIDGET_HPP

#include "EditorTabWidget.hpp"
#include <NovelTea/Dialogue.hpp>
#include <QWidget>
#include <json.hpp>

using json = nlohmann::json;

namespace Ui {
class DialogueWidget;
}

class DialogueWidget : public EditorTabWidget
{
	Q_OBJECT
public:
	explicit DialogueWidget(const std::string &idName, QWidget *parent = 0);
	virtual ~DialogueWidget();

	QString tabText() const override;
	Type getType() const override;

private slots:
	void on_actionRemoveObject_triggered();

private:
	void saveData() const override;
	void loadData() override;

	Ui::DialogueWidget *ui;

	std::shared_ptr<NovelTea::Dialogue> m_dialogue;
};

#endif // DIALOGUEWIDGET_HPP
