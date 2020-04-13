#ifndef WIZARDPAGECUTSCENE_HPP
#define WIZARDPAGECUTSCENE_HPP

#include <QWizardPage>

namespace Ui {
class WizardPageCutscene;
}

class WizardPageCutscene : public QWizardPage
{
	Q_OBJECT

public:
	explicit WizardPageCutscene(QWidget *parent = 0);
	~WizardPageCutscene();

	int nextId() const override;

	bool validatePage() override;

private:
	Ui::WizardPageCutscene *ui;
};

#endif // WIZARDPAGECUTSCENE_HPP
