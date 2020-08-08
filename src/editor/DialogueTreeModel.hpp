#ifndef DIALOGUETREEMODEL_HPP
#define DIALOGUETREEMODEL_HPP

#include "Widgets/EditorTabWidget.hpp"
#include <NovelTea/Dialogue.hpp>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>


class DialogueTreeItem;
class QContextMenuEvent;

class DialogueTreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	explicit DialogueTreeModel(QObject *parent = 0);
	~DialogueTreeModel();

	QVariant data(const QModelIndex &index, int role) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
//	QVariant headerData(int section, Qt::Orientation orientation,
//						int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	bool insertRows(int position, int rows,
					const QModelIndex &parent = QModelIndex()) override;
	bool removeRows(int position, int rows,
					const QModelIndex &parent = QModelIndex()) override;

	void saveDialogue(const std::shared_ptr<NovelTea::Dialogue> &dialogue) const;
	void loadDialogue(const std::shared_ptr<NovelTea::Dialogue> &dialogue);
	bool changeParent(const QModelIndex &child, const QModelIndex &newParent);

private:
	DialogueTreeItem *getItem(const QModelIndex &index) const;

	DialogueTreeItem *m_rootItem;
};

#endif // DIALOGUETREEMODEL_HPP