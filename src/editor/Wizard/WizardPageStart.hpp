#ifndef WIZARDPAGESTART_HPP
#define WIZARDPAGESTART_HPP

#include <QWizardPage>

class QButtonGroup;

namespace Ui {
class WizardPageStart;
}

class WizardPageStart : public QWizardPage
{
	Q_OBJECT

	Q_PROPERTY(int typeId READ getTypeId WRITE setTypeId)

public:
	explicit WizardPageStart(QWidget *parent = 0);
	~WizardPageStart();

	int nextId() const override;

	int getTypeId() const;

public slots:
	void setTypeId(int id);

signals:
	void typeIdChanged();

private:
	Ui::WizardPageStart *ui;
	QButtonGroup *newSelectionGroup;
	int typeId;
};

#endif // WIZARDPAGESTART_HPP
