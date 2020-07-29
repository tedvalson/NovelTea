#ifndef PROPERTYEDITOR_HPP
#define PROPERTYEDITOR_HPP

#include <QWidget>
#include "QtPropertyBrowser/qtpropertymanager.h"
#include "QtPropertyBrowser/qtvariantproperty.h"
#include <NovelTea/json.hpp>

namespace Ui {
class PropertyEditor;
}

class PropertyEditor : public QWidget
{
	Q_OBJECT

public:
	explicit PropertyEditor(QWidget *parent = 0);
	~PropertyEditor();

	void setValue(sj::JSON value);
	sj::JSON getValue() const;

protected:
	void addProperty(int type);
	void addProperty(int type, const QString &name, const QVariant &value);
	void update();

private slots:
	void on_actionRemoveProperty_triggered();
	void on_actionAddTextProperty_triggered();
	void on_actionAddNumberProperty_triggered();
	void on_actionAddBooleanProperty_triggered();
	void on_propertyBrowser_currentItemChanged(QtBrowserItem *item);

signals:
	void valueChanged(sj::JSON value);

private:
	Ui::PropertyEditor *ui;
	QMenu *m_menuAdd;
	QtVariantPropertyManager *m_variantManager;
	QtVariantEditorFactory *m_variantFactory;

	sj::JSON m_value;
};

#endif // PROPERTYEDITOR_HPP
