#include "EditorUtils.hpp"
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/TextBlock.hpp>
#include <NovelTea/TextFragment.hpp>
#include <QTextBlock>
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
					auto fmtIndex = NovelTea::ProjectData::instance().addTextFormat(format);

					if (fmtIndex != fmtIndexLast)
					{
						if (fmtIndexLast >= 0)
						{
							jfrag[1] = sfrag;
							jblock[1].append(jfrag);
						}
						fmtIndexLast = fmtIndex;
						jfrag[0] = fmtIndex;
						sfrag.clear();
					}

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
			auto textformat = toQTextCharFormat(Proj.textFormat(jfrag[0].ToInt()));
			cursor.insertText(QString::fromStdString(jfrag[1].ToString()), textformat);
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
