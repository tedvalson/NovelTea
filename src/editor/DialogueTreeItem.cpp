#include "DialogueTreeItem.hpp"
#include <NovelTea/Game.hpp>

DialogueTreeItem::DialogueTreeItem(NovelTea::Context *context, const std::string &dialogueId, const std::shared_ptr<NovelTea::DialogueSegment> &segment, DialogueTreeItem *parent)
: NovelTea::ContextObject(context)
, m_parentItem(parent)
, m_linkItem(nullptr)
, m_dialogueId(dialogueId)
{
	if (segment)
		setDialogueSegment(segment);
	else
		setDialogueSegment(std::make_shared<NovelTea::DialogueSegment>(context));
}

DialogueTreeItem::~DialogueTreeItem()
{
	qDeleteAll(m_childItems);
}

void DialogueTreeItem::appendChild(DialogueTreeItem *item)
{
	m_childItems.append(item);
}

DialogueTreeItem *DialogueTreeItem::child(int row)
{
	return m_childItems.value(row);
}

int DialogueTreeItem::childCount() const
{
	return m_childItems.count();
}

int DialogueTreeItem::columnCount() const
{
	return 1;
}

QVariant DialogueTreeItem::data(int column) const
{
	auto segment = m_segment;
	if (column == 0)
	{
		if (m_linkItem)
			segment = m_linkItem->getDialogueSegment();

		if (segment->getType() == NovelTea::DialogueSegment::Root)
			return "ROOT";
		else if (segment->getTextRaw().empty())
			return "[SKIP]";
		else if (segment->isComment())
			return QString::fromStdString(segment->getText());
		else if (segment->getType() == NovelTea::DialogueSegment::Option)
		{
			QString str;
			auto lines = segment->getOptionMultiline();
			for (auto &line : lines) {
				if (!str.isEmpty())
					str += "\n";
				str += QString::fromStdString(line).replace("\n"," ");
			}
			return str.replace("\t", " ");
		}
		else
		{
			QString str;
			auto lines = segment->getTextMultiline();
			for (auto &line : lines) {
				if (!str.isEmpty())
					str += "\n";
				if (!line.first.empty())
					str += QString::fromStdString("["+line.first+"] ");
				str += QString::fromStdString(line.second).replace("\n"," ");
			}
			return str.replace("\t", " ");
		}
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
	return m_parentItem;
}

void DialogueTreeItem::moveItem(DialogueTreeItem *newParent, int newPosition)
{
	if (m_parentItem == newParent)
	{
		if (row() < newPosition)
			--newPosition;
		m_parentItem->m_childItems.move(row(), newPosition);
	} else {
		m_parentItem->m_childItems.takeAt(row());
		if (newPosition < 0 || newPosition > newParent->childCount())
			newParent->appendChild(this);
		else
			newParent->insertItem(newPosition, this);
		m_parentItem = newParent;
	}
}

void DialogueTreeItem::setLink(DialogueTreeItem *link)
{
	m_linkItem = link;
}

DialogueTreeItem *DialogueTreeItem::getLink()
{
	return m_linkItem;
}

void DialogueTreeItem::setDialogueSegment(const std::shared_ptr<NovelTea::DialogueSegment> &segment)
{
	m_segment = segment;
}

const std::shared_ptr<NovelTea::DialogueSegment> &DialogueTreeItem::getDialogueSegment() const
{
	return m_segment;
}

DialogueTreeItem *DialogueTreeItem::makeCopy(DialogueTreeItem *parentItem)
{
	auto segment = std::make_shared<NovelTea::DialogueSegment>(getContext());
	*segment = *m_segment;
	auto result = new DialogueTreeItem(getContext(), m_dialogueId, segment, parentItem);
	result->m_linkItem = m_linkItem;
	for (auto &child : m_childItems)
	{
		auto childCopy = child->makeCopy(result);
		result->appendChild(childCopy);
	}
	return result;
}

bool DialogueTreeItem::insertChildren(int position, int count, int columns)
{
	return insertSegment(position, count, std::make_shared<NovelTea::DialogueSegment>(getContext()));
}

bool DialogueTreeItem::removeChildren(int position, int count)
{
	if (position < 0 || position + count > m_childItems.size())
		return false;

	for (int row = 0; row < count; ++row)
		delete m_childItems.takeAt(position);

	return true;
}

bool DialogueTreeItem::insertSegment(int position, int count, const std::shared_ptr<NovelTea::DialogueSegment> &segment)
{
	if (position < 0 || position > m_childItems.size())
		return false;

	for (int row = 0; row < count; ++row) {
		auto item = new DialogueTreeItem(getContext(), m_dialogueId, segment, this);
		m_childItems.insert(position, item);
	}

	return true;
}

bool DialogueTreeItem::insertLink(int position, DialogueTreeItem *sourceItem)
{
	auto segment = std::make_shared<NovelTea::DialogueSegment>(getContext());
	auto item = new DialogueTreeItem(getContext(), m_dialogueId, segment, this);
	segment->setType(NovelTea::DialogueSegment::Link);
	item->m_linkItem = sourceItem;

	m_childItems.insert(position, item);
	return true;
}

bool DialogueTreeItem::insertItem(int position, DialogueTreeItem *item)
{
	if (position > m_childItems.size())
		return false;
	m_childItems.insert(position, item);
	return true;
}

int DialogueTreeItem::row() const
{
	if (m_parentItem)
		return m_parentItem->m_childItems.indexOf(const_cast<DialogueTreeItem*>(this));

	return 0;
}
