#ifndef SPELLCHECKWIDGET_HPP
#define SPELLCHECKWIDGET_HPP

#include "EditorTabWidget.hpp"
#include <NovelTea/json.hpp>
#include <QTreeWidgetItem>
#include <memory>

namespace Ui {
class SpellCheckWidget;
}
class SpellChecker;

class SpellCheckWidget : public EditorTabWidget
{
	Q_OBJECT
public:
	explicit SpellCheckWidget(QWidget *parent = 0);
	virtual ~SpellCheckWidget();

	void refresh();

	QString tabText() const override;
	Type getType() const override;

private slots:
	void on_buttonSearchAgain_clicked();
	void on_treeWidget_activated(const QModelIndex &index);
	void on_buttonAddToWhitelist_clicked();

protected:
	void timerEvent(QTimerEvent*) override;

private:
	void checkIndexChange();
	void fillWordInfo();
	void saveData() const override;
	void loadData() override;

	void processPropJson(QTreeWidgetItem *treeItem, const sj::JSON &jprops);
	void processJavascript(QTreeWidgetItem *treeItem, const QString &s);
	void processString(QTreeWidgetItem *treeItem, const QString &s);
	void checkEntities(const std::string &entityId, const QString &name, bool checkProps);

	Ui::SpellCheckWidget *ui;
	std::unique_ptr<SpellChecker> m_spellChecker;
	QTreeWidgetItem *m_selectedItem;
};

#endif // SPELLCHECKWIDGET_HPP
