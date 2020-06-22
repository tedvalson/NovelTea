#include "SyntaxHighlighter.hpp"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
: QSyntaxHighlighter(parent)
, m_markCaseSensitivity(Qt::CaseInsensitive)
{
	// default color scheme
	m_colors[ColorComponent::Normal]     = QColor("#000000");
	m_colors[ColorComponent::Comment]    = QColor("#808080");
	m_colors[ColorComponent::Number]     = QColor("#008000");
	m_colors[ColorComponent::String]     = QColor("#800000");
	m_colors[ColorComponent::Operator]   = QColor("#808000");
	m_colors[ColorComponent::Identifier] = QColor("#000020");
	m_colors[ColorComponent::Keyword]    = QColor("#000080");
	m_colors[ColorComponent::BuiltIn]    = QColor("#008080");
	m_colors[ColorComponent::Marker]     = QColor("#ffff00");

	// https://developer.mozilla.org/en/JavaScript/Reference/Reserved_Words
	m_keywords << "break";
	m_keywords << "case";
	m_keywords << "catch";
	m_keywords << "continue";
	m_keywords << "default";
	m_keywords << "delete";
	m_keywords << "do";
	m_keywords << "else";
	m_keywords << "finally";
	m_keywords << "for";
	m_keywords << "function";
	m_keywords << "if";
	m_keywords << "in";
	m_keywords << "instanceof";
	m_keywords << "new";
	m_keywords << "return";
	m_keywords << "switch";
	m_keywords << "this";
	m_keywords << "throw";
	m_keywords << "try";
	m_keywords << "typeof";
	m_keywords << "var";
	m_keywords << "void";
	m_keywords << "while";
	m_keywords << "with";

	m_keywords << "true";
	m_keywords << "false";
	m_keywords << "null";

	// built-in and other popular objects + properties
	m_knownIds << "Object";
	m_knownIds << "prototype";
	m_knownIds << "create";
	m_knownIds << "defineProperty";
	m_knownIds << "defineProperties";
	m_knownIds << "getOwnPropertyDescriptor";
	m_knownIds << "keys";
	m_knownIds << "getOwnPropertyNames";
	m_knownIds << "constructor";
	m_knownIds << "__parent__";
	m_knownIds << "__proto__";
	m_knownIds << "__defineGetter__";
	m_knownIds << "__defineSetter__";
	m_knownIds << "eval";
	m_knownIds << "hasOwnProperty";
	m_knownIds << "isPrototypeOf";
	m_knownIds << "__lookupGetter__";
	m_knownIds << "__lookupSetter__";
	m_knownIds << "__noSuchMethod__";
	m_knownIds << "propertyIsEnumerable";
	m_knownIds << "toSource";
	m_knownIds << "toLocaleString";
	m_knownIds << "toString";
	m_knownIds << "unwatch";
	m_knownIds << "valueOf";
	m_knownIds << "watch";

	m_knownIds << "Function";
	m_knownIds << "arguments";
	m_knownIds << "arity";
	m_knownIds << "caller";
	m_knownIds << "constructor";
	m_knownIds << "length";
	m_knownIds << "name";
	m_knownIds << "apply";
	m_knownIds << "bind";
	m_knownIds << "call";

	m_knownIds << "String";
	m_knownIds << "fromCharCode";
	m_knownIds << "length";
	m_knownIds << "charAt";
	m_knownIds << "charCodeAt";
	m_knownIds << "concat";
	m_knownIds << "indexOf";
	m_knownIds << "lastIndexOf";
	m_knownIds << "localCompare";
	m_knownIds << "match";
	m_knownIds << "quote";
	m_knownIds << "replace";
	m_knownIds << "search";
	m_knownIds << "slice";
	m_knownIds << "split";
	m_knownIds << "substr";
	m_knownIds << "substring";
	m_knownIds << "toLocaleLowerCase";
	m_knownIds << "toLocaleUpperCase";
	m_knownIds << "toLowerCase";
	m_knownIds << "toUpperCase";
	m_knownIds << "trim";
	m_knownIds << "trimLeft";
	m_knownIds << "trimRight";

	m_knownIds << "Array";
	m_knownIds << "isArray";
	m_knownIds << "index";
	m_knownIds << "input";
	m_knownIds << "pop";
	m_knownIds << "push";
	m_knownIds << "reverse";
	m_knownIds << "shift";
	m_knownIds << "sort";
	m_knownIds << "splice";
	m_knownIds << "unshift";
	m_knownIds << "concat";
	m_knownIds << "join";
	m_knownIds << "filter";
	m_knownIds << "forEach";
	m_knownIds << "every";
	m_knownIds << "map";
	m_knownIds << "some";
	m_knownIds << "reduce";
	m_knownIds << "reduceRight";

	m_knownIds << "RegExp";
	m_knownIds << "global";
	m_knownIds << "ignoreCase";
	m_knownIds << "lastIndex";
	m_knownIds << "multiline";
	m_knownIds << "source";
	m_knownIds << "exec";
	m_knownIds << "test";

	m_knownIds << "JSON";
	m_knownIds << "parse";
	m_knownIds << "stringify";

	m_knownIds << "decodeURI";
	m_knownIds << "decodeURIComponent";
	m_knownIds << "encodeURI";
	m_knownIds << "encodeURIComponent";
	m_knownIds << "eval";
	m_knownIds << "isFinite";
	m_knownIds << "isNaN";
	m_knownIds << "parseFloat";
	m_knownIds << "parseInt";
	m_knownIds << "Infinity";
	m_knownIds << "NaN";
	m_knownIds << "undefined";

	m_knownIds << "Math";
	m_knownIds << "E";
	m_knownIds << "LN2";
	m_knownIds << "LN10";
	m_knownIds << "LOG2E";
	m_knownIds << "LOG10E";
	m_knownIds << "PI";
	m_knownIds << "SQRT1_2";
	m_knownIds << "SQRT2";
	m_knownIds << "abs";
	m_knownIds << "acos";
	m_knownIds << "asin";
	m_knownIds << "atan";
	m_knownIds << "atan2";
	m_knownIds << "ceil";
	m_knownIds << "cos";
	m_knownIds << "exp";
	m_knownIds << "floor";
	m_knownIds << "log";
	m_knownIds << "max";
	m_knownIds << "min";
	m_knownIds << "pow";
	m_knownIds << "random";
	m_knownIds << "round";
	m_knownIds << "sin";
	m_knownIds << "sqrt";
	m_knownIds << "tan";

	m_knownIds << "document";
	m_knownIds << "window";
	m_knownIds << "navigator";
	m_knownIds << "userAgent";
}

