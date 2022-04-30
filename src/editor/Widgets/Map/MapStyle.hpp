#pragma once

#include <QColor>

class MapStyle
{
public:
	static MapStyle& instance();
private:
	MapStyle();
	MapStyle(MapStyle const&) = delete;
	MapStyle& operator=(MapStyle const&) = delete;
public:

	// Node
	QColor NodeNormalBoundaryColor;
	QColor NodeSelectedBoundaryColor;
	QColor NodeShadowColor;
	QColor NodeFontColor;
	QColor NodeFontColorFaded;
	QColor NodeConnectionPointColor;
	QColor NodeFilledConnectionPointColor;
	QColor NodeDoorNormalColor;
	QColor NodeDoorErrorColor;
	QColor NodeGradientColor0;
	QColor NodeGradientColor1;
	QColor NodeGradientColor2;
	QColor NodeGradientColor3;
	float NodePenWidth;
	float NodeHoveredPenWidth;
	float NodeConnectionPointDiameter;
	float NodeOpacity;

	// Connection
	QColor ConnectionConstructionColor;
	QColor ConnectionNormalColor;
	QColor ConnectionSelectedColor;
	QColor ConnectionSelectedHaloColor;
	QColor ConnectionHoveredColor;
	float ConnectionLineWidth;
	float ConnectionConstructionLineWidth;
	float ConnectionPointDiameter;

	// Scene
	QColor SceneBackgroundColor;
	QColor SceneCoarseGridColor;
	QColor SceneFineGridColor;
};
