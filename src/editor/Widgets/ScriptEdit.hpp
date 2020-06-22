#ifndef SCRIPTEDIT_HPP
#define SCRIPTEDIT_HPP

#include "SyntaxHighlighter.hpp"
#include <QPlainTextEdit>
#include <QScopedPointer>

class ScriptEditPrivate;

class ScriptEdit : public QPlainTextEdit
{
	Q_OBJECT
	Q_PROPERTY(bool bracketsMatchingEnabled READ isBracketsMatchingEnabled WRITE setBracketsMatchingEnabled)
	Q_PROPERTY(bool codeFoldingEnabled READ isCodeFoldingEnabled WRITE setCodeFoldingEnabled)
	Q_PROPERTY(bool lineNumbersVisible READ isLineNumbersVisible WRITE setLineNumbersVisible)
	Q_PROPERTY(bool textWrapEnabled READ isTextWrapEnabled WRITE setTextWrapEnabled)

public:
	explicit ScriptEdit(QWidget *parent = 0);
	~ScriptEdit();

	template <typename T>
	bool checkErrors();

	void setColor(SyntaxHighlighter::ColorComponent component, const QColor &color);

	QStringList keywords() const;
	void setKeywords(const QStringList &keywords);

	int getLineWithError() const;

	bool isBracketsMatchingEnabled() const;
	bool isCodeFoldingEnabled() const;
	bool isLineNumbersVisible() const;
	bool isTextWrapEnabled() const;

	bool isFoldable(int line) const;
	bool isFolded(int line) const;

public slots:
	void updateSidebar();
	void mark(const QString &str, Qt::CaseSensitivity sens = Qt::CaseInsensitive);
	void setBracketsMatchingEnabled(bool enable);
	void setCodeFoldingEnabled(bool enable);
	void setLineNumbersVisible(bool visible);
	void setTextWrapEnabled(bool enable);

	void fold(int line);
	void unfold(int line);
	void toggleFold(int line);

protected:
	bool event(QEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
	void processErrorMsg(const std::string &error);

private slots:
	void updateCursor();
	void updateSidebar(const QRect &rect, int d);

private:
	QScopedPointer<ScriptEditPrivate> d_ptr;
	Q_DECLARE_PRIVATE(ScriptEdit);
	Q_DISABLE_COPY(ScriptEdit);
};

#endif // SCRIPTEDIT_HPP
