#ifndef DIALOGUETREEITEM_HPP
#define DIALOGUETREEITEM_HPP

#include <NovelTea/DialogueSegment.hpp>
#include <QList>
#include <QVariant>

class DialogueTreeItem
{
public:
	explicit DialogueTreeItem(const std::string &title, DialogueTreeItem *parent = 0);
	explicit DialogueTreeItem(const std::shared_ptr<NovelTea::DialogueSegment> &segment, DialogueTreeItem *parent = 0);
	~DialogueTreeItem();

	void appendChild(DialogueTreeItem *child);

	DialogueTreeItem *child(int row);
	int childCount() const;
	int columnCount() const;
	QVariant data(int column) const;
	bool setData(int column, QVariant value);
	int row() const;
	DialogueTreeItem *parent();
	void changeParent(DialogueTreeItem *parent);

	void saveDialogueSegment(std::shared_ptr<NovelTea::DialogueSegment> segment) const;
	void loadDialogueSegment(const std::shared_ptr<NovelTea::DialogueSegment> segment);

	bool insertChildren(int position, int count, int columns);
	bool removeChildren(int position, int count);

private:
	QList<DialogueTreeItem*> childItems;
	QList<QVariant> itemData;
	DialogueTreeItem *parentItem;
};

#endif // DIALOGUETREEITEM_HPP
