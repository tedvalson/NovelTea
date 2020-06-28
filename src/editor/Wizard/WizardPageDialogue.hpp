#ifndef WIZARDPAGEDIALOGUE_HPP
#define WIZARDPAGEDIALOGUE_HPP

#include <QWizardPage>

namespace Ui {
class WizardPageDialogue;
}

class WizardPageDialogue : public QWizardPage
{
	Q_OBJECT

public:
	explicit WizardPageDialogue(QWidget *parent = 0);
	~WizardPageDialogue();

	int nextId() const override;

	bool validatePage() override;

private:
	Ui::WizardPageDialogue *ui;
};

#endif // WIZARDPAGEDIALOGUE_HPP
