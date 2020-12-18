#ifndef SEARCHWIDGET_HPP
#define SEARCHWIDGET_HPP

#include "EditorTabWidget.hpp"
#include <QWidget>

namespace Ui {
class SearchWidget;
}

class SearchWidget : public EditorTabWidget
{
	Q_OBJECT
public:
	explicit SearchWidget(const std::string &searchTerm, QWidget *parent = 0);
	virtual ~SearchWidget();

	QString tabText() const override;
	Type getType() const override;

private slots:
	void on_buttonSearchAgain_clicked();

private:
	void saveData() const override;
	void loadData() override;

	void searchRooms();

	Ui::SearchWidget *ui;
	std::string m_searchTerm;
};

#endif // SEARCHWIDGET_HPP
