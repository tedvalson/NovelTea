#ifndef SCRIPTEDIT_HPP
#define SCRIPTEDIT_HPP

#include <QPlainTextEdit>

class SyntaxHighlighter;

class ScriptEdit : public QPlainTextEdit
{
	Q_OBJECT

public:
	explicit ScriptEdit(QWidget *parent = 0);
	~ScriptEdit();

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth();

	bool checkErrors();

protected:
	void resizeEvent(QResizeEvent *event) override;
	void processErrorMsg(const std::string &error);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(const QRect &, int);

private:
	QWidget *lineNumberArea;
	SyntaxHighlighter *syntaxHighlighter;
	int lineWithError;
};

class LineNumberArea : public QWidget
{
public:
	LineNumberArea(ScriptEdit *editor) : QWidget(editor) {
		scriptEdit = editor;
	}

	QSize sizeHint() const {
		return QSize(scriptEdit->lineNumberAreaWidth(), 0);
	}

protected:
	void paintEvent(QPaintEvent *event) {
		scriptEdit->lineNumberAreaPaintEvent(event);
	}

private:
	ScriptEdit *scriptEdit;
};

#endif // SCRIPTEDIT_HPP
