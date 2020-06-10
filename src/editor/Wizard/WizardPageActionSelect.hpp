#ifndef WIZARDPAGEACTIONSELECT_HPP
#define WIZARDPAGEACTIONSELECT_HPP

#include <QWizardPage>
#include <QModelIndex>
#include <json.hpp>

namespace Ui {
class WizardPageActionSelect;
}

class WizardPageActionSelect : public QWizardPage
{
	Q_OBJECT

public:
	explicit WizardPageActionSelect(QWidget *parent = 0);
	~WizardPageActionSelect();

	void setValue(nlohmann::json value);
	nlohmann::json getValue() const;

	void setFilterRegExp(const QString &pattern);
	void allowCustomScript(bool allow);

	bool isComplete() const override;

protected:
	void timerEvent(QTimerEvent*) override;

private slots:
	void on_radioExisting_toggled(bool checked);
	void on_radioCustom_toggled(bool checked);

private:
	Ui::WizardPageActionSelect *ui;
	QModelIndex currentIndex;
};

#endif // WIZARDPAGEACTIONSELECT_HPP
