#ifndef RICHTEXTEDITOR_HPP
#define RICHTEXTEDITOR_HPP

#include <QWidget>
#include <QTextCharFormat>
#include <NovelTea/ActiveText.hpp>

namespace Ui {
class RichTextEditor;
}

class RichTextEditor : public QWidget
{
	Q_OBJECT

public:
	explicit RichTextEditor(QWidget *parent = 0);
	~RichTextEditor();

	void mergeFormat(const QTextCharFormat &format);
	void invoke();

	void setValue(const std::shared_ptr<NovelTea::ActiveText> &text);
	std::shared_ptr<NovelTea::ActiveText> getValue() const;

	void setFormattingEnabled(bool value);
	bool getFormattingEnabled() const;

	static QTextDocument *activeTextToDocument(const std::shared_ptr<NovelTea::ActiveText> &activeText);
	static std::shared_ptr<NovelTea::ActiveText> documentToActiveText(const QTextDocument *doc);

protected:
	void fontChanged(const QFont &font);
	void colorChanged(const QColor &color);
	void timerEvent(QTimerEvent *event) override;

private slots:
	void on_actionFinish_triggered();
	void on_actionBold_triggered();
	void on_actionItalic_triggered();
	void on_actionUnderline_triggered();
	void on_textEdit_currentCharFormatChanged(const QTextCharFormat &format);
	void on_spinBox_valueChanged(int arg1);
	void on_textEdit_textChanged();

signals:
	void invoked();
	void saved(const std::shared_ptr<NovelTea::ActiveText> &data);
	void changed(const std::shared_ptr<NovelTea::ActiveText> &data);
	void canceled();
	
private:
	Ui::RichTextEditor *ui;
	std::shared_ptr<NovelTea::ActiveText> m_activeText;
	bool m_isChanged;
	bool m_formattingEnabled;
};

#endif // RICHTEXTEDITOR_HPP
