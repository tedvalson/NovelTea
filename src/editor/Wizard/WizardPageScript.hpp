#ifndef WIZARDPAGESCRIPT_HPP
#define WIZARDPAGESCRIPT_HPP

#include <QWizardPage>

namespace Ui {
class WizardPageScript;
}

class WizardPageScript : public QWizardPage
{
	Q_OBJECT

public:
	explicit WizardPageScript(QWidget *parent = 0);
	~WizardPageScript();

	int nextId() const override;

	bool validatePage() override;

private:
	Ui::WizardPageScript *ui;
};

#endif // WIZARDPAGESCRIPT_HPP
