#include "ScriptEdit.hpp"
#include "SyntaxHighlighter.hpp"
#include <NovelTea/ScriptManager.hpp>
#include <QTextBlock>
#include <QPainter>
#include <QFont>
#include <iostream>

ScriptEdit::ScriptEdit(QWidget *parent)
	: QPlainTextEdit(parent)
	, m_syntaxHighlighter(new SyntaxHighlighter(document()))
	, m_lineWithError(-1)
{
	m_lineNumberArea = new LineNumberArea(this);

	// Set some fixed properties
	auto fnt = font();
	fnt.setStyleHint(QFont::Monospace);
	fnt.setFamily(fnt.defaultFamily());
	setFont(fnt);
	setTabStopWidth(fontMetrics().width(QLatin1Char(' ')) * 4);

	setMouseTracking(true);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
}

ScriptEdit::~ScriptEdit()
{
	delete m_syntaxHighlighter;
}

int ScriptEdit::lineNumberAreaWidth()
{
	int digits = 1;
	int max = qMax(1, document()->blockCount());
	while (max >= 10)
	{
		max /= 10;
		++digits;
	}

	int space = 6 + fontMetrics().width(QLatin1Char('9')) * digits;

	return space;
}

bool ScriptEdit::checkErrors()
{
	auto result = false;
	try
	{
		m_scriptManager.runInClosure(toPlainText().toStdString());
		m_lineWithError = -1;
		result = true;
	}
	catch (DukException &e)
	{
		processErrorMsg(e.what());
	}

	repaint();
	return result;
}

bool ScriptEdit::event(QEvent *event)
{
	if (event->type() == QEvent::ToolTip)
	{
		auto helpEvent = static_cast<QHelpEvent*>(event);
	}

	return QPlainTextEdit::event(event);
}

void ScriptEdit::updateLineNumberAreaWidth(int /* newBlockCount */)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void ScriptEdit::updateLineNumberArea(const QRect &rect, int dy)
{
	if (dy)
		m_lineNumberArea->scroll(0, dy);
	else
		m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

void ScriptEdit::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void ScriptEdit::processErrorMsg(const std::string &error)
{
	// Get error line number from DukException string, example:
	//
	// ReferenceError: identifier 'v' undefined
	// at [anon] (eval:3)
	const auto rx = QRegExp("\\(eval:(\\d+)\\)");

	m_lineWithError = -1;
	m_errorMessage = error;
	if (rx.indexIn(QString::fromStdString(error)) > -1)
		m_lineWithError = rx.cap(1).toInt();

	std::cout << error << std::endl;
}

void ScriptEdit::highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly())
	{
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
	QPainter painter(m_lineNumberArea);
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

			if (block.blockNumber() == m_lineWithError - 1)
				painter.fillRect(0, top, m_lineNumberArea->width(), fontMetrics().height(), QColor(Qt::red).lighter(150));

			painter.drawText(0, top, m_lineNumberArea->width() - 3, fontMetrics().height(), Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + blockBoundingRect(block).height();
		++blockNumber;
	}
}
