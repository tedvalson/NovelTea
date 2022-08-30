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

	bool addFontFromFile(bool systemFont, const QString &fileName, const QString &alias);
	bool addFontFromData(bool systemFont, const QString &name, const std::string &data, const QString &alias);
	void refreshFontList();

protected:

private:
	bool loadImageData(const std::string &data);
	bool loadImageFile(const QString &fileName);
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
	void on_buttonSelectImage_clicked();
	void on_buttonFontRename_clicked();
	void on_buttonFontDelete_clicked();

private:
	void fillVerbs(const TreeModel *model, const QModelIndex &index);

	Ui::ProjectSettingsWidget *ui;
	QString m_defaultFontAlias;
	std::map<std::string, std::string> m_fontsData;
	std::string m_imageData;
};

#endif // PROJECTSETTINGSWIDGET_HPP
