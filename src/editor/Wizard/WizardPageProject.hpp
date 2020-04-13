#ifndef WIZARDPAGEPROJECT_HPP
#define WIZARDPAGEPROJECT_HPP

#include <QWizardPage>

namespace Ui {
class WizardPageProject;
}

class WizardPageProject : public QWizardPage
{
	Q_OBJECT

public:
	explicit WizardPageProject(QWidget *parent = 0);
	~WizardPageProject();

	int nextId() const override;

private:
	Ui::WizardPageProject *ui;
};

#endif // WIZARDPAGEPROJECT_HPP
