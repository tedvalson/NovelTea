#include "MapStyle.hpp"

MapStyle &MapStyle::instance()
{
	static MapStyle style;
	return style;
}

MapStyle::MapStyle()
: NodeNormalBoundaryColor("darkgray")
, NodeSelectedBoundaryColor("deepskyblue")
, NodeShadowColor(200, 200, 200)
, NodeFontColor(10, 10, 10)
, NodeFontColorFaded(100, 100, 100)
, NodeConnectionPointColor("white")
, NodeFilledConnectionPointColor("cyan")
, NodeDoorNormalColor("black")
, NodeDoorErrorColor("red")
, NodeGradientColor0("mintcream")
, NodeGradientColor1("mintcream")
, NodeGradientColor2("mintcream")
, NodeGradientColor3("mintcream")
, NodePenWidth(2.f)
, NodeHoveredPenWidth(2.5f)
, NodeConnectionPointDiameter(10.f)
, NodeOpacity(1.f)
, ConnectionConstructionColor("gray")
, ConnectionNormalColor("black")
, ConnectionSelectedColor("gray")
, ConnectionSelectedHaloColor("deepskyblue")
, ConnectionHoveredColor("deepskyblue")
, ConnectionLineWidth(3.f)
, ConnectionConstructionLineWidth(2.f)
, ConnectionPointDiameter(15.f)
, SceneBackgroundColor(255, 255, 240)
, SceneCoarseGridColor(235, 235, 220)
, SceneFineGridColor(245, 245, 230)
{
}
