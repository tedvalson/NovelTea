#ifndef WIZARD_H
#define WIZARD_H

#include <QWizard>

constexpr int cNewStart = 0;
constexpr int cNewProject = 1;
constexpr int cNewCutscene = 2;
constexpr int cNewScript = 3;

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
		Cutscene,
		Script,
	};

	explicit Wizard(QWidget *parent = 0);
	~Wizard();
	void accept() override;
};

#endif // WIZARD_H
