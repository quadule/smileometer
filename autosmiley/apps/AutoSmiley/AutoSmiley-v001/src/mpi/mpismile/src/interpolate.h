/*
 *  interpolate
 *
 *  Original:Ian Fasel, Bret Fortenberry
 *  Author:Kazuki Aisaka
 *  Fixes:
 *
 *  Copyright (c) 2005 Machine Perception Laboratory
 *  University of California San Diego.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *    3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __INTERPOLATE_H__
#define __INTERPOLATE_H__

//
// Define for Include
// To use this, use "bjam debug"
//#define II_USE_IMAGEMAGICK	(1)

//
// Include
//
#include <iostream>
#include <math.h>

#include <mpisearch.h>

#ifdef II_USE_IMAGEMAGICK
#include <Magick++.h>
using namespace std;
using namespace Magick;

#include <time.h>
#endif

//
// Define
//
// Err Code (CFourCoordinates)
#define FC_NO_ERR		(0)
#define FC_ERR			(1)
#define FC_NOT_SUPPORT		(2)
// Err Code (CIntegralInterpolate)
#define II_NO_ERR		(0)
#define II_ERR			(1)
#define II_NOT_SUPPORT		(2)
// create facebox by eye position
#define II_EYE_LEN_RATE		(0.50)
#define II_EYE_TOP_RATE		(0.20)
#define II_EYE_LEFT_RATE	(0.25)
#define II_EYE_RIGHT_RATE	(0.25)
// Debug
#define FC_DO_PRINT		(0)
#define II_DO_PRINT		(0)
#define II_DO_DUMP		(0)
#define II_DO_DISPLAY		(0)
#define II_DO_LOG               (1)

// Macro
#define II_MAX(x,y)            ( (x > y) ? (x) : (y) )
#define II_ABS(x)              ( (x > 0) ? (x) : -(x) )

// Template
template < class T > class CIntegralInterpolate;

//
// Class, Four Coordinates
//
template < class T >
class CFourCoordinates {
 public:
  CFourCoordinates();
  CFourCoordinates( T tix1, T tiy1, 
		    T tix2, T tiy2,
		    T tix3, T tiy3,
		    T tix4, T tiy4 );
  virtual ~CFourCoordinates();

  //
  // operateor =
  //
  //CFourCoordinates operator=(const CFourCoordinates& v);

  //
  // Set 4 Coodinates
  //
  // tix*: x coordinates
  // tiy*: y coordinates
  // Return: error code (FC_***)
  //
  inline void vSetFourCoordinates( T tix1, T tiy1, 
				   T tix2, T tiy2, 
				   T tix3, T tiy3, 
				   T tix4, T tiy4 );

  //
  // Get 4 Coodinates
  //
  // ptox*: x coordinates
  // ptoy*: y coordinates
  // Return: error code (FC_***)
  //
  inline void vGetFourCoordinates( T* ptox1, T* ptoy1,
				   T* ptox2, T* ptoy2,
				   T* ptox3, T* ptoy3,
				   T* ptox4, T* ptoy4 );
  
  //
  // Check 4 Coodinates are appropriaet
  //
  // pixels: original image
  // Return: error code (FC_***)
  //
  inline int iCheckFourCoordinates( const RImage<T> &pixels );

#if FC_DO_PRINT
  //  // If you use iGetFaceFromEye, you don't need to set 4 Points.
  // Because 4 Coordinates are stored in class as members.
  // Print 4 coordinates
  //
  void vPrintFourCoordinates( void );
#endif

 public:
  //
  // Four Coordinates
  // This Four coordinates are used to interpolate.
  //
  //   Left-Up             Right-Up
  //   (x1,y1)-----------(x2,y2)
  //      |                  |
  //     |                  |
  //    |                  |
  // (x4,y4)---------(x3,y3)
  // Left-Bottom       Right-Bottom
  //
  T m_tx1;
  T m_ty1;
  T m_tx2;
  T m_ty2;
  T m_tx3;
  T m_ty3;
  T m_tx4;
  T m_ty4;
};
//
// Constructor
//
template < class T >
CFourCoordinates<T>::CFourCoordinates() :
  m_tx1( 0 ),
  m_ty1( 0 ),
  m_tx2( 0 ),
  m_ty2( 0 ),
  m_tx3( 0 ),
  m_ty3( 0 ),
  m_tx4( 0 ),
  m_ty4( 0 )
{
}

template < class T >
CFourCoordinates<T>::CFourCoordinates( T tix1, T tiy1, 
				       T tix2, T tiy2, 
				       T tix3, T tiy3, 
				       T tix4, T tiy4 ) : 
  m_tx1( tix1 ),
  m_ty1( tiy1 ),
  m_tx2( tix2 ),
  m_ty2( tiy2 ),
  m_tx3( tix3 ),
  m_ty3( tiy3 ),
  m_tx4( tix4 ),
  m_ty4( tiy4 )
{

}  // If you use iGetFaceFromEye, you don't need to set 4 Points.
  // Because 4 Coordinates are stored in class as members.

//
// Destructor
//
template < class T >
CFourCoordinates<T>::~CFourCoordinates()
{
}

//
// Set 4 Coodinates
//
// tix*: x coordinates
// tiy*: y coordinates
// Return: error code (FC_***)
//
template < class T >
inline void CFourCoordinates<T>::vSetFourCoordinates( T tix1, T tiy1, 
						      T tix2, T tiy2, 
						      T tix3, T tiy3, 
						      T tix4, T tiy4 )
{
  m_tx1 = tix1;
  m_ty1 = tiy1;
  m_tx2 = tix2;
  m_ty2 = tiy2;
  m_tx3 = tix3;
  m_ty3 = tiy3;
  m_tx4 = tix4;
  m_ty4 = tiy4;
}

//
// operator =
//
/*template < class T >
CFourCoordinates CFourCoordinates::operator=(const CFourCoordinates& v){
  CFourCoordinates tmp = *this;
  this.m_tx1 = v.m_tx1;
  this.m_ty1 = v.m_ty1;
  this.m_tx2 = v.m_tx2;
  this.m_ty2 = v.m_ty2;
  this.m_tx3 = v.m_tx3;
  this.m_ty3 = v.m_ty3;
  this.m_tx4 = v.m_tx4;
  this.m_ty4 = v.m_ty4;
  }*/

