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
	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	bool insertRows(int position, int rows,
					const QModelIndex &parent = QModelIndex()) override;
	bool removeRows(int position, int rows,
					const QModelIndex &parent = QModelIndex()) override;
	bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
					const QModelIndex &destinationParent, int destinationChild) override;

	void saveDialogue(const std::shared_ptr<NovelTea::Dialogue> &dialogue) const;
	void loadDialogue(const std::shared_ptr<NovelTea::Dialogue> &dialogue);
	bool changeParent(const QModelIndex &child, const QModelIndex &newParent);

	bool copy(const QModelIndex &source, const QModelIndex &destParent);
	bool insertSegmentLink(const QModelIndex &source, const QModelIndex &destParent);
	bool insertSegment(int row, const QModelIndex &parent, const std::shared_ptr<NovelTea::DialogueSegment> &segment);
	bool updateSegment(const QModelIndex &index, const std::shared_ptr<NovelTea::DialogueSegment> &segment);

private:
	DialogueTreeItem *getItem(const QModelIndex &index) const;

	DialogueTreeItem *m_rootItem;
	std::string m_dialogueId;
};

#endif // DIALOGUETREEMODEL_HPP
