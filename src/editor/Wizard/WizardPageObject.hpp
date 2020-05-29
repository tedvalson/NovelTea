#ifndef WIZARDPAGEOBJECT_HPP
#define WIZARDPAGEOBJECT_HPP

#include <QWizardPage>

namespace Ui {
class WizardPageObject;
}

class WizardPageObject : public QWizardPage
{
	Q_OBJECT

public:
	explicit WizardPageObject(QWidget *parent = 0);
	~WizardPageObject();

	int nextId() const override;

	bool validatePage() override;

private:
	Ui::WizardPageObject *ui;
};

#endif // WIZARDPAGEOBJECT_HPP
