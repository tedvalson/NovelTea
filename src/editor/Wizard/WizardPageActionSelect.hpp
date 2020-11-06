#ifndef WIZARDPAGEACTIONSELECT_HPP
#define WIZARDPAGEACTIONSELECT_HPP

#include <QWizardPage>
#include <QModelIndex>
#include <NovelTea/json.hpp>

namespace Ui {
class WizardPageActionSelect;
}

class WizardPageActionSelect : public QWizardPage
{
	Q_OBJECT

public:
	explicit WizardPageActionSelect(QWidget *parent = 0);
	~WizardPageActionSelect();

	void setValue(sj::JSON value);
	sj::JSON getValue() const;
	QModelIndex getSelectedIndex() const;

	void setFilterRegExp(const QString &pattern);
	void setLabel(const QString &message = QString());
	void allowCustomScript(bool allow);

	bool isComplete() const override;

	static std::string getItemId(const QString &filterRegex, const QString &labelMessage = QString());

protected:
	void timerEvent(QTimerEvent*) override;

private slots:
	void on_radioExisting_toggled(bool checked);
	void on_radioCustom_toggled(bool checked);
	void on_radioNone_toggled(bool checked);

private:
	Ui::WizardPageActionSelect *ui;
	QModelIndex currentIndex;
};

#endif // WIZARDPAGEACTIONSELECT_HPP
