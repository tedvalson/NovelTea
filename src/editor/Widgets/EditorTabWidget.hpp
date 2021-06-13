#ifndef EDITORTABWIDGET_HPP
#define EDITORTABWIDGET_HPP

#include <QWidget>
#include <NovelTea/ProjectDataIdentifiers.hpp>

#define MODIFIER(sender, signal) connect(sender, signal, this, &EditorTabWidget::setModified)

class EditorTabWidget: public QWidget
{
	Q_OBJECT
public:
	enum Type : int {
		Invalid = -1,
		Cutscene = 0,
		Room     = 1,
		Object   = 2,
		Action,
		Dialogue,
		Tests,
		Search,
		Script,
		Settings,
		Verb,
	};

	explicit EditorTabWidget(QWidget *parent = 0);
	virtual ~EditorTabWidget();
	bool isModified() const;
	virtual QString tabText() const = 0;
	virtual Type getType() const = 0;
	virtual QIcon getIcon() const;
	const std::string &idName() const;
	void rename(const std::string &newIdName);

	static Type entityTypeToTabType(NovelTea::EntityType entityType);
	static Type entityTypeToTabType(int entityType);
	static NovelTea::EntityType tabTypeToEntityType(Type tabType);
	static NovelTea::EntityType tabTypeToEntityType(int tabType);

private:
	virtual void saveData() const = 0;
	virtual void loadData() = 0;
public slots:
	void save();
	void load();
	void setModified();
signals:
	void saved();
	void loaded();
	void modified();
protected:
	std::string m_idName;
private:
	bool m_modified = false;
};

#endif // EDITORTABWIDGET_HPP
