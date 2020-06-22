#include "ScriptEdit.hpp"
#include <NovelTea/ScriptManager.hpp>
#include <QWhatsThis>
#include <QTextBlock>
#include <QPainter>
#include <QFont>
#include <iostream>

struct BlockInfo {
	int position;
	int number;
	bool foldable: 1;
	bool folded : 1;
};

Q_DECLARE_TYPEINFO(BlockInfo, Q_PRIMITIVE_TYPE);

class SidebarWidget : public QWidget
{
public:
	SidebarWidget(ScriptEdit *editor);
	QVector<BlockInfo> lineNumbers;
	QColor backgroundColor;
	QColor lineNumberColor;
	QColor lineNumberActiveColor;
	QColor indicatorColor;
	QColor foldIndicatorColor;
	QFont font;
	int foldIndicatorWidth;
	int lineNumberPadding;
	QPixmap rightArrowIcon;
	QPixmap downArrowIcon;
protected:
	void mousePressEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);
};

SidebarWidget::SidebarWidget(ScriptEdit *editor)
: QWidget(editor)
, foldIndicatorWidth(0)
, lineNumberPadding(4)
{
	backgroundColor = Qt::lightGray;
	lineNumberColor = Qt::gray;
	lineNumberActiveColor = Qt::black;
	indicatorColor = Qt::white;
	foldIndicatorColor = Qt::lightGray;
}

void SidebarWidget::mousePressEvent(QMouseEvent *event)
{
	if (foldIndicatorWidth > 0) {
		int xofs = width() - foldIndicatorWidth;
		int lineNo = -1;
		int fh = fontMetrics().lineSpacing();
		int ys = event->pos().y();
		if (event->pos().x() > xofs) {
			foreach (BlockInfo ln, lineNumbers)
				if (ln.position < ys && (ln.position + fh) > ys) {
					if (ln.foldable)
						lineNo = ln.number;
					break;
				}
		}
		if (lineNo >= 0) {
			ScriptEdit *editor = qobject_cast<ScriptEdit*>(parent());
			if (editor)
				editor->toggleFold(lineNo);
		}
	}
}

void SidebarWidget::paintEvent(QPaintEvent *event)
{
	auto editor = qobject_cast<ScriptEdit*>(parent());
	QPainter p(this);
	p.fillRect(event->rect(), backgroundColor);
	p.setFont(font);
	int fh = QFontMetrics(font).height();

	for (auto &ln : lineNumbers)
	{
		if (ln.number == editor->textCursor().blockNumber() + 1)
			p.setPen(lineNumberActiveColor);
		else
			p.setPen(lineNumberColor);

		if (ln.number == editor->getLineWithError())
			p.fillRect(0, ln.position, width(), fh, QColor(Qt::red).lighter(150));

		p.drawText(0, ln.position, width() - foldIndicatorWidth - lineNumberPadding, fh, Qt::AlignRight, QString::number(ln.number));
	}

	if (foldIndicatorWidth > 0) {
		int xofs = width() - foldIndicatorWidth;
		p.fillRect(xofs, 0, foldIndicatorWidth, height(), indicatorColor);

		// initialize (or recreate) the arrow icons whenever necessary
		if (foldIndicatorWidth != rightArrowIcon.width()) {
			QPainter iconPainter;
			QPolygonF polygon;

			int dim = foldIndicatorWidth;
			rightArrowIcon = QPixmap(dim, dim);
			rightArrowIcon.fill(Qt::transparent);
			downArrowIcon = rightArrowIcon;

			polygon << QPointF(dim * 0.4, dim * 0.25);
			polygon << QPointF(dim * 0.4, dim * 0.75);
			polygon << QPointF(dim * 0.8, dim * 0.5);
			iconPainter.begin(&rightArrowIcon);
			iconPainter.setRenderHint(QPainter::Antialiasing);
			iconPainter.setPen(Qt::NoPen);
			iconPainter.setBrush(foldIndicatorColor);
			iconPainter.drawPolygon(polygon);
			iconPainter.end();

			polygon.clear();
			polygon << QPointF(dim * 0.25, dim * 0.4);
			polygon << QPointF(dim * 0.75, dim * 0.4);
			polygon << QPointF(dim * 0.5, dim * 0.8);
			iconPainter.begin(&downArrowIcon);
			iconPainter.setRenderHint(QPainter::Antialiasing);
			iconPainter.setPen(Qt::NoPen);
			iconPainter.setBrush(foldIndicatorColor);
			iconPainter.drawPolygon(polygon);
			iconPainter.end();
		}

		for (auto &ln : lineNumbers)
			if (ln.foldable) {
				if (ln.folded)
					p.drawPixmap(xofs, ln.position, rightArrowIcon);
				else
					p.drawPixmap(xofs, ln.position, downArrowIcon);
			}
	}
}

