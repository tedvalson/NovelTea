#ifndef DIALOGUETREEITEM_HPP
#define DIALOGUETREEITEM_HPP

#include <NovelTea/DialogueSegment.hpp>
#include <QVariant>

class DialogueTreeItem
{
public:
	explicit DialogueTreeItem(const std::shared_ptr<NovelTea::DialogueSegment> &segment = std::make_shared<NovelTea::DialogueSegment>(), DialogueTreeItem *parent = 0);
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

	void setDialogueSegment(const std::shared_ptr<NovelTea::DialogueSegment> &segment);
	const std::shared_ptr<NovelTea::DialogueSegment> &getDialogueSegment() const;

	bool insertChildren(int position, int count, int columns);
	bool removeChildren(int position, int count);
	bool insertSegment(int position, int count, const std::shared_ptr<NovelTea::DialogueSegment> &segment);

private:
	QList<DialogueTreeItem*> childItems;
	std::shared_ptr<NovelTea::DialogueSegment> m_segment;
	DialogueTreeItem *parentItem;
};

#endif // DIALOGUETREEITEM_HPP
