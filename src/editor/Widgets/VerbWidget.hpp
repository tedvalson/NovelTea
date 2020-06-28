#ifndef VERBWIDGET_HPP
#define VERBWIDGET_HPP

#include "EditorTabWidget.hpp"
#include <NovelTea/Verb.hpp>
#include <QWidget>
#include <QLineEdit>
#include <json.hpp>

using json = nlohmann::json;

namespace Ui {
class VerbWidget;
}

class VerbWidget : public EditorTabWidget
{
	Q_OBJECT
public:
	explicit VerbWidget(const std::string &idName, QWidget *parent = 0);
	virtual ~VerbWidget();

	QString tabText() const override;
	Type getType() const override;

private slots:
	void on_horizontalSlider_valueChanged(int value);

private:
	void addLineEdit();
	void loadActionStructure();
	void saveData() const override;
	void loadData() override;

	Ui::VerbWidget *ui;

	std::vector<QLineEdit*> m_lineEdits;

	std::shared_ptr<NovelTea::Verb> m_verb;
};

#endif // VERBWIDGET_HPP
