#include "EditorUtils.hpp"
#include <NovelTea/ProjectData.hpp>
#include <QTextBlock>
#include <QPainter>
#include <QIcon>
#include <iostream>

QString EditorUtils::escape(const QString &s)
{
	auto result = s;
	return result.replace("\n", "\\n").replace("\t", "\\t");
}

QString EditorUtils::unescape(const QString &s)
{
	auto result = s;
	return result.replace("\\n", "\n").replace("\\t", "\t");
}

std::string EditorUtils::getFileContents(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
		return "";
	std::string data;
	data.resize(file.size());
	file.read(&data[0], data.size());
	return data;
}

QIcon EditorUtils::iconFromTabType(EditorTabWidget::Type type)
{
	QColor color;
	QPixmap pixmap(":/icons/bg.png");

	switch (type) {
	case EditorTabWidget::Action:
		color = Qt::magenta;
		break;
	case EditorTabWidget::Cutscene:
		color = Qt::black;
		break;
	case EditorTabWidget::Dialogue:
		color = Qt::cyan;
		break;
	case EditorTabWidget::Map:
		color = Qt::gray;
		break;
	case EditorTabWidget::Object:
		color = Qt::red;
		break;
	case EditorTabWidget::Room:
		color = Qt::blue;
		break;
	case EditorTabWidget::Script:
		color = Qt::yellow;
		break;
	case EditorTabWidget::Verb:
		color = Qt::green;
		break;
	default:
		return QIcon();
	}

	color.setAlpha(250);
	pixmap.fill(color);

	auto image = QImage(QString(":/icons/%1.png").arg(type));
	QPainter p;
	p.begin(&pixmap);
	p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
	p.drawImage(0, 0, image);
	p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
	// Exclude shadow for the black icon
	if (type != EditorTabWidget::Cutscene)
		p.drawImage(2, 2, image);
	p.end();

	return QIcon(pixmap);
}

QString EditorUtils::documentToBBCode(const QTextDocument *doc)
{
	QString result;
	return result;
}

QTextDocument *EditorUtils::documentFromBBCode(const QString &bbstring)
{
	auto doc = new QTextDocument;
	auto cursor = QTextCursor{doc};
	auto firstBlock = true;
	QFont defaultFont("DejaVu Sans", 12);
	QTextBlockFormat blockFormat;

	doc->setDefaultFont(defaultFont);

	return doc;
}

/*
json EditorUtils::documentToJson(const QTextDocument *doc)
{
	json j = sj::Array();
	int fmtIndexLast = -1;

	json jblock;

	for (auto block = doc->begin(); block != doc->end(); block = block.next())
	{
		if (block.isValid())
		{
			json jfrag = sj::Array(0);
			auto sfrag = std::string();
			jblock = sj::Array(0, sj::Array());
			for (auto it = block.begin(); !it.atEnd(); ++it)
			{
				QTextFragment fragment = it.fragment();
				if (fragment.isValid())
				{
					auto format = toTextFormat(fragment.charFormat());

					sfrag += fragment.text().toStdString();
				}
			}

			fmtIndexLast = -1;

			jfrag[1] = sfrag;
			jblock[1].append(jfrag);
			j.append(jblock);
		}
	}

	return j;
}

QTextDocument *EditorUtils::jsonToDocument(const json &j)
{
	auto doc = new QTextDocument;
	auto cursor = QTextCursor{doc};
	auto firstBlock = true;
	QFont defaultFont("DejaVu Sans", 12);
	QTextBlockFormat blockFormat;

	doc->setDefaultFont(defaultFont);

	for (auto &jblock : j.ArrayRange())
	{
		blockFormat.setAlignment(Qt::AlignLeft);
		if (firstBlock)
		{
			firstBlock = false;
			cursor.setBlockFormat(blockFormat);
		}
		else
			cursor.insertBlock(blockFormat);

		for (auto &jfrag : jblock[1].ArrayRange())
		{
		}
	}
	return doc;
}

QString EditorUtils::activeTextToString(const NovelTea::ActiveText &activeText)
{
	QString result;
	bool processedFirstBlock = false;
	for (auto &block : activeText.blocks())
	{
		if (processedFirstBlock)
			result += " | ";
		processedFirstBlock = true;
		for (auto &frag : block->fragments())
			result += QString::fromStdString(frag->getText());
	}
	return result;
}

NovelTea::TextFormat EditorUtils::toTextFormat(const QTextCharFormat &format)
{
	NovelTea::TextFormat fmt;
	int size = format.fontPointSize();

	fmt.size(size == 0 ? 12 : size);
	fmt.bold(format.fontWeight() == QFont::Bold);
	fmt.italic(format.fontItalic());
	fmt.underline(format.fontUnderline());

	auto c = format.foreground().color();
	fmt.color(sf::Color(c.red(), c.green(), c.blue(), c.alpha()));

	return fmt;
}

QTextCharFormat EditorUtils::toQTextCharFormat(const NovelTea::TextFormat &format)
{
	QTextCharFormat fmt;

	fmt.setFontPointSize(format.size());
	fmt.setFontWeight(format.bold() ? QFont::Bold : QFont::Normal);
	fmt.setFontItalic(format.italic());
	fmt.setFontUnderline(format.underline());

	return fmt;
}
*/