//
// Get 4 Coodinates
//
// ptix*: x coordinates
// ptiy*: y coordinates
// Return: error code (FC_***)
//
template < class T >
inline void CFourCoordinates<T>::vGetFourCoordinates( T* ptox1, T* ptoy1,
						      T* ptox2, T* ptoy2,
						      T* ptox3, T* ptoy3,
						      T* ptox4, T* ptoy4 )
{
  *ptox1 = m_tx1;
  *ptoy1 = m_ty1;
  *ptox2 = m_tx2;
  *ptoy2 = m_ty2;
  *ptox3 = m_tx3;
  *ptoy3 = m_ty3;
  *ptox4 = m_tx4;
  *ptoy4 = m_ty4;
}

//
// Check 4 Coodinate is appropriate
//
// pixels: original image
// Return: error code (FC_***)
//
template < class T >
inline int CFourCoordinates<T>::iCheckFourCoordinates( const RImage<T> &pixels )
{
  // check size
  if( ( m_tx1 < 0 ) || ( m_tx2 < 0 ) || ( m_tx4 < 0 ) || ( m_tx3 < 0 ) || 
      ( m_ty1 < 0 ) || ( m_ty2 < 0 ) || ( m_ty4 < 0 ) || ( m_ty3 < 0 ) ){
#if FC_DO_PRINT
    printf(" under 0\n");
#endif
    return FC_ERR;
  }

  if( ( m_tx1 >= (T)pixels.width )  || ( m_tx2 >= (T)pixels.width ) ||
      ( m_tx4 >= (T)pixels.width )  || ( m_tx3 >= (T)pixels.width ) ||
      ( m_ty1 >= (T)pixels.height ) || ( m_ty2 >= (T)pixels.height ) ||
      ( m_ty4 >= (T)pixels.height ) || ( m_ty3 >= (T)pixels.height ) ){
#if FC_DO_PRINT
    printf(" over image size\n");
#endif  // If you use iGetFaceFromEye, you don't need to set 4 Points.
  // Because 4 Coordinates are stored in class as members.
    return FC_ERR;
  }

  return FC_NO_ERR;
}

