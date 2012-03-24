/*
 *  rect
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

#ifndef __AREA_H__
#define __AREA_H__

// Include
//
#include <iostream>
//#include <math.h>


//
// Define
//
// Err Code (CArea)
#define AA_NO_ERR		(0)
#define AA_ERR			(1)
#define AA_NOT_SUPPORT		(2)
// Debug
#define AA_DO_PRINT		(1)

// Macro
#define AA_MAX(x,y)            ( (x > y) ? (x) : (y) )
#define AA_MIN(x,y)            ( (x < y) ? (x) : (y) )

// Struct
//typeder struct RAAArea{
//  int ix1;
//  int iy1;
//  int ix2;
//  int iy2;
//}RAAArea

template < class T >
class CBaseArea {
 public:
  CBaseArea();
  CBaseArea( T tix1, T tiy1, 
	     T tix2, T tiy2 );
  virtual ~CBaseArea();

  //
  // Set Area
  //
  // tix*: x coordinates
  // tiy*: y coordinates
  // Return: error code (AA_***)
  //
  inline void vSetBaseArea( T tix1, T tiy1, 
			    T tix2, T tiy2 );

  //
  // Get Area
  //
  // ptox*: x coordinates
  // ptoy*: y coordinates
  // Return: error code (AA_***)
  //
  inline void vGetBaseArea( T* ptox1, T* ptoy1,
			    T* ptox2, T* ptoy2 );
  
#if AA_DO_PRINT
  //  // If you use iGetFaceFromEye, you don't need to set 4 Points.
  // Because 4 Coordinates are stored in class as members.
  // Print 4 coordinates
  //
  void vPrintBaseArea( void );
#endif

 public:
  //
  // Area (x1 < x2) && (y1 < y2)
  //
  //   Left-Up             Right-Up
  //   (x1,y1)---------(x2,y1)
  //      |               |
  //      |               |
  //      |               |
  //   (x1,y2)---------(x2,y2)
  // Left-Bottom       Right-Bottom
  //
  T m_tx1;
  T m_ty1;
  T m_tx2;
  T m_ty2;
};

//
// Constructor
//
template < class T >
CBaseArea<T>::CBaseArea() :
  m_tx1( 0 ),
  m_ty1( 0 ),
  m_tx2( 0 ),
  m_ty2( 0 )
{
}

template < class T >
CBaseArea<T>::CBaseArea( T tix1, T tiy1, 
			 T tix2, T tiy2 ) :
  m_tx1( tix1 ),
  m_ty1( tiy1 ),
  m_tx2( tix2 ),
  m_ty2( tiy2 )
{

}

//
// Destructor
//
template < class T >
CBaseArea<T>::~CBaseArea()
{
}

//
// Set Area
//
// tix*: x coordinates
// tiy*: y coordinates
// Return: error code (AA_***)
//
template < class T >
inline void CBaseArea<T>::vSetBaseArea( T tix1, T tiy1, 
					T tix2, T tiy2 )
{
  m_tx1 = tix1;
  m_ty1 = tiy1;
  m_tx2 = tix2;
  m_ty2 = tiy2;
}

//
// Get Area
//
// ptix*: x coordinates
// ptiy*: y coordinates
// Return: error code (AA_***)
//
template < class T >
inline void CBaseArea<T>::vGetBaseArea( T* ptox1, T* ptoy1,
					T* ptox2, T* ptoy2 )
{
  *ptox1 = m_tx1;
  *ptoy1 = m_ty1;
  *ptox2 = m_tx2;
  *ptoy2 = m_ty2;
}

#if AA_DO_PRINT
//  
// Print Area
//
template < class T >
void CBaseArea<T>::vPrintBaseArea( void )
{
  cout << " x1[" << m_tx1
       << "] y1[" << m_ty1
       << "] x2[" << m_tx2
       << "] y2[" << m_ty2
       << "]" << endl;
}
#endif

//
//
// Class, Area
//
template < class T >
class CArea {
 public:
  CArea();
  virtual ~CArea();

  //
  // Get AND Rect
  //
  // pArea1: Area 1
  // pArea2: Area 2
  // Return: BaseArea
  //
  inline CBaseArea<T>* CGetANDRect( CBaseArea<T>* pArea1, CBaseArea<T>* pArea2 );

  //
  // Get Area
  //
  // pArea:  Area
  // Return: Area value
  //
  inline T tGetArea( CBaseArea<T>* pArea );

  //
  // Get AND Area
  //
  // pArea1: Area 1
  // pArea2: Area 2
  // Return: AND Area value
  //
  inline T tGetANDArea( CBaseArea<T>* pArea1, CBaseArea<T>* pArea2 );

  //
  // Get OR Area
  //
  // pArea1: Area 1
  // pArea2: Area 2
  // Return: OR Area value
  //
  inline T tGetORArea( CBaseArea<T>* pArea1, CBaseArea<T>* pArea2 );
  
 public:
  CBaseArea<T> m_tCBaseArea;

};
//
// Constructor
//
template < class T >
CArea<T>::CArea()
{
}

//
// Destructor
//
template < class T >
CArea<T>::~CArea()
{
}

//
// Get AND Rect
//
// pArea1: Area 1
// pArea2: Area 2
// Return: BaseArea
//
template < class T >
inline CBaseArea<T>* CArea<T>::CGetANDRect( CBaseArea<T>* pArea1, CBaseArea<T>* pArea2 )
{
  m_tCBaseArea.m_tx1 = AA_MAX( pArea1->m_tx1, pArea2->m_tx1 );
  m_tCBaseArea.m_ty1 = AA_MAX( pArea1->m_ty1, pArea2->m_ty1 );
  m_tCBaseArea.m_tx2 = AA_MIN( pArea1->m_tx2, pArea2->m_tx2 );
  m_tCBaseArea.m_ty2 = AA_MIN( pArea1->m_ty2, pArea2->m_ty2 );
  return &(m_tCBaseArea);
}

//
// Get Area
//
// pArea:  Area
// Return: Area value
//
template < class T >
inline T CArea<T>::tGetArea( CBaseArea<T>* pArea )
{
  return ( pArea->m_tx2 - pArea->m_tx1 ) * ( pArea->m_ty2 - pArea->m_ty1 );
}

//
// Get AND Area
//
// pArea1: Area 1
// pArea2: Area 2
// Return: AND Area value
//
template < class T >
inline T CArea<T>::tGetANDArea( CBaseArea<T>* pArea1, CBaseArea<T>* pArea2 )
{
  if( ( pArea1->m_tx2 < pArea2->m_tx1 ) || ( pArea2->m_tx2 < pArea1->m_tx1 ) ){
    return 0;
  }
  else if( ( pArea1->m_ty2 < pArea2->m_ty1 ) || ( pArea2->m_ty2 < pArea1->m_ty1 ) ){
    return 0;
  }

  return tGetArea( CGetANDRect(pArea1, pArea2) );
}

//
// Get OR Area
//
// pArea1: Area 1
// pArea2: Area 2
// Return: OR Area value
//
template < class T >
inline T CArea<T>::tGetORArea( CBaseArea<T>* pArea1, CBaseArea<T>* pArea2 )
{
  return tGetArea(pArea1) + tGetArea(pArea2) - tGetANDArea(pArea1, pArea2);
}

#endif // __AREA_H__
