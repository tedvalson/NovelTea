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

	bool isComplete() const override;

protected:
	void timerEvent(QTimerEvent*) override;

private:
	Ui::WizardPageActionSelect *ui;
	QModelIndex currentIndex;
};

#endif // WIZARDPAGEACTIONSELECT_HPP