static int findClosingMatch(const QTextDocument *doc, int cursorPosition)
{
	QTextBlock block = doc->findBlock(cursorPosition);
	BlockData *blockData = reinterpret_cast<BlockData*>(block.userData());
	if (!blockData->bracketPositions.isEmpty()) {
		int depth = 1;
		while (block.isValid()) {
			blockData = reinterpret_cast<BlockData*>(block.userData());
			if (blockData && !blockData->bracketPositions.isEmpty()) {
				for (int c = 0; c < blockData->bracketPositions.count(); ++c) {
					int absPos = block.position() + blockData->bracketPositions.at(c);
					if (absPos <= cursorPosition)
						continue;
					if (doc->characterAt(absPos) == '{')
						depth++;
					else
						depth--;
					if (depth == 0)
						return absPos;
				}
			}
			block = block.next();
		}
	}
	return -1;
}

static int findOpeningMatch(const QTextDocument *doc, int cursorPosition)
{
	QTextBlock block = doc->findBlock(cursorPosition);
	BlockData *blockData = reinterpret_cast<BlockData*>(block.userData());
	if (!blockData->bracketPositions.isEmpty()) {
		int depth = 1;
		while (block.isValid()) {
			blockData = reinterpret_cast<BlockData*>(block.userData());
			if (blockData && !blockData->bracketPositions.isEmpty()) {
				for (int c = blockData->bracketPositions.count() - 1; c >= 0; --c) {
					int absPos = block.position() + blockData->bracketPositions.at(c);
					if (absPos >= cursorPosition - 1)
						continue;
					if (doc->characterAt(absPos) == '}')
						depth++;
					else
						depth--;
					if (depth == 0)
						return absPos;
				}
			}
			block = block.previous();
		}
	}
	return -1;
}


class DocLayout: public QPlainTextDocumentLayout
{
public:
	DocLayout(QTextDocument *doc);
	void forceUpdate();
};

DocLayout::DocLayout(QTextDocument *doc)
: QPlainTextDocumentLayout(doc)
{
}

void DocLayout::forceUpdate()
{
	emit documentSizeChanged(documentSize());
}

class ScriptEditPrivate
{
public:
	ScriptEdit *editor;
	DocLayout *layout;
	SyntaxHighlighter *highlighter;
	SidebarWidget *sidebar;
	bool showLineNumbers;
	bool textWrap;
	QColor cursorColor;
	bool bracketsMatching;
	QList<int> matchPositions;
	QColor bracketMatchColor;
	QList<int> errorPositions;
	QColor bracketErrorColor;
	bool codeFolding : 1;

	int lineWithError;
	std::string errorMessage;
	NovelTea::ScriptManager scriptManager;
};

ScriptEdit::ScriptEdit(QWidget *parent)
: QPlainTextEdit(parent)
, d_ptr(new ScriptEditPrivate)
{
	d_ptr->editor = this;
	d_ptr->layout = new DocLayout(document());
	d_ptr->highlighter = new SyntaxHighlighter(document());
	d_ptr->sidebar = new SidebarWidget(this);
	d_ptr->showLineNumbers = true;
	d_ptr->textWrap = true;
	d_ptr->bracketsMatching = true;
	d_ptr->cursorColor = QColor(255, 255, 192);
	d_ptr->bracketMatchColor = QColor(180, 238, 180);
	d_ptr->bracketErrorColor = QColor(224, 128, 128);
	d_ptr->codeFolding = true;

	document()->setDocumentLayout(d_ptr->layout);

	// Set some fixed properties
	auto fnt = font();
	fnt.setStyleHint(QFont::Monospace);
	fnt.setFamily(fnt.defaultFamily());
	setFont(fnt);
	setTabStopWidth(fontMetrics().width(QLatin1Char(' ')) * 4);

	setMouseTracking(true);

	// Updates active line number color
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateSidebar()));

	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateCursor()));
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateSidebar()));
	connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateSidebar(QRect, int)));
}

