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

	void setValue(const NovelTea::ActiveText &text);
	NovelTea::ActiveText getValue() const;

	static QTextDocument *activeTextToDocument(const NovelTea::ActiveText &activeText);
	static NovelTea::ActiveText documentToActiveText(const QTextDocument *doc);

protected:
	void fontChanged(const QFont &font);
	void colorChanged(const QColor &color);

private slots:
	void on_actionFinish_triggered();
	void on_actionBold_triggered();
	void on_actionItalic_triggered();
	void on_actionUnderline_triggered();
	void on_textEdit_currentCharFormatChanged(const QTextCharFormat &format);

signals:
	void invoked();
	void saved(const NovelTea::ActiveText &data);
	void canceled();
	
private:
	Ui::RichTextEditor *ui;
	NovelTea::ActiveText m_activeText;
};

#endif // RICHTEXTEDITOR_HPP
