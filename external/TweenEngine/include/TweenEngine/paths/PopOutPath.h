#ifndef __PopUpPath__
#define __PopUpPath__

#include <TweenEngine/TweenPath.h>

namespace TweenEngine
{
	class PopOutPath : public TweenPath
    {
        float compute(float t, float *points, int pointsCnt);
    };
}

#endif /* defined(__PopUpPath__) */
