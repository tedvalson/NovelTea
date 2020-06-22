#ifndef SYNTAXHIGHLIGHTER_HPP
#define SYNTAXHIGHLIGHTER_HPP

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class BlockData: public QTextBlockUserData
{
public:
	QList<int> bracketPositions;
};


class SyntaxHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:
	enum ColorComponent {
		Background,
		Normal,
		Comment,
		Number,
		String,
		Operator,
		Identifier,
		Keyword,
		BuiltIn,
		Sidebar,
		LineNumber,
		LineNumberActive,
		Cursor,
		Marker,
		BracketMatch,
		BracketError,
		FoldIndicator
	};

	SyntaxHighlighter(QTextDocument *parent = 0);

	void setColor(ColorComponent component, const QColor &color);
	void mark(const QString &str, Qt::CaseSensitivity caseSensitivity);

	QStringList keywords() const;
	void setKeywords(const QStringList &keywords);

protected:
	void highlightBlock(const QString &text) override;

private:
	QSet<QString> m_keywords;
	QSet<QString> m_knownIds;
	QHash<ColorComponent, QColor> m_colors;
	QString m_markString;
	Qt::CaseSensitivity m_markCaseSensitivity;
};

#endif // SYNTAXHIGHLIGHTER_HPP
