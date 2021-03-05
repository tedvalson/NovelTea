#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>

class TreeItem
{
public:
	explicit TreeItem(const QString &title, TreeItem *parent = 0);
	explicit TreeItem(const QList<QVariant> &data, TreeItem *parent = 0);
	~TreeItem();

	void appendChild(TreeItem *child);

	TreeItem *child(int row) const;
	int childCount() const;
	int columnCount() const;
	QVariant data(int column) const;
	bool setData(int column, QVariant value);
	int row() const;
	TreeItem *parent();
	void changeParent(TreeItem *parent);

	bool insertChildren(int position, int count, int columns);
	bool removeChildren(int position, int count);

private:
	QList<TreeItem*> childItems;
	QList<QVariant> itemData;
	TreeItem *parentItem;
};

#endif // TREEITEM_H
