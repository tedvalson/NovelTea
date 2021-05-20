#ifndef PROJECTSETTINGSWIDGET_HPP
#define PROJECTSETTINGSWIDGET_HPP

#include "EditorTabWidget.hpp"
#include "TreeModel.hpp"
#include <QAbstractItemModel>

namespace Ui {
class ProjectSettingsWidget;
}

class ProjectSettingsWidget : public EditorTabWidget
{
	Q_OBJECT
public:
	explicit ProjectSettingsWidget(QWidget *parent = 0);
	virtual ~ProjectSettingsWidget();

	QString tabText() const override;
	Type getType() const override;

	void addFontBuiltin(const QString &name);
	void addFont(const QString &name);
	void makeFontDefault(int index);

protected:

private:
	void saveData() const override;
	void loadData() override;

public slots:
	void refreshVerbs();

private slots:
	void on_lineEditFontPreview_textChanged(const QString &arg1);
	void on_listFonts_currentRowChanged(int currentRow);
	void on_buttonImportFont_clicked();
	void on_buttonSetDefaultFont_clicked();
	void on_actionAddObject_triggered();
	void on_actionRemoveObject_triggered();
	void on_listInventory_currentRowChanged(int currentRow);

private:
	void fillVerbs(const TreeModel *model, const QModelIndex &index);

	Ui::ProjectSettingsWidget *ui;
	int defaultFontIndex;
};

#endif // PROJECTSETTINGSWIDGET_HPP
