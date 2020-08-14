#include "DialogueTreeItem.hpp"
#include <NovelTea/Game.hpp>

DialogueTreeItem::DialogueTreeItem(const std::shared_ptr<NovelTea::DialogueSegment> &segment, DialogueTreeItem *parent)
{
	parentItem = parent;
	setDialogueSegment(segment);
}

DialogueTreeItem::~DialogueTreeItem()
{
	qDeleteAll(childItems);
}

void DialogueTreeItem::appendChild(DialogueTreeItem *item)
{
	childItems.append(item);
}

DialogueTreeItem *DialogueTreeItem::child(int row)
{
	return childItems.value(row);
}

int DialogueTreeItem::childCount() const
{
	return childItems.count();
}

int DialogueTreeItem::columnCount() const
{
	return 1;
}

QVariant DialogueTreeItem::data(int column) const
{
	if (column == 0)
	{
		if (m_segment->getType() == NovelTea::DialogueSegment::Root)
			return "ROOT";
		else if (m_segment->getScriptedText())
		{
			try {
				auto result = ActiveGame->getScriptManager().runInClosure<std::string>(m_segment->getText());
				return QString::fromStdString(result);
			} catch (std::exception &e) {
				auto result = QString::fromStdString(e.what());
				result.truncate(result.indexOf("\n"));
				return "ERROR: " + result;
			}
		}
		else
			return QString::fromStdString(m_segment->getText()).replace("\n"," ");
	}
	else
		return QString();
}

bool DialogueTreeItem::setData(int column, QVariant value)
{
	return false;
}

DialogueTreeItem *DialogueTreeItem::parent()
{
	return parentItem;
}

void DialogueTreeItem::changeParent(DialogueTreeItem *parent)
{
	parentItem->childItems.takeAt(row());
	parent->appendChild(this);
	parentItem = parent;
}

void DialogueTreeItem::setDialogueSegment(const std::shared_ptr<NovelTea::DialogueSegment> &segment)
{
	m_segment = segment;
}

const std::shared_ptr<NovelTea::DialogueSegment> &DialogueTreeItem::getDialogueSegment() const
{
	return m_segment;
}

bool DialogueTreeItem::insertChildren(int position, int count, int columns)
{
	return insertSegment(position, count, std::make_shared<NovelTea::DialogueSegment>());
}

bool DialogueTreeItem::removeChildren(int position, int count)
{
	if (position < 0 || position + count > childItems.size())
		return false;

	for (int row = 0; row < count; ++row)
		delete childItems.takeAt(position);

	return true;
}

bool DialogueTreeItem::insertSegment(int position, int count, const std::shared_ptr<NovelTea::DialogueSegment> &segment)
{
	if (position < 0 || position > childItems.size())
		return false;

	for (int row = 0; row < count; ++row) {
		auto item = new DialogueTreeItem(segment, this);
		childItems.insert(position, item);
	}

	return true;
}

int DialogueTreeItem::row() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<DialogueTreeItem*>(this));

	return 0;
}
