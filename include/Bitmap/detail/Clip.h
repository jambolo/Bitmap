#if !defined(BITMAP_DETAIL_CLIP_H)
#define BITMAP_DETAIL_CLIP_H

#pragma once

#include "Rect/Rect.h"

namespace detail
{
// Adjusts the rects used to copy from an arbitrary region in a source bitmap to a location in a destination bitmap.
//
// Parameters:
//  - a source rectangle (*srcRect) inside a source bitmap of size (srcW, srcH)
//  - a destination placement (*dstX, *dstY) inside a destination bitmap of size (dstW, dstH)
//
// It mutates *srcRect, *dstX, and *dstY so a later copy only operates on valid pixels in the source bitmap.
inline void clip(Rect * srcRect, int srcW, int srcH, int * dstX, int * dstY, int dstW, int dstH)
{
	Rect rect = *srcRect;
	int  dx   = *dstX;
	int  dy   = *dstY;

	// Clip the source rect to the source bitmap dimensions (0, 0, srcW, srcH)

	if (rect.x < 0)
	{
		rect.width += rect.x;
		dx         -= rect.x; // The origin of the source rect moved so the destination must move too
		rect.x      = 0;
	}

	if (rect.y < 0)
	{
		rect.height += rect.y;
		dy          -= rect.y; // The origin of the source rect moved so the destination must move too
		rect.y       = 0;
	}

	if (rect.x + rect.width > srcW)
		rect.width = srcW - rect.x;

	if (rect.y + rect.height > srcH)
		rect.height = srcH - rect.y;

	// Clip the translated source rect to the destination bitmap dimensions (0, 0, dstW, dstH). Ensure that the destination
	// coordinates are non-negative, and adjust the source rect accordingly.

	if (dx < 0)
	{
		rect.width += dx;
		rect.x     -= dx;
		dx          = 0;
	}

	if (dy < 0)
	{
		rect.height += dy;
		rect.y      -= dy;
		dy           = 0;
	}

	if (dx + rect.width > dstW)
		rect.width = dstW - dx;

	if (dy + rect.height > dstH)
		rect.height = dstH - dy;

	*srcRect = rect;
	*dstX    = dx;
	*dstY    = dy;
}
} // namespace detail

#endif // !defined(BITMAP_DETAIL_CLIP_H)