#if FC_DO_PRINT
//
// Print 4 coordinates
//
template < class T >
void CFourCoordinates<T>::vPrintFourCoordinates( void )
{
  cout << " facebox " << endl
       << "  1[" << m_tx1 << " " << m_ty1 << "]" << endl
       << "  2[" << m_tx2 << " " << m_ty2 << "]" << endl
       << "  3[" << m_tx3 << " " << m_ty3 << "]" << endl
       << "  4[" << m_tx4 << " " << m_ty4 << "]" << endl;
}
#endif


//
// Class, Interpolate using Integral Image
//
template < class T >
class CIntegralInterpolate {
 public:
  CIntegralInterpolate();
  virtual ~CIntegralInterpolate();

  //
  // Interpolate from Integral ImageApps/unix/eyefinder/
  //
  // srcpixels:  original image
  // dstpixeld:  interpolated image
  // tpIntegral: original integral pointer
  // iWidth:     interpolate width
  // iHeight:    interpolate height
  // ptC4Points: interpolate 4 coordinates
  // Return:     error code (II_***)
  //
  int iIntegralInterpolate( const RImage<T> &srcpixels,
			    const RImage<T> &dstpixels,
			    RIntegral<T>* tpIntegral,
			    int iWidth, int iHeight,
			    CFourCoordinates< T >* ptC4Points );

  //
  // Interpolate from Integral Image
  // If you use iGetFaceFromEye, you don't need to set 4 Points.
  // Because 4 Coordinates are stored in class as members.
  //
  // srcpixels:  original image
  // dstpixeld:  interpolated image
  // tpIntegral: original integral pointer
  // iWidth:     interpolate width
  // iHeight:    interpolate height
  // Return:     error code (II_***)
  //
  int iIntegralInterpolate( const RImage<T> &srcpixels,
			    const RImage<T> &dstpixels,
			    RIntegral<T>* tpIntegral,
			    int iWidth, int iHeight );

  //
  // Get FaceBox by using Eye Position
  // 
  // pixels:      original image
  // txleye:      left x eye position
  // tyleye:      left y eye position
  // tRIghtX:     right x eye position
  // tRIghtY:     right y eye position
  // ptC4Points:  4 coordinates
  // tEyeLenRate: Eye Length Rate ( default is II_EYE_LEN_RATE )
  // Return:  crror code (II_***)
  //
  int iGetFaceFromEye( const RImage<T> &pixels,
		       T txleye,  T tyleye,
		       T txreye, T tyreye,
		       CFourCoordinates<T>* ptC4Points,
                       T tEyeLenRate = II_EYE_LEN_RATE );

  //
  // Get FaceBox by using Eye Position
  // You may not want to get 4 points(ptC4Points).
  // You can call iIntegralInterpolate, 
  // because the four coordinates are stored in class.
  // 
  // pixels:  original image
  // txleye:  left x eye position
  // tyleye:  left y eye position
  // tRIghtX: right x eye position
  // tRIghtY: right y eye position
  // Return:  crror code (II_***)
  //
  int iGetFaceFromEye( const RImage<T> &pixels,
		       T txleye,  T tyleye,
		       T txreye, T tyreye );

  //
  // Get FaceBox by using Eye Position
  // You may not want to get 4 points(ptC4Points).
  // You can call iIntegralInterpolate, 
  // because the four coordinates are stored in class.
  // 
  // pixels:      original image
  // txleye:      left x eye position
  // tyleye:      left y eye position
  // tRIghtX:     right x eye position
  // tRIghtY:     right y eye position
  // tEyeLenRate: Eye Length Rate ( default is II_EYE_LEN_RATE )
  // Return:  crror code (II_***)
  //
  int iGetFaceFromEye( const RImage<T> &pixels,
		       T txleye,  T tyleye,
		       T txreye, T tyreye, 
		       T tEyeLenRate );

  void vGetFourCoordinates( CFourCoordinates< T >** ppC4Points );

 protected:
  //
  // Calculate Distance between (x1,y1) and (x2,y2)
  //
  inline T tDistSqrt( T tx1, T ty1, T tx2, T ty2 );

 private:
  // 4 Coordinates
  CFourCoordinates< T > m_tC4Points;
#if II_DO_DUMP
  int m_iCnt;
#endif
};


//
// Constructor
//
template < class T >
CIntegralInterpolate<T>::CIntegralInterpolate()
#if II_DO_DUMP
  : m_iCnt(0)
#endif
{
}

//
// Destructor
//
template < class T >
CIntegralInterpolate<T>::~CIntegralInterpolate()
{
}

