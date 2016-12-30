#if !defined( PALETTIZEDBITMAP_H_INCLUDED )
#define PALETTIZEDBITMAP_H_INCLUDED

#pragma once

/*****************************************************************************

                              PalettizedBitmap.h

						Copyright 2001, John J. Bolton
	----------------------------------------------------------------------

	$Header: //depot/Libraries/Bitmap/PalettizedBitmap.h#2 $

	$NoKeywords: $

*****************************************************************************/

#include "Bitmap.h"
#include "Palette.h"

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template< class _Color >
class PalettizedBitmap : public Bitmap< Pixel8 >
{
public:

	typedef Bitmap< Pixel8 >	BaseClass;
	typedef _Color				Color;

		// Constructors and destructors

	PalettizedBitmap( int w = 0, int h = 0 );											// Normal
	PalettizedBitmap( int w, int h, int p, Pixel * pMap, Palette< Color > const & pal );	// Reference bitmap
	PalettizedBitmap( PalettizedBitmap const & rval );									// Copy constructor
	virtual ~PalettizedBitmap();														// Destructor

		// Assignment operator
	PalettizedBitmap& operator=( PalettizedBitmap const & rval ); 

		// Load this bitmap from a memory buffer
	void Import( int w, int h, int p, Pixel const * pMap, Palette< Color > const & pal );

		// Reference a memory buffer through this bitmap
	void Reference( int w, int h, int p, Pixel * pMap, Palette< Color > const & pal );

		// Returns the palette for this bitmap
	Palette< Color > const & GetPalette() const { return m_Palette; }
	Palette< Color >       & GetPalette()       { return m_Palette; }

		// Draw a section of this bitmap onto another palettized bitmap
	void Draw( Rect const & srcRect, PalettizedBitmap * pDst, int dx, int dy ) const;

		// Draw a section of this bitmap onto a non-palettized bitmap
	void Draw( Rect const & srcRect, Bitmap< Color > * pDst, int dx, int dy ) const;


private:

	Palette< Color >	m_Palette;	// The palette
};



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

typedef PalettizedBitmap< Pixel16 >	Bitmap816;
typedef PalettizedBitmap< Pixel24 >	Bitmap824;


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template< class _Color >
PalettizedBitmap< _Color >::PalettizedBitmap( int w/* = 0*/, int h/* = 0*/ )
	: BaseClass( w, h )
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template< class _Color >
PalettizedBitmap< _Color >::PalettizedBitmap( int w, int h, int p, Pixel * pMap, Palette< Color > const & pal )
	: BaseClass( w, h, p, pMap ),
	  m_Palette( pal )
{
}



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template< class _Color >
PalettizedBitmap< _Color >::PalettizedBitmap( PalettizedBitmap< _Color > const & rval )
	: BaseClass( rval ),
	  m_Palette( rval.m_Palette )
{
}



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template< class _Color >
PalettizedBitmap< _Color >& PalettizedBitmap< _Color >::operator=( PalettizedBitmap< _Color > const & rval )
{
	if ( this == &rval )
		return *this;

	BaseClass::operator=( rval );

	m_Palette = rval.m_Palette;

	return *this;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template< class _Color >
void PalettizedBitmap< _Color >::Import( int w, int h, int p, Pixel const * pMap, Palette< _Color > const & pal )
{
	BaseClass::Import( w, h, p, pMap );
	m_Palette = pal;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template< class _Color >
void PalettizedBitmap< _Color >::Reference( int w, int h, int p, Pixel* pMap, Palette< _Color > const & pal )
{
	BaseClass::Reference( w, h, p, pMap );
	m_Palette = pal;
}



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template< class _Color >
inline void PalettizedBitmap< _Color >::Draw( Rect const & srcRect, PalettizedBitmap * pDst, int dx, int dy ) const
{
	BaseClass::Draw( srcRect, pDst, dx, dy );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template< class _Color >
void PalettizedBitmap< _Color >::Draw( Rect const & srcRect, Bitmap< Color > * pDst, int dx, int dy ) const
{
	Rect	rect	= srcRect;

	ClipForDrawing( &rect, &dx, &dy, dst->Getm_Width(), dst->GetHeight() );

	if ( rect.m_Width <= 0 || rect.m_Height <= 0 )
		return;

	Color *			dstp	= pDst->GetMap( dx, dy );
	Pixel const *	srcp	= GetMap( rect.m_X, rect.m_Y );
	const int		dpitch	= pDst->GetPitch();

	for ( int i = 0; i < rect.m_Height; i++ )
	{
		_Color *      d = dstp;
		Pixel const * s = srcp;

		for ( int j = 0; j < rect.m_Width; j++ )
		{
			*d = m_Palette[ *s ];
			++d;
			++s;
		}

		dstp = reinterpret_cast< Color *       >( reinterpret_cast< char *       >( dstp ) + dpitch );
		srcp = reinterpret_cast< Pixel const * >( reinterpret_cast< char const * >( srcp ) + m_Pitch );
	}
}



#endif // !defined( PALETTIZEDBITMAP_H_INCLUDED )