ScriptEdit::~ScriptEdit()
{
	delete d_ptr->layout;
}

void ScriptEdit::setColor(SyntaxHighlighter::ColorComponent component, const QColor &color)
{
	Q_D(ScriptEdit);

	if (component == SyntaxHighlighter::Background) {
		QPalette pal = palette();
		pal.setColor(QPalette::Base, color);
		setPalette(pal);
		d->sidebar->indicatorColor = color;
		updateSidebar();
	} else if (component == SyntaxHighlighter::Normal) {
		QPalette pal = palette();
		pal.setColor(QPalette::Text, color);
		setPalette(pal);
	} else if (component == SyntaxHighlighter::Sidebar) {
		d->sidebar->backgroundColor = color;
		updateSidebar();
	} else if (component == SyntaxHighlighter::LineNumber) {
		d->sidebar->lineNumberColor = color;
		updateSidebar();
	} else if (component == SyntaxHighlighter::LineNumberActive) {
		d->sidebar->lineNumberActiveColor = color;
		updateSidebar();
	} else if (component == SyntaxHighlighter::Cursor) {
		d->cursorColor = color;
		updateCursor();
	} else if (component == SyntaxHighlighter::BracketMatch) {
		d->bracketMatchColor = color;
		updateCursor();
	} else if (component == SyntaxHighlighter::BracketError) {
		d->bracketErrorColor = color;
		updateCursor();
	} else if (component == SyntaxHighlighter::FoldIndicator) {
		d->sidebar->foldIndicatorColor = color;
		updateSidebar();
	} else {
		d->highlighter->setColor(component, color);
		updateCursor();
	}
}

QStringList ScriptEdit::keywords() const
{
	return d_ptr->highlighter->keywords();
}

void ScriptEdit::setKeywords(const QStringList &keywords)
{
	d_ptr->highlighter->setKeywords(keywords);
}

int ScriptEdit::getLineWithError() const
{
	return d_ptr->lineWithError;
}

bool ScriptEdit::isLineNumbersVisible() const
{
	return d_ptr->showLineNumbers;
}

void ScriptEdit::setLineNumbersVisible(bool visible)
{
	d_ptr->showLineNumbers = visible;
	updateSidebar();
}

bool ScriptEdit::isTextWrapEnabled() const
{
	return d_ptr->textWrap;
}

void ScriptEdit::setTextWrapEnabled(bool enable)
{
	d_ptr->textWrap = enable;
	setLineWrapMode(enable ? WidgetWidth : NoWrap);
}

bool ScriptEdit::isBracketsMatchingEnabled() const
{
	return d_ptr->bracketsMatching;
}

void ScriptEdit::setBracketsMatchingEnabled(bool enable)
{
	d_ptr->bracketsMatching = enable;
	updateCursor();
}

bool ScriptEdit::isCodeFoldingEnabled() const
{
	return d_ptr->codeFolding;
}

void ScriptEdit::setCodeFoldingEnabled(bool enable)
{
	d_ptr->codeFolding = enable;
	updateSidebar();
}

static int findClosingConstruct(const QTextBlock &block)
{
	if (!block.isValid())
		return -1;
	BlockData *blockData = reinterpret_cast<BlockData*>(block.userData());
	if (!blockData)
		return -1;
	if (blockData->bracketPositions.isEmpty())
		return -1;
	const QTextDocument *doc = block.document();
	int offset = block.position();
	foreach (int pos, blockData->bracketPositions) {
		int absPos = offset + pos;
		if (doc->characterAt(absPos) == '{') {
			int matchPos = findClosingMatch(doc, absPos);
			if (matchPos >= 0)
				return matchPos;
		}
	}
	return -1;
}