//
// Interpolate from Integral Image
//
// srcpixels:  original imageApps/unix/eyefinder/
// dstpixeld:  interpolated image
// tpIntegral: original integral pointer
// iWidth:     interpolate width
// iHeight:    interpolate height
// ptC4Points: interpolate 4 coordinates
// Return:     error code (II_***)
//
template < class T >
int CIntegralInterpolate<T>::iIntegralInterpolate( const RImage<T> &srcpixels,
						   const RImage<T> &dstpixels,
						   RIntegral<T>* tpIntegral,
						   int iWidth, int iHeight,
						   CFourCoordinates< T >* ptC4Points )
{
  //m_tC4Points = *ptC4Points;
  m_tC4Points.vSetFourCoordinates( ptC4Points->m_tx1, ptC4Points->m_ty1,
				   ptC4Points->m_tx2, ptC4Points->m_ty2,
				   ptC4Points->m_tx3, ptC4Points->m_ty3,
				   ptC4Points->m_tx4, ptC4Points->m_ty4 );
  return iIntegralInterpolate( srcpixels, dstpixels, tpIntegral, iWidth, iHeight );
}

//
// Interpolate from Integral Image
// If you use iGetFaceFromEye, you don't need to set 4 Points.
// Because 4 Coordinates are stored in class as members.
//
// srcpixels:  original image
// dstpixeld:  interpolated image
// tpIntegral: original integral pointer
// iWidth:     interpolate width
// iHeight:    interpolate height
// Return:     error code (II_***)
//
template < class T >
int CIntegralInterpolate<T>::iIntegralInterpolate( const RImage<T> &srcpixels,
						   const RImage<T> &dstpixels,
						   RIntegral<T>* tpIntegral,
						   int iWidth, int iHeight )
{
  int iErr = II_NO_ERR;

#if II_DO_PRINT
  cout << " -----" << endl;
#endif
  
  // distance on x axis
  T txd1_2 = m_tC4Points.m_tx2 - m_tC4Points.m_tx1;
  T txd2_3 = m_tC4Points.m_tx3 - m_tC4Points.m_tx2;
  T txd3_4 = m_tC4Points.m_tx4 - m_tC4Points.m_tx3;
  T txd4_1 = m_tC4Points.m_tx1 - m_tC4Points.m_tx4;
#if II_DO_PRINT
  cout << " xd[" 
       << txd1_2 << " "
       << txd2_3 << " "
       << txd3_4 << " "
       << txd4_1 << "]" << endl;
#endif

  // distance on y axis  
#if II_DO_PRINT
  T tyd1_2 = m_tC4Points.m_ty1 - m_tC4Points.m_ty2;
  T tyd2_3 = m_tC4Points.m_ty2 - m_tC4Points.m_ty3;
  T tyd3_4 = m_tC4Points.m_ty3 - m_tC4Points.m_ty4;
#endif
  T tyd4_1 = m_tC4Points.m_ty4 - m_tC4Points.m_ty1;
#if II_DO_PRINT
  cout << " yd[" 
       << tyd1_2 << " "
       << tyd2_3 << " "
       << tyd3_4 << " "
       << tyd4_1 << "]" << endl;
#endif
  
  // distance beween (x*,y*) and (x*,y*)
  T tdist1_2 = tDistSqrt(m_tC4Points.m_tx1,m_tC4Points.m_ty1,m_tC4Points.m_tx2,m_tC4Points.m_ty2);
  T tdist2_3 = tDistSqrt(m_tC4Points.m_tx2,m_tC4Points.m_ty2,m_tC4Points.m_tx3,m_tC4Points.m_ty3);
  T tdist3_4 = tDistSqrt(m_tC4Points.m_tx3,m_tC4Points.m_ty3,m_tC4Points.m_tx4,m_tC4Points.m_ty4);
  T tdist4_1 = tDistSqrt(m_tC4Points.m_tx4,m_tC4Points.m_ty4,m_tC4Points.m_tx1,m_tC4Points.m_ty1);
#if II_DO_PRINT
  cout << " dist[" 
       << tdist1_2 << " "
       << tdist2_3 << " "
       << tdist3_4 << " "
       << tdist4_1 << "]" << endl;
#endif

  // cosine
  T tcos1_2 = txd1_2 / tdist1_2;
  T tcos2_3 = txd2_3 / tdist2_3;
  T tcos3_4 = txd3_4 / tdist3_4;
  T tcos4_1 = txd4_1 / tdist4_1;  
#if II_DO_PRINT
  cout << " cos[" 
       << tcos1_2 << " "
       << tcos2_3 << " "
       << tcos3_4 << " "
       << tcos4_1 << "]" << endl;
#endif

  // sine
#if II_DO_PRINT
  T tsin1_2 = tyd1_2 / tdist1_2;
  T tsin2_3 = tyd2_3 / tdist2_3;
  T tsin3_4 = tyd3_4 / tdist3_4;
#endif
  T tsin4_1 = tyd4_1 / tdist4_1;    
#if II_DO_PRINT
  cout << " sin[" 
       << tsin1_2 << " "
       << tsin2_3 << " "
       << tsin3_4 << " "
       << tsin4_1 << "]" << endl;
#endif
   
  if( ( (int)tdist1_2 != (int)tdist3_4 ) || 
      ( (int)tdist2_3 != (int)tdist4_1 ) ||
      ( (int)( tcos1_2 + tcos3_4 ) != 0 ) || 
      ( (int)( tcos2_3 + tcos4_1 ) != 0 ) ){
#if II_DO_PRINT
    printf(" not rectangle\n");
    printf(" tdist1_2[%d] tdist3_4[%d] tdist2_3[%d] tdist4_1[%d]\n",
	   (int)tdist1_2, (int)tdist3_4, (int)tdist2_3, (int)tdist4_1);
    printf(" tdist1_2[%f] tdist3_4[%f] tdist2_3[%f] tdist4_1[%f]\n",
	   tdist1_2, tdist3_4, tdist2_3, tdist4_1);
    printf(" tcos1_2+tcos3_4[%f] tcos2_3+tcos4_1[%f]\n",
	   tcos1_2+tcos3_4, tcos2_3+tcos4_1);
#endif
    return II_ERR;
  }

  // sampling step
  T txstep = tdist1_2 / iWidth;
  T tystep = tdist2_3 / iHeight;
#if II_DO_PRINT
  cout << " xstep[" << txstep
       << "] ystep[" << tystep
       << "] width[" << iWidth
       << "] height[" << iHeight 
       << "]" << endl;
#endif
      
  // make rectangle to calculate pixel value
  // take half of distance between the pixel and the next pixel (middle point)
#if 0
  // use averate square
  T txdiff = (txstep * II_ABS(tsin4_1) + txstep * II_ABS(tcos4_1)) / 4;
  T tydiff = (tystep * II_ABS(tcos4_1) + tystep * II_ABS(tsin4_1)) / 4;
#else
  // use maximum rectangle
  T txdiff = II_MAX(txstep * II_ABS(tsin4_1), txstep * II_ABS(tcos4_1)) / 2;
  T tydiff = II_MAX(tystep * II_ABS(tcos4_1), tystep * II_ABS(tsin4_1)) / 2;
#endif
#if II_DO_PRINT
  cout << " xdiff[" << txdiff
       << "] ydiff[" << tydiff
       << "]" << endl;
#endif

  // initialize
  int ict = 0;      // counter
  T txinit = 0;     // init position of horizontal line
  T tyinit = 0;     // init position of horizontal line
  T txctr = 0;      // center of current pixel
  T tyctr = 0;      // center of current pixel
  int ixp[2];       // x points in rectangle, which are used to calculate pixel value
  int iyp[2];       // y points in rectangle, which are used to calculate pixel value
  int inormdiv = 1; // the number of pixel, it's used to normarize pixel value
  //RIntegral<T> * pInteg = m_pCEyeFinder->getIntegralPtr();
  T *p = dstpixels.array; // destination buffer, which interpolate image is stored

  // interpolate
  for( int i = 0; i < iHeight; i++ ){
    
    // init position of horizontal line
    txinit = m_tC4Points.m_tx1 - i * txstep * tcos4_1;
    tyinit = m_tC4Points.m_ty1 + i * tystep * tsin4_1;

    for( int j = 0; j <  iWidth; j++ ){
      ict = i*iHeight+j; // counter
      
      // center of pixel
      txctr = txinit + j * txstep * tsin4_1;
      tyctr = tyinit + j * tystep * tcos4_1;
      
      // x coordinate      
      ixp[0] = (int)(txctr - txdiff + 0.5 );
      ixp[1] = (int)(txctr + txdiff + 0.5 ) + 1;
      // y coordinate      
      iyp[0] = (int)(tyctr - tydiff + 0.5 );
      iyp[1] = (int)(tyctr + tydiff + 0.5 ) + 1;

      // check size
      if( ixp[0] < 0 ){
	ixp[0] = 0;
      }
      if( iyp[0] < 0 ){
	iyp[0] = 0;
      }
      if( ixp[1] >= srcpixels.width ){
	ixp[1] = srcpixels.width;
      }
      if( iyp[1] >= srcpixels.height ){
	iyp[1] = srcpixels.height;
      }

      // number of pixels
      inormdiv = (ixp[1]-ixp[0]) * (iyp[1]-iyp[0]);
      
      // sampling data
      *(p++) = ( tpIntegral->getPixel(ixp[1],iyp[1])
		 - tpIntegral->getPixel(ixp[1],iyp[0])
		 - tpIntegral->getPixel(ixp[0],iyp[1])
		 + tpIntegral->getPixel(ixp[0],iyp[0]) ) / inormdiv;
#if 0 //II_DO_PRINT
      printf("[%d: xp[%d %d] yp[%d %d] dif[%d] p[%f] [%f-%f-%f+%f]", 
	     ict, ixp[0], ixp[1], iyp[0], iyp[1], inormdiv, *(p-1),
	     tpIntegral->getPixel(ixp[1],iyp[1]),tpIntegral->getPixel(ixp[1],iyp[0]),
	     tpIntegral->getPixel(ixp[0],iyp[1]),tpIntegral->getPixel(ixp[0],iyp[0]));
#endif
    }
  }

#ifdef II_USE_IMAGEMAGICK
#if II_DO_DISPLAY
  Image imaged(iHeight, iWidth, "I", FloatPixel, dstpixels.array );
  imaged.display();
#endif
#if II_DO_DUMP
  ///--- If the picture is not normalized, Do normalize here.
  float fSrc;
  float fMax = 0;
  RImage<float> rtmpimg(iHeight,iWidth);
  for(int y = 0; y < iHeight; y++){
    for(int x = 0; x < iWidth; x++){
      fSrc = dstpixels.getPixel(y*iWidth+x);
      fMax = II_MAX(fMax,fSrc);
      rtmpimg.setPixel( x,y, fSrc/2550 );
      //printf("%f ",fSrc);
    }
  }
  //printf("\nMax %f\n", fMax);

  ///--- Make filename from time and counter.
  char acFileName[256];
  time_t timer;
  struct tm *ptm;
  time(&timer);
  ptm = localtime(&timer);
  sprintf(acFileName, "faceimg/face_%04d%02d%02d%02d%02d%02d%02d.tiff",
	  ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, 
	  ptm->tm_hour, ptm->tm_min, ptm->tm_sec, m_iCnt%10);
  m_iCnt++;
  
  ///--- Output face image
  try { 
    if( fMax > 1 ){
      // Use Normalized image
      Image imagew(iWidth, iHeight, "I", FloatPixel, rtmpimg.array );
      printf(" Output face: %s\n", acFileName);
      imagew.write(acFileName);
    }
    else{
      // image is normalized.
      Image imagew(iWidth, iHeight, "I", FloatPixel, dstpixels.array );
      imagew.write("face.tiff");
    }
  }catch ( Exception &error ) {
    cerr << " Error writing file: " 
	 <<  (( fMax > 1 ) ? acFileName : "face.tiff")
	 << ". ImageMagick said: " 
	 << error.what() 
	 << endl;
  }
#endif //II_DO_DUMP
#endif //II_USE_IMAGEMAGICK

  return iErr;
}

