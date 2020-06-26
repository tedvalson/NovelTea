#ifndef WIZARDPAGEACTION_HPP
#define WIZARDPAGEACTION_HPP

#include <QWizardPage>

namespace Ui {
class WizardPageAction;
}

class WizardPageAction : public QWizardPage
{
	Q_OBJECT

public:
	explicit WizardPageAction(QWidget *parent = 0);
	~WizardPageAction();

	int nextId() const override;

	bool validatePage() override;

private:
	Ui::WizardPageAction *ui;
};

#endif // WIZARDPAGEACTION_HPP
