#ifndef EDITORUTILS_HPP
#define EDITORUTILS_HPP

#include <QTextDocument>
#include <QTextCharFormat>
#include <SFML/Graphics/Color.hpp>
#include <NovelTea/TextFormat.hpp>
#include <NovelTea/ActiveText.hpp>
#include <json.hpp>
#include <iostream>

using json = nlohmann::json;

class EditorUtils
{
public:
	EditorUtils();

	static json documentToJson(const QTextDocument *doc);
	static QTextDocument *jsonToDocument(const json &j);
	static QString activeTextToString(const NovelTea::ActiveText &activeText);

	static NovelTea::TextFormat toTextFormat(const QTextCharFormat &format);
	static QTextCharFormat toQTextCharFormat(const NovelTea::TextFormat &format);
};

#endif // EDITORUTILS_HPP
