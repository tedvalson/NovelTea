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
