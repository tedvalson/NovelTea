#include "RichTextEditor.hpp"
#include "ui_RichTextEditor.h"
#include "Utils.hpp"
#include <NovelTea/TextBlock.hpp>
#include <NovelTea/TextFragment.hpp>
#include <NovelTea/ProjectData.hpp>
#include <QTextBlock>
#include <iostream>

RichTextEditor::RichTextEditor(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::RichTextEditor)
{
	ui->setupUi(this);

	ui->textEdit->setText("test test test test");
}

RichTextEditor::~RichTextEditor()
{
	delete ui;
}

void RichTextEditor::mergeFormat(const QTextCharFormat &format)
{
	QTextCursor cursor = ui->textEdit->textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(format);
	ui->textEdit->mergeCurrentCharFormat(format);
}

void RichTextEditor::invoke()
{
	emit invoked();
}

void RichTextEditor::setValue(const std::shared_ptr<NovelTea::ActiveText> &text)
{
	auto doc = activeTextToDocument(text);
	ui->textEdit->setDocument(doc);
}

std::shared_ptr<NovelTea::ActiveText> RichTextEditor::getValue() const
{
	return documentToActiveText(ui->textEdit->document());
}

QTextDocument *RichTextEditor::activeTextToDocument(const std::shared_ptr<NovelTea::ActiveText> &activeText)
{
	auto doc = new QTextDocument;
	auto cursor = QTextCursor{doc};
	auto firstBlock = true;
	QFont defaultFont("DejaVu Sans", 12);
	QTextBlockFormat blockFormat;

	doc->setDefaultFont(defaultFont);

	for (auto &block : activeText->blocks())
	{
		blockFormat.setAlignment(Qt::AlignLeft);
		if (firstBlock)
		{
			firstBlock = false;
			cursor.setBlockFormat(blockFormat);
		}
		else
			cursor.insertBlock(blockFormat);

		for (auto &fragment : block->fragments())
		{
//			auto textformat = toQTextCharFormat(Proj.textFormat(jfrag[0]));
//			cursor.insertText(QString::fromStdString(jfrag[1]), textformat);
			auto textformat = Utils::toQTextCharFormat(fragment->getTextFormat());
			cursor.insertText(QString::fromStdString(fragment->getText()), textformat);
		}
	}
	return doc;
}

std::shared_ptr<NovelTea::ActiveText> RichTextEditor::documentToActiveText(const QTextDocument *doc)
{
	auto activeText = std::make_shared<NovelTea::ActiveText>();
	int fmtIndexLast = -1;

	for (auto qblock = doc->begin(); qblock != doc->end(); qblock = qblock.next())
	{
		if (qblock.isValid())
		{
			auto block = std::make_shared<NovelTea::TextBlock>();
			NovelTea::TextFragment fragment;
			auto sfrag = std::string();
//			block->setAlignment

			for (auto it = qblock.begin(); !it.atEnd(); ++it)
			{
				QTextFragment qfragment = it.fragment();
				if (qfragment.isValid())
				{
					auto format = Utils::toTextFormat(qfragment.charFormat());
					auto fmtIndex = NovelTea::ProjectData::instance().addTextFormat(format);

					if (fmtIndex != fmtIndexLast)
					{
						if (fmtIndexLast >= 0)
						{
							fragment.setText(sfrag);
							block->addFragment(std::make_shared<NovelTea::TextFragment>(fragment));
//							jfrag[1] = sfrag;
//							jblock[1].push_back(jfrag);
						}
						fmtIndexLast = fmtIndex;
//						jfrag[0] = fmtIndex;
						fragment.setTextFormat(format);
						sfrag.clear();
					}

					sfrag += qfragment.text().toStdString();
				}
//				if ((++it).atEnd())
//				{
//					fmtIndexLast = -1;
//					fragment.setText(sfrag);
//				}
			}

			fmtIndexLast = -1;

			fragment.setText(sfrag);
			block->addFragment(std::make_shared<NovelTea::TextFragment>(fragment));
			activeText->addBlock(block);
//			jfrag[1] = sfrag;
//			jblock[1].push_back(jfrag);
//			j.push_back(jblock);
		}
	}

	return activeText;
}

//void RichTextEditor::setValue(const json &j)
//{
//	auto doc = Utils::jsonToDocument(j);
//	ui->textEdit->setDocument(doc);
//}

void RichTextEditor::fontChanged(const QFont &font)
{
//	comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
//	comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
	ui->actionBold->setChecked(font.bold());
	ui->actionItalic->setChecked(font.italic());
	ui->actionUnderline->setChecked(font.underline());
}

void RichTextEditor::colorChanged(const QColor &color)
{
	QPixmap pix(16, 16);
	pix.fill(color);
//	actionTextColor->setIcon(pix);
}

void RichTextEditor::on_actionFinish_triggered()
{
//	auto doc = ui->textEdit->document();
//	json j = Utils::documentToJson(doc);

//	NovelTea::TextFormat f;
//	json j = f;
//	std::cout << j.dump() << std::endl;

	emit saved(documentToActiveText(ui->textEdit->document()));

//	NovelTea::ProjectData::instance().saveToFile("/home/android/test.ntp");
//	NovelTea::ProjectData::instance().loadFromFile("/home/android/test.ntp");
}

void RichTextEditor::on_actionBold_triggered()
{
	QTextCharFormat fmt;
	fmt.setFontWeight(ui->actionBold->isChecked() ? QFont::Bold : QFont::Normal);
	mergeFormat(fmt);
}

void RichTextEditor::on_actionItalic_triggered()
{

}

void RichTextEditor::on_actionUnderline_triggered()
{

}

void RichTextEditor::on_textEdit_currentCharFormatChanged(const QTextCharFormat &format)
{
	fontChanged(format.font());
	colorChanged(format.foreground().color());
}