void SyntaxHighlighter::setColor(SyntaxHighlighter::ColorComponent component, const QColor &color)
{
	m_colors[component] = color;
	rehighlight();
}

void SyntaxHighlighter::mark(const QString &str, Qt::CaseSensitivity caseSensitivity)
{
	m_markString = str;
	m_markCaseSensitivity = caseSensitivity;
	rehighlight();

}

QStringList SyntaxHighlighter::keywords() const
{
	return m_keywords.toList();

}

void SyntaxHighlighter::setKeywords(const QStringList &keywords)
{
	m_keywords = QSet<QString>::fromList(keywords);
	rehighlight();
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
	// parsing state
	enum {
		Start = 0,
		Number = 1,
		Identifier = 2,
		String = 3,
		Comment = 4,
		Regex = 5
	};

	QList<int> bracketPositions;

	int blockState = previousBlockState();
	int bracketLevel = blockState >> 4;
	int state = blockState & 15;
	if (blockState < 0) {
		bracketLevel = 0;
		state = Start;
	}

	int start = 0;
	int i = 0;
	while (i <= text.length()) {
		QChar ch = (i < text.length()) ? text.at(i) : QChar();
		QChar next = (i < text.length() - 1) ? text.at(i + 1) : QChar();

		switch (state) {

		case Start:
			start = i;
			if (ch.isSpace()) {
				++i;
			} else if (ch.isDigit()) {
				++i;
				state = Number;
			} else if (ch.isLetter() || ch == '_') {
				++i;
				state = Identifier;
			} else if (ch == '\'' || ch == '\"') {
				++i;
				state = String;
			} else if (ch == '/' && next == '*') {
				++i;
				++i;
				state = Comment;
			} else if (ch == '/' && next == '/') {
				i = text.length();
				setFormat(start, text.length(), m_colors[ColorComponent::Comment]);
			} else if (ch == '/' && next != '*') {
				++i;
				state = Regex;
			} else {
				if (!QString("(){}[]").contains(ch))
					setFormat(start, 1, m_colors[ColorComponent::Operator]);
				if (ch =='{' || ch == '}') {
					bracketPositions += i;
					if (ch == '{')
						bracketLevel++;
					else
						bracketLevel--;
				}
				++i;
				state = Start;
			}
			break;

		case Number:
			if (ch.isSpace() || !ch.isDigit()) {
				setFormat(start, i - start, m_colors[ColorComponent::Number]);
				state = Start;
			} else {
				++i;
			}
			break;

		case Identifier:
			if (ch.isSpace() || !(ch.isDigit() || ch.isLetter() || ch == '_')) {
				QString token = text.mid(start, i - start).trimmed();
				if (m_keywords.contains(token))
					setFormat(start, i - start, m_colors[ColorComponent::Keyword]);
				else if (m_knownIds.contains(token))
					setFormat(start, i - start, m_colors[ColorComponent::BuiltIn]);
				state = Start;
			} else {
				++i;
			}
			break;

		case String:
			if (ch == text.at(start)) {
				QChar prev = (i > 0) ? text.at(i - 1) : QChar();
				if (prev != '\\') {
					++i;
					setFormat(start, i - start, m_colors[ColorComponent::String]);
					state = Start;
				} else {
					++i;
				}
			} else {
				++i;
			}
			break;

		case Comment:
			if (ch == '*' && next == '/') {
				++i;
				++i;
				setFormat(start, i - start, m_colors[ColorComponent::Comment]);
				state = Start;
			} else {
				++i;
			}
			break;

		case Regex:
			if (ch == '/') {
				QChar prev = (i > 0) ? text.at(i - 1) : QChar();
				if (prev != '\\') {
					++i;
					setFormat(start, i - start, m_colors[ColorComponent::String]);
					state = Start;
				} else {
					++i;
				}
			} else {
				++i;
			}
			break;

		default:
			state = Start;
			break;
		}
	}

	if (state == Comment)
		setFormat(start, text.length(), m_colors[ColorComponent::Comment]);
	else
		state = Start;

	if (!m_markString.isEmpty()) {
		int pos = 0;
		int len = m_markString.length();
		QTextCharFormat markerFormat;
		markerFormat.setBackground(m_colors[ColorComponent::Marker]);
		markerFormat.setForeground(m_colors[ColorComponent::Normal]);
		for (;;) {
			pos = text.indexOf(m_markString, pos, m_markCaseSensitivity);
			if (pos < 0)
				break;
			setFormat(pos, len, markerFormat);
			++pos;
		}
	}

	if (!bracketPositions.isEmpty()) {
		BlockData *blockData = reinterpret_cast<BlockData*>(currentBlock().userData());
		if (!blockData) {
			blockData = new BlockData;
			currentBlock().setUserData(blockData);
		}
		blockData->bracketPositions = bracketPositions;
	}

	blockState = (state & 15) | (bracketLevel << 4);
	setCurrentBlockState(blockState);
}
