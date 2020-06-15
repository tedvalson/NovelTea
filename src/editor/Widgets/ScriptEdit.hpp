#ifndef SCRIPTEDIT_HPP
#define SCRIPTEDIT_HPP

#include <NovelTea/ScriptManager.hpp>
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
	bool event(QEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
	void processErrorMsg(const std::string &error);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(const QRect &, int);

private:
	QWidget *m_lineNumberArea;
	SyntaxHighlighter *m_syntaxHighlighter;
	int m_lineWithError;
	std::string m_errorMessage;
	NovelTea::ScriptManager m_scriptManager;
};

class LineNumberArea : public QWidget
{
public:
	LineNumberArea(ScriptEdit *editor) : QWidget(editor) {
		m_scriptEdit = editor;
	}

	QSize sizeHint() const {
		return QSize(m_scriptEdit->lineNumberAreaWidth(), 0);
	}

protected:
	void paintEvent(QPaintEvent *event) {
		m_scriptEdit->lineNumberAreaPaintEvent(event);
	}

private:
	ScriptEdit *m_scriptEdit;
};

#endif // SCRIPTEDIT_HPP