bool ScriptEdit::isFoldable(int line) const
{
	int matchPos = findClosingConstruct(document()->findBlockByNumber(line - 1));
	if (matchPos >= 0) {
		QTextBlock matchBlock = document()->findBlock(matchPos);
		if (matchBlock.isValid() && matchBlock.blockNumber() > line)
			return true;
	}
	return false;
}

bool ScriptEdit::isFolded(int line) const
{
	QTextBlock block = document()->findBlockByNumber(line - 1);
	if (!block.isValid())
		return false;
	block = block.next();
	if (!block.isValid())
		return false;
	return !block.isVisible();
}

void ScriptEdit::fold(int line)
{
	QTextBlock startBlock = document()->findBlockByNumber(line - 1);
	int endPos = findClosingConstruct(startBlock);
	if (endPos < 0)
		return;
	QTextBlock endBlock = document()->findBlock(endPos);

	QTextBlock block = startBlock.next();
	while (block.isValid() && block != endBlock) {
		block.setVisible(false);
		block.setLineCount(0);
		block = block.next();
	}

	document()->markContentsDirty(startBlock.position(), endPos - startBlock.position() + 1);
	updateSidebar();
	update();

	DocLayout *layout = reinterpret_cast<DocLayout*>(document()->documentLayout());
	layout->forceUpdate();
}

void ScriptEdit::unfold(int line)
{
	QTextBlock startBlock = document()->findBlockByNumber(line - 1);
	int endPos = findClosingConstruct(startBlock);

	QTextBlock block = startBlock.next();
	while (block.isValid() && !block.isVisible()) {
		block.setVisible(true);
		block.setLineCount(block.layout()->lineCount());
		endPos = block.position() + block.length();
		block = block.next();
	}

	document()->markContentsDirty(startBlock.position(), endPos - startBlock.position() + 1);
	updateSidebar();
	update();

	DocLayout *layout = reinterpret_cast<DocLayout*>(document()->documentLayout());
	layout->forceUpdate();
}

void ScriptEdit::toggleFold(int line)
{
	if (isFolded(line))
		unfold(line);
	else
		fold(line);
}

void ScriptEdit::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);
	updateSidebar();
}


void ScriptEdit::updateCursor()
{
	Q_D(ScriptEdit);

	if (isReadOnly()) {
		setExtraSelections(QList<QTextEdit::ExtraSelection>());
	} else {

		d->matchPositions.clear();
		d->errorPositions.clear();

		if (d->bracketsMatching && textCursor().block().userData()) {
			QTextCursor cursor = textCursor();
			int cursorPosition = cursor.position();

			if (document()->characterAt(cursorPosition) == '{') {
				int matchPos = findClosingMatch(document(), cursorPosition);
				if (matchPos < 0) {
					d->errorPositions += cursorPosition;
				} else {
					d->matchPositions += cursorPosition;
					d->matchPositions += matchPos;
				}
			}

			if (document()->characterAt(cursorPosition - 1) == '}') {
				int matchPos = findOpeningMatch(document(), cursorPosition);
				if (matchPos < 0) {
					d->errorPositions += cursorPosition - 1;
				} else {
					d->matchPositions += cursorPosition - 1;
					d->matchPositions += matchPos;
				}
			}
		}

		QTextEdit::ExtraSelection highlight;
		highlight.format.setBackground(d->cursorColor);
		highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
		highlight.cursor = textCursor();
		highlight.cursor.clearSelection();

		QList<QTextEdit::ExtraSelection> extraSelections;
		extraSelections.append(highlight);

		for (int i = 0; i < d->matchPositions.count(); ++i) {
			int pos = d->matchPositions.at(i);
			QTextEdit::ExtraSelection matchHighlight;
			matchHighlight.format.setBackground(d->bracketMatchColor);
			matchHighlight.cursor = textCursor();
			matchHighlight.cursor.setPosition(pos);
			matchHighlight.cursor.setPosition(pos + 1, QTextCursor::KeepAnchor);
			extraSelections.append(matchHighlight);
		}

		for (int i = 0; i < d->errorPositions.count(); ++i) {
			int pos = d->errorPositions.at(i);
			QTextEdit::ExtraSelection errorHighlight;
			errorHighlight.format.setBackground(d->bracketErrorColor);
			errorHighlight.cursor = textCursor();
			errorHighlight.cursor.setPosition(pos);
			errorHighlight.cursor.setPosition(pos + 1, QTextCursor::KeepAnchor);
			extraSelections.append(errorHighlight);
		}

		setExtraSelections(extraSelections);
	}
}

