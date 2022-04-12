#ifndef WIZARDPAGEMAP_HPP
#define WIZARDPAGEMAP_HPP

#include <QWizardPage>

namespace Ui {
class WizardPageMap;
}

class WizardPageMap : public QWizardPage
{
	Q_OBJECT

public:
	explicit WizardPageMap(QWidget *parent = 0);
	~WizardPageMap();

	int nextId() const override;

	bool validatePage() override;

private:
	Ui::WizardPageMap *ui;
};

#endif // WIZARDPAGEMAP_HPP
