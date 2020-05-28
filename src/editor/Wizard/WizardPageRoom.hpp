#ifndef WIZARDPAGEROOM_HPP
#define WIZARDPAGEROOM_HPP

#include <QWizardPage>

namespace Ui {
class WizardPageRoom;
}

class WizardPageRoom : public QWizardPage
{
	Q_OBJECT

public:
	explicit WizardPageRoom(QWidget *parent = 0);
	~WizardPageRoom();

	int nextId() const override;

	bool validatePage() override;

private:
	Ui::WizardPageRoom *ui;
};

#endif // WIZARDPAGEROOM_HPP
