#ifndef WIZARDPAGEVERB_HPP
#define WIZARDPAGEVERB_HPP

#include <QWizardPage>

namespace Ui {
class WizardPageVerb;
}

class WizardPageVerb : public QWizardPage
{
	Q_OBJECT

public:
	explicit WizardPageVerb(QWidget *parent = 0);
	~WizardPageVerb();

	int nextId() const override;

	bool validatePage() override;

private:
	Ui::WizardPageVerb *ui;
};

#endif // WIZARDPAGEVERB_HPP