void ScriptEdit::updateSidebar(const QRect &rect, int d)
{
	Q_UNUSED(rect)
	if (d != 0)
		updateSidebar();
}

void ScriptEdit::updateSidebar()
{
	Q_D(ScriptEdit);

	if (!d->showLineNumbers && !d->codeFolding) {
		d->sidebar->hide();
		setViewportMargins(0, 0, 0, 0);
		d->sidebar->setGeometry(3, 0, 0, height());
		return;
	}

	d->sidebar->foldIndicatorWidth = 0;
	d->sidebar->font = this->font();
	d->sidebar->show();

	int sw = 0;
	if (d->showLineNumbers) {
		sw =+ d->sidebar->lineNumberPadding * 2;
		int digits = 1;
		int max = qMax(1, document()->blockCount());
		while (max >= 10) {
			max /= 10;
			++digits;
		}
		sw += fontMetrics().width(QLatin1Char('9')) * digits;
	}
	if (d->codeFolding) {
		int fh = fontMetrics().lineSpacing();
		int fw = fontMetrics().width('w');
		d->sidebar->foldIndicatorWidth = qMax(fw, fh);
		sw += d->sidebar->foldIndicatorWidth;
	}
	setViewportMargins(sw, 0, 0, 0);

	d->sidebar->setGeometry(0, 0, sw, height());
	QRectF sidebarRect(0, 0, sw, height());

	QTextBlock block = firstVisibleBlock();
	int index = 0;
	while (block.isValid()) {
		if (block.isVisible()) {
			QRectF rect = blockBoundingGeometry(block).translated(contentOffset());
			if (sidebarRect.intersects(rect)) {
				if (d->sidebar->lineNumbers.count() >= index)
					d->sidebar->lineNumbers.resize(index + 1);
				d->sidebar->lineNumbers[index].position = rect.top();
				d->sidebar->lineNumbers[index].number = block.blockNumber() + 1;
				d->sidebar->lineNumbers[index].foldable = d->codeFolding ? isFoldable(block.blockNumber() + 1) : false;
				d->sidebar->lineNumbers[index].folded = d->codeFolding ? isFolded(block.blockNumber() + 1) : false;
				++index;
			}
			if (rect.top() > sidebarRect.bottom())
				break;
		}
		block = block.next();
	}
	d->sidebar->lineNumbers.resize(index);
	d->sidebar->update();
}

void ScriptEdit::mark(const QString &str, Qt::CaseSensitivity sens)
{
	d_ptr->highlighter->mark(str, sens);
}

template <typename T>
bool ScriptEdit::checkErrors()
{
	auto result = false;
	try
	{
		d_ptr->scriptManager.runInClosure<T>(toPlainText().toStdString());
		d_ptr->lineWithError = -1;
		result = true;
	}
	catch (DukException &e)
	{
		processErrorMsg(e.what());
	}

	repaint();
	return result;
}

template bool ScriptEdit::checkErrors<std::string>();

bool ScriptEdit::event(QEvent *event)
{
	if (event->type() == QEvent::ToolTip)
	{
		auto helpEvent = static_cast<QHelpEvent*>(event);
		return true;
	}
	else if (event->type() == QEvent::WhatsThisClicked)
	{
		return true;
	}

	return QPlainTextEdit::event(event);
}

void ScriptEdit::processErrorMsg(const std::string &error)
{
	// Get error line number from DukException string, example:
	//
	// ReferenceError: identifier 'v' undefined
	// at [anon] (eval:3)
	const auto rx = QRegExp("\\(eval:(\\d+)\\)");

	d_ptr->lineWithError = -1;
	d_ptr->errorMessage = error;
	if (rx.indexIn(QString::fromStdString(error)) > -1)
		d_ptr->lineWithError = rx.cap(1).toInt();

}
