#ifndef SEARCHWIDGET_HPP
#define SEARCHWIDGET_HPP

#include "EditorTabWidget.hpp"
#include <NovelTea/json.hpp>
#include <QTreeWidgetItem>

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
	void on_treeWidget_activated(const QModelIndex &index);

private:
	void saveData() const override;
	void loadData() override;

	void processEntityJson(QTreeWidgetItem *treeItem, const sj::JSON &jentity, bool caseSensitive);
	void processString(QTreeWidgetItem *treeItem, const std::string &value, bool caseSensitive);
	void searchEntities(const std::string &entityId, const QString &name, bool caseSensitive);
	void searchTests(bool caseSensitive);
	void searchProjectSettings(bool caseSensitive);

	Ui::SearchWidget *ui;
	std::string m_searchTerm;
};

#endif // SEARCHWIDGET_HPP
