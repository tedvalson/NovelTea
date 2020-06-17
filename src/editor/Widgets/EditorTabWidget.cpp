#include "EditorTabWidget.hpp"


EditorTabWidget::EditorTabWidget(QWidget *parent)
	: QWidget(parent)
{

}

EditorTabWidget::~EditorTabWidget()
{

}

bool EditorTabWidget::isModified() const
{
	return _modified;
}

const std::string &EditorTabWidget::idName() const
{
	return _idName;
}

void EditorTabWidget::rename(const std::string &newIdName)
{
	_idName = newIdName;
}

EditorTabWidget::Type EditorTabWidget::entityTypeToTabType(NovelTea::EntityType entityType)
{
	switch (entityType)
	{
		case NovelTea::EntityType::Cutscene:
			return Type::Cutscene;
		case NovelTea::EntityType::Room:
			return Type::Room;
		case NovelTea::EntityType::Object:
			return Type::Object;
		case NovelTea::EntityType::Verb:
			return Type::Verb;
		default:
			return Type::Invalid;
	}
}

NovelTea::EntityType EditorTabWidget::tabTypeToEntityType(EditorTabWidget::Type tabType)
{
	switch (tabType)
	{
		case Type::Cutscene:
			return NovelTea::EntityType::Cutscene;
		case Type::Room:
			return NovelTea::EntityType::Room;
		case Type::Object:
			return NovelTea::EntityType::Object;
		case Type::Verb:
			return NovelTea::EntityType::Verb;
		default:
			return NovelTea::EntityType::Invalid;
	}
}

void EditorTabWidget::save()
{
	if (_modified)
	{
		_modified = false;
		saveData();
		emit saved();
	}
}

void EditorTabWidget::load()
{
	_modified = false;
	loadData();
	emit loaded();
}

void EditorTabWidget::setModified()
{
	if (!_modified)
	{
		_modified = true;
		emit modified();
	}
}
