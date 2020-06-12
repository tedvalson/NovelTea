#ifndef SYNTAXHIGHLIGHTER_HPP
#define SYNTAXHIGHLIGHTER_HPP

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class SyntaxHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	SyntaxHighlighter(QTextDocument *parent = 0);

protected:
	void highlightBlock(const QString &text) override;

private:
};

#endif // SYNTAXHIGHLIGHTER_HPP
