#ifndef ACTIONBUILDWIDGET_HPP
#define ACTIONBUILDWIDGET_HPP

#include <NovelTea/ContextObject.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <QWidget>
#include <QComboBox>
#include <NovelTea/json.hpp>

namespace Ui {
class ActionBuildWidget;
}
class TreeModel;

class ActionBuildWidget : public QWidget, public NovelTea::ContextObject
{
	Q_OBJECT

public:
	explicit ActionBuildWidget(NovelTea::Context *context, QWidget *parent = 0);
	~ActionBuildWidget();

	void setValue(sj::JSON value);
	sj::JSON getValue() const;

public slots:
	void renamed(NovelTea::EntityType entityType, const std::string &oldValue, const std::string &newValue);
	void refresh();

private slots:
	void on_comboVerb_currentIndexChanged(const QString &value);
	void comboBox_currentIndexChanged(const QString &value);

signals:
	void valueChanged(sj::JSON value);

private:
	bool isValid() const;
	void fillVerbs(const TreeModel *model, const QModelIndex &index);
	void fillObjects(const TreeModel *model, const QModelIndex &index);

	Ui::ActionBuildWidget *ui;
	sj::JSON m_value;

	std::vector<QComboBox*> m_comboBoxes;
	QStringList m_objectStrings;
};

#endif // ACTIONBUILDWIDGET_HPP
