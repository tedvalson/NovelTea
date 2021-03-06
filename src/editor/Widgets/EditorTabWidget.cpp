#include "EditorTabWidget.hpp"
#include "EditorUtils.hpp"
#include <NovelTea/Game.hpp>
#include <QIcon>


EditorTabWidget::EditorTabWidget(QWidget *parent)
: QWidget(parent)
{

}

EditorTabWidget::~EditorTabWidget()
{

}

bool EditorTabWidget::isModified() const
{
	return m_modified;
}

QIcon EditorTabWidget::getIcon() const
{
	return EditorUtils::iconFromTabType(getType());
}

const std::string &EditorTabWidget::idName() const
{
	return m_idName;
}

void EditorTabWidget::rename(const std::string &newIdName)
{
	m_idName = newIdName;
}

EditorTabWidget::Type EditorTabWidget::entityTypeToTabType(NovelTea::EntityType entityType)
{
	switch (entityType)
	{
		case NovelTea::EntityType::Action:
			return Type::Action;
		case NovelTea::EntityType::Cutscene:
			return Type::Cutscene;
		case NovelTea::EntityType::Dialogue:
			return Type::Dialogue;
		case NovelTea::EntityType::Room:
			return Type::Room;
		case NovelTea::EntityType::Object:
			return Type::Object;
		case NovelTea::EntityType::Script:
			return Type::Script;
		case NovelTea::EntityType::Verb:
			return Type::Verb;
		default:
			return Type::Invalid;
	}
}

EditorTabWidget::Type EditorTabWidget::entityTypeToTabType(int entityType)
{
	return entityTypeToTabType(static_cast<NovelTea::EntityType>(entityType));
}

NovelTea::EntityType EditorTabWidget::tabTypeToEntityType(EditorTabWidget::Type tabType)
{
	switch (tabType)
	{
		case Type::Action:
			return NovelTea::EntityType::Action;
		case Type::Cutscene:
			return NovelTea::EntityType::Cutscene;
		case Type::Dialogue:
			return NovelTea::EntityType::Dialogue;
		case Type::Room:
			return NovelTea::EntityType::Room;
		case Type::Object:
			return NovelTea::EntityType::Object;
		case Type::Script:
			return NovelTea::EntityType::Script;
		case Type::Verb:
			return NovelTea::EntityType::Verb;
		default:
			return NovelTea::EntityType::Invalid;
	}
}

NovelTea::EntityType EditorTabWidget::tabTypeToEntityType(int tabType)
{
	return tabTypeToEntityType(static_cast<Type>(tabType));
}

void EditorTabWidget::save()
{
	if (m_modified)
	{
		m_modified = false;
		saveData();
		emit saved();
	}
}

void EditorTabWidget::load()
{
	m_modified = false;
	loadData();
	emit loaded();
}

void EditorTabWidget::setModified()
{
	if (!m_modified)
	{
		m_modified = true;
		emit modified();
	}
}

void EditorTabWidget::showEvent(QShowEvent *)
{
	ActiveGame->reset();
}

void EditorTabWidget::hideEvent(QHideEvent *)
{
}
