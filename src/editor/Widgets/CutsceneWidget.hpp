#ifndef CUTSCENEWIDGET_HPP
#define CUTSCENEWIDGET_HPP

#include "EditorTabWidget.hpp"
#include <NovelTea/Cutscene.hpp>
#include <QWidget>
#include <QMenu>
#include <QListWidgetItem>
#include <NovelTea/json.hpp>
#include "QtPropertyBrowser/qtpropertymanager.h"
#include "QtPropertyBrowser/qtvariantproperty.h"
#include "QtPropertyBrowser/qttreepropertybrowser.h"

using json = sj::JSON;

namespace Ui {
class CutsceneWidget;
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
	void on_actionAddPage_triggered();
	void on_actionAddPageBreak_triggered();
	void on_actionAddScript_triggered();
	void on_actionRemoveSegment_triggered();
	void on_horizontalSlider_valueChanged(int value);
	void on_actionPlayPause_toggled(bool checked);
	void on_actionStop_triggered();
	void on_actionLoop_toggled(bool checked);
	void on_listWidget_clicked(const QModelIndex &index);
	void on_scriptEdit_textChanged();

protected:
	void timerEvent(QTimerEvent*) override;
	void showEvent(QShowEvent*) override;
	void hideEvent(QHideEvent*) override;

	void updateCutscene();
	void updateLoopValues();

private:
	void createMenus();
	void fillPropertyEditor();
	void fillSettingsPropertyEditor();
	void checkIndexChange();
	void addItem(std::shared_ptr<NovelTea::CutsceneSegment> segment, bool addToInternalObject = true, int index = -1);

	void saveData() const override;
	void loadData() override;

	void segmentPropertyChanged(QtProperty *property, const QVariant &value);
	void settingPropertyChanged(QtProperty *property, const QVariant &value);

	void on_rowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);

	Ui::CutsceneWidget *ui;

	QMenu *menuAdd;
	int selectedIndex;
	int timerId;

	QtVariantPropertyManager *segmentsVariantManager;
	QtVariantPropertyManager *settingsVariantManager;
	QtVariantEditorFactory *variantFactory;

	std::shared_ptr<NovelTea::Cutscene> m_cutscene;
	bool m_cutscenePlaying = false;
	bool m_segmentLooping = false;
	size_t m_loopStartMs = 0;
	size_t m_loopEndMs = 0;
	size_t m_lastTimeMs;
};

#endif // CUTSCENEWIDGET_HPP
