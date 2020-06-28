#ifndef WIZARD_H
#define WIZARD_H

#include <QWizard>

namespace Ui {
class Wizard;
}

class Wizard : public QWizard
{
	Q_OBJECT
public:
	enum Page {
		Start = 0,
		Project = 1,
		Action,
		Cutscene,
		Dialogue,
		Room,
		Object,
		Script,
		Verb,
	};

	explicit Wizard(QWidget *parent = 0);
	~Wizard();
	void accept() override;
};

#endif // WIZARD_H
