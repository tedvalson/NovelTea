#ifndef EDITORUTILS_HPP
#define EDITORUTILS_HPP

#include <QTextDocument>
#include <QTextCharFormat>
#include <SFML/Graphics/Color.hpp>
#include "Widgets/EditorTabWidget.hpp"
#include <NovelTea/ActiveText.hpp>
#include <NovelTea/json.hpp>
#include <iostream>

using json = sj::JSON;

class EditorUtils
{
public:
	static QString escape(const QString &s);
	static QString unescape(const QString &s);

	static std::string getFileContents(const QString &fileName);

	static QIcon iconFromTabType(EditorTabWidget::Type type);

	static QString documentToBBCode(const QTextDocument *doc);
	static QTextDocument *documentFromBBCode(const QString &bbstring);

};

#endif // EDITORUTILS_HPP