//
// Get FaceBox by using Eye Position
// 
// pixels:      original image
// txleye:      left x eye position
// tyleye:      left y eye position
// tRIghtX:     right x eye position
// tRIghtY:     right y eye position
// ptC4Points:  4 coordinates
// tEyeLenRate: Eye Length Rate ( default is II_EYE_LEN_RATE )
// Return:  crror code (II_***)
//
template < class T >
int CIntegralInterpolate<T>::iGetFaceFromEye( const RImage<T> &pixels,
					      T txleye,  T tyleye,
					      T txreye, T tyreye,
					      CFourCoordinates<T>* ptC4Points,
					      T tEyeLenRate )
{
  int iErr = II_NO_ERR;
  iErr = iGetFaceFromEye( pixels, txleye, tyleye, txreye, tyreye, tEyeLenRate );
  if( iErr ){
    return iErr;
  }

  // *ptC4Points = m_tC4Poitns;
  ptC4Points->vSetFourCoordinates( m_tC4Points.m_tx1, m_tC4Points.m_ty1,
				   m_tC4Points.m_tx2, m_tC4Points.m_ty2,
				   m_tC4Points.m_tx3, m_tC4Points.m_ty3,
				   m_tC4Points.m_tx4, m_tC4Points.m_ty4 );
  return iErr;
}

