#include "ScriptEdit.hpp"
#include "SyntaxHighlighter.hpp"
#include <NovelTea/ScriptManager.hpp>
#include <QTextBlock>
#include <QPainter>
#include <iostream>

ScriptEdit::ScriptEdit(QWidget *parent)
	: QPlainTextEdit(parent)
	, syntaxHighlighter(new SyntaxHighlighter(document()))
	, lineWithError(-1)
{
	lineNumberArea = new LineNumberArea(this);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	setTabStopWidth(fontMetrics().width(QLatin1Char(' ')) * 4);

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
}

ScriptEdit::~ScriptEdit()
{
	delete syntaxHighlighter;
}

int ScriptEdit::lineNumberAreaWidth()
{
	int digits = 1;
	int max = qMax(1, document()->blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}

	int space = 6 + fontMetrics().width(QLatin1Char('9')) * digits;

	return space;
}

bool ScriptEdit::checkErrors()
{
	try
	{
		Script.runInClosure(toPlainText().toStdString());
		lineWithError = -1;
		return true;
	}
	catch (DukException &e)
	{
		processErrorMsg(e.what());
		return false;
	}
}

void ScriptEdit::updateLineNumberAreaWidth(int /* newBlockCount */)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void ScriptEdit::updateLineNumberArea(const QRect &rect, int dy)
{
	if (dy)
		lineNumberArea->scroll(0, dy);
	else
		lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

void ScriptEdit::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void ScriptEdit::processErrorMsg(const std::string &error)
{
	const auto rx = QRegExp("\\(eval:(\\d+)\\)");

	lineWithError = -1;
	if (rx.indexIn(QString::fromStdString(error)) > -1)
		lineWithError = rx.cap(1).toInt();

	std::cout << error << std::endl;
}

void ScriptEdit::highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly()) {
		QTextEdit::ExtraSelection selection;

		QColor lineColor = QColor(Qt::yellow).lighter(180);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}

void ScriptEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	QPainter painter(lineNumberArea);
	painter.fillRect(event->rect(), QColor(200, 200, 200));

	auto block = firstVisibleBlock();
	auto blockNumber = block.blockNumber();
	auto top = blockBoundingGeometry(block).translated(contentOffset()).top();
	auto bottom = top + blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom())
	{
		if (block.isVisible() && bottom >= event->rect().top())
		{
			QString number = QString::number(blockNumber + 1);

			if (block.blockNumber() == textCursor().blockNumber())
				painter.setPen(QColor(0, 0, 0, 250));
			else
				painter.setPen(QColor(0, 0, 0, 80));

			if (block.blockNumber() == lineWithError - 1)
				painter.fillRect(0, top, lineNumberArea->width(), fontMetrics().height(), QColor(Qt::red).lighter(150));

			painter.drawText(0, top, lineNumberArea->width() - 3, fontMetrics().height(), Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + blockBoundingRect(block).height();
		++blockNumber;
	}
}
