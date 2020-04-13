#ifndef EDITORTABWIDGET_HPP
#define EDITORTABWIDGET_HPP

#include <QWidget>

#define MODIFIER(sender, signal) connect(sender, signal, this, &EditorTabWidget::setModified)

class EditorTabWidget: public QWidget
{
	Q_OBJECT
public:
	enum Type : int {
		Invalid = -1,
		Cutscene = 0,
		Settings = 1,
	};

	explicit EditorTabWidget(QWidget *parent = 0);
	virtual ~EditorTabWidget();
	bool isModified() const;
	virtual QString tabText() const = 0;
	virtual Type getType() const = 0;
	const std::string &idName() const;
	void rename(const std::string &newIdName);
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
	std::string _idName;
private:
	bool _modified = false;
};

#endif // EDITORTABWIDGET_HPP
