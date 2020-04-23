#ifndef CUTSCENEWIDGET_HPP
#define CUTSCENEWIDGET_HPP

#include "EditorTabWidget.hpp"
#include <QWidget>
#include <QMenu>
#include <QStandardItemModel>
#include <json.hpp>
#include "QtPropertyBrowser/qtpropertymanager.h"
#include "QtPropertyBrowser/qtvariantproperty.h"
#include "QtPropertyBrowser/qttreepropertybrowser.h"

using json = nlohmann::json;

namespace Ui {
class CutsceneWidget;
}
namespace NovelTea {
class Cutscene;
class CutsceneSegment;
}

class CutsceneWidget : public EditorTabWidget
{
	Q_OBJECT

public:
	explicit CutsceneWidget(const std::string &idName, QWidget *parent = 0);
	virtual ~CutsceneWidget();

	QString tabText() const override;
	Type getType() const override;

private slots:
	void on_actionAddText_triggered();
	void on_actionAddPageBreak_triggered();
	void on_treeView_pressed(const QModelIndex &index);
	void on_actionRemoveSegment_triggered();
	void on_horizontalSlider_valueChanged(int value);

protected:
	void timerEvent(QTimerEvent *event);

private:
	void createMenus();
	void fillPropertyEditor();
	void checkIndexChange();
	void addItem(std::shared_ptr<NovelTea::CutsceneSegment> segment, int index = -1);

	void saveData() const override;
	void loadData() override;

	void propertyChanged(QtProperty *property, const QVariant &value);

	Ui::CutsceneWidget *ui;

	QMenu *menuAdd;
	QStandardItemModel *itemModel;
	int selectedIndex;

	QtVariantPropertyManager *variantManager;
	QtVariantEditorFactory *variantFactory;

	std::shared_ptr<NovelTea::Cutscene> m_cutscene;
};

#endif // CUTSCENEWIDGET_HPP
