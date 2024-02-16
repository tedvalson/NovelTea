#ifndef SHADERWIDGET_HPP
#define SHADERWIDGET_HPP

#include "EditorTabWidget.hpp"
#include <QWidget>
#include <NovelTea/json.hpp>
#include <SFML/Graphics/Shader.hpp>

namespace Ui {
class ShaderWidget;
}

class QtVariantEditorFactory;
class QtVariantPropertyManager;
class QtProperty;
class QComboBox;
class QListWidgetItem;

class ShaderWidget : public EditorTabWidget
{
	Q_OBJECT
public:
	explicit ShaderWidget(QWidget *parent = 0);
	virtual ~ShaderWidget();

	QString tabText() const override;
	Type getType() const override;

	void loadShaderId(const std::string &shaderId);

protected:
	void timerEvent(QTimerEvent *event);

private slots:
	void on_actionAddShader_triggered();
	void on_actionDeleteShader_triggered();
	void on_listWidget_currentRowChanged(int currentRow);
	void on_tabWidget_currentChanged(int index);
	void on_actionPreview_toggled(bool checked);
	void on_actionUniforms_toggled(bool checked);
	void on_actionCompileOutput_toggled(bool checked);
	void on_scriptEdit_textChanged();
	void on_actionAddTexture_triggered();
	void on_actionRemoveTexture_triggered();
	void on_listTextures_itemChanged(QListWidgetItem *item);
	void on_listTextures_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
	std::string makeTextureName(const std::string &textureName);
	bool addTextureToList(const std::string &textureData, const std::string &textureName);
	bool isVertexShader(const std::string &script);
	bool updateErrorLog();
	void updatePropertyList(const sj::JSON *uniformArray = nullptr);
	void updatePreview();
	void saveCurrentShaderId() const;
	sj::JSON getUniforms() const;
	void propertyChanged(QtProperty *property, const QVariant &value);
	void saveData() const override;
	void loadData() override;

	Ui::ShaderWidget *ui;

	QtVariantPropertyManager *m_variantManager;
	QtVariantEditorFactory *m_variantFactory;
	std::map<QComboBox*, int> m_systemShaderMap;
	std::map<std::string, std::string> m_texturesData;
	QString m_lastSelectedTextureName;

	sf::Shader m_shader;
	mutable sj::JSON m_shaders;
	std::string m_currentShaderId;
	bool m_shaderChanged;
};

#endif // SHADERWIDGET_HPP