//
// Get FaceBox by using Eye Position
// You may not want to get 4 points(ptC4Points).
// You can call iIntegralInterpolate, 
// because the four coordinates are stored in class.
// 
// pixels:  original image
// txleye:  left x eye position
// tyleye:  left y eye position
// tRIghtX: right x eye position
// tRIghtY: right y eye position
// Return:  crror code (II_***)
//
template < class T >
int CIntegralInterpolate<T>::iGetFaceFromEye( const RImage<T> &pixels,
					      T txleye,  T tyleye,
					      T txreye, T tyreye )
{
  return iGetFaceFromEye( pixels, txleye, tyleye, txreye, tyreye, II_EYE_LEN_RATE );
}

//
// Get FaceBox by using Eye Position
// You may not want to get 4 points(ptC4Points).
// You can call iIntegralInterpolate, 
// because the four coordinates are stored in class.
// 
// pixels:      original image
// txleye:      left x eye position
// tyleye:      left y eye position
// tRIghtX:     right x eye position
// tRIghtY:     right y eye position
// tEyeLenRate: Eye Length Rate
// Return:  crror code (II_***)
//
template < class T >
int CIntegralInterpolate<T>::iGetFaceFromEye( const RImage<T> &pixels,
					      T txleye,  T tyleye,
					      T txreye, T tyreye, T tEyeLenRate )
{
  //int iErr = II_NO_ERR;

  //T tEyeLenRate = II_EYE_LEN_RATE; <- input from argument
  T tEyeTopRate = II_EYE_TOP_RATE;
  //T tEyeLeftRate = II_EYE_LEFT_RATE;
  //T tEyeRightRate = II_EYE_RIGHT_RATE;
  T tEyeLen = tDistSqrt( txleye, tyleye, txreye, tyreye );
  T txEyeAvg = ( txleye + txreye ) / 2;
  T tyEyeAvg = ( tyleye + tyreye ) / 2;

  T txd = txleye - txreye;
  T tyd = tyleye - tyreye;
  T tcosd = txd / tEyeLen;
  T tsind = tyd / tEyeLen;
  T tFaceSize = tEyeLen / tEyeLenRate;

#if II_DO_PRINT
  cout << " --- iGetFaceFromEye ---" << endl;
  cout << " eye[" << txleye 
       << " "     << tyleye 
       << " "     << txreye 
       << " "     << tyreye 
       << "]" << endl;
  cout << " tEyeLen["  << tEyeLen 
       << "] tEyeAvg[" << txEyeAvg 
       << " "          << tyEyeAvg 
       << "]" << endl;
  cout << " tFaceSize[" << tFaceSize 
       << "]" << endl;
  cout << " txd[ "  << txd 
       << "] tyd["  << tyd 
       << "] tcos[" << tcosd 
       << "] tsin[" << tsind 
       << "]" << endl;
#endif

#if II_DO_LOG
  ofstream fout("smilelog",ios::app);
  fout <<" eye[" << txleye 
       << " "     << tyleye 
       << " "     << txreye 
       << " "     << tyreye 
       << "]" << endl;
  fout << " tEyeLen["  << tEyeLen 
       << "] tFaceSize[" << tFaceSize 
       << "]" << endl;
  fout.close();
#endif
  
  if( ( tcosd == 1 ) && ( tsind == 0 ) ){
#if II_DO_PRINT
    printf(" degree 0\n");
#endif
    //
    // Face Rectangle is like this.
    // (x0,y0)----(x1,y0)
    //    |          |
    // (x0,y1)----(x1,y1)
    //
    T tx0 = txEyeAvg - tFaceSize / 2;
    T tx1 = txEyeAvg + tFaceSize / 2;    
    T ty0 = tyEyeAvg - tEyeTopRate * tFaceSize;
    T ty1 = tyEyeAvg + (1 - tEyeTopRate) * tFaceSize;

    // new 4 coordinates
    m_tC4Points.m_tx1 = tx0;
    m_tC4Points.m_tx2 = tx1;
    m_tC4Points.m_tx3 = tx1;
    m_tC4Points.m_tx4 = tx0;
    m_tC4Points.m_ty1 = ty0;
    m_tC4Points.m_ty2 = ty0;
    m_tC4Points.m_ty3 = ty1;
    m_tC4Points.m_ty4 = ty1;
  }
  else{
    T txup = txEyeAvg + tEyeTopRate * tFaceSize * tsind;
    T tyup = tyEyeAvg - tEyeTopRate * tFaceSize * tcosd;
    T txdown = txEyeAvg - (1 - tEyeTopRate) * tFaceSize * tsind;
    T tydown = tyEyeAvg + (1 - tEyeTopRate) * tFaceSize * tcosd;

#if II_DO_PRINT
    T tdistctr = tDistSqrt( txup, tyup, txdown, tydown );
    T txdctr = abs(txup - txdown);
    T tydctr = abs(tyup - tydown);
    cout << " txup["     << txup 
	 << "] tyup["    << tyup
	 << "] txdown [" << txdown 
	 << "] tydown["  << tydown 
	 << "]" << endl;
    // should be equal to cosd and sind
    cout << " Center cos[" << tydctr / tdistctr 
	 << "] sin[" << txdctr / tdistctr 
	 << "]" << endl;
#endif

#if 1 // use T
    m_tC4Points.m_tx1 = txup - tFaceSize / 2 * tcosd;
    m_tC4Points.m_ty1 = tyup - tFaceSize / 2 * tsind;
    m_tC4Points.m_tx2 = txup + tFaceSize / 2 * tcosd;
    m_tC4Points.m_ty2 = tyup + tFaceSize / 2 * tsind;
    m_tC4Points.m_tx3 = txdown + tFaceSize / 2 * tcosd;
    m_tC4Points.m_ty3 = tydown + tFaceSize / 2 * tsind;
    m_tC4Points.m_tx4 = txdown - tFaceSize / 2 * tcosd;
    m_tC4Points.m_ty4 = tydown - tFaceSize / 2 * tsind;
#else // use int
    m_tC4Points.m_tx1 = (T)((int)(txup - tFaceSize / 2 * tcosd + 0.5));
    m_tC4Points.m_ty1 = (T)((int)(tyup - tFaceSize / 2 * tsind + 0.5));
    m_tC4Points.m_tx2 = (T)((int)(txup + tFaceSize / 2 * tcosd + 0.5));
    m_tC4Points.m_ty2 = (T)((int)(tyup + tFaceSize / 2 * tsind + 0.5));
    m_tC4Points.m_tx3 = (T)((int)(txdown + tFaceSize / 2 * tcosd + 0.5));
    m_tC4Points.m_ty3 = (T)((int)(tydown + tFaceSize / 2 * tsind + 0.5));
    m_tC4Points.m_tx4 = (T)((int)(txdown - tFaceSize / 2 * tcosd + 0.5));
    m_tC4Points.m_ty4 = (T)((int)(tydown - tFaceSize / 2 * tsind + 0.5));
#endif
  }

#if FC_DO_PRINT
  m_tC4Points.vPrintFourCoordinates();
#endif

  // check size
  if( m_tC4Points.iCheckFourCoordinates( pixels ) ){
    return II_ERR;
  }

  return II_NO_ERR;
}

template < class T >
void CIntegralInterpolate<T>::vGetFourCoordinates( CFourCoordinates< T >** ppC4Points )
{
  *ppC4Points = &m_tC4Points;
}


//
// Calculate Distance between (x1,y1) and (x2,y2)
//
template < class T >
inline T CIntegralInterpolate<T>::tDistSqrt( T tx1, T ty1, T tx2, T ty2 )
{
  return (T)sqrt( (double)( tx1 - tx2 ) * (double)( tx1 - tx2 )
		  + (double)( ty1 - ty2 ) * (double)( ty1 - ty2 ) );
}


//
// Class, Interpolate using Integral Image
//
/*class CIntegralInterpolateFloat : public CIntegralInterpolate<float> {
 public:
  CIntegralInterpolateFloat(){};
  virtual ~CIntegralInterpolateFloat(){};
  };*/

#endif // __INTERPOLATE_H__


/*
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 
 *    1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *    3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
