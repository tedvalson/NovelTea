#include <math.h>
#include <TweenEngine/paths/PopOutPath.h>

namespace TweenEngine
{
	float PopOutPath::compute(float t, float *points, int pointsCnt)
    {
		if (pointsCnt != 3)
			return points[pointsCnt-1];
		auto popTime = 1.f / 6.f;
		if (t < popTime)
			return points[0] + t * 6.f * points[1];
		else if (t > 1.f - popTime)
			return points[1] + (t - 1.f + popTime) * 6.f * (points[2] - points[1]);
		else
			return points[1];
    }
}
