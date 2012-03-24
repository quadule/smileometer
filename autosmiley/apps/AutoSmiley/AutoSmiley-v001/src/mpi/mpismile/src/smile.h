/*
 *  smile detector.
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

#ifndef __SMILE_H__
#define __SMILE_H__

//
// Define for Include
//
#define SMD_COMPILE_MAC	        (0)
#define SMD_USE_VIMAGE		(0)
//// To use this, use "bjam debug"
//#define SMD_USE_IMAGEMAGICK	(1)
#define SMD_USE_BLAS		(0)

//
// Define for RUBI
//
#define SMD_USE_RUBI		(0)
#if SMD_USE_RUBI
extern int vision_save_faceDetect_req;
#endif
//
// Include
//
#include <eyefinderBinary.h>
#include "interpolate.h"
//#include "cluster.h"
#if SMD_COMPILE_MAC
#include <Carbon/Carbon.h>
#endif

#if SMD_USE_VIMAGE
#include <Accelerate/Accelerate.h>
#include </System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vImage.framework/Versions/A/Headers/Geometry.h>
#endif

#ifdef SMD_USE_IMAGEMAGICK
#include <Magick++.h>
using namespace std;
using namespace Magick;
#endif

#if SMD_USE_BLAS
#include <vecLib/vBLAS.h>
#endif

//
// Define
//
// Err
#define SMD_NO_ERR		(0)
#define SMD_ERR			(1)
#define SMD_NOT_SUPPORT		(2)
// CONSTANT VALUE
#define SMD_GRAY_PIXEL_SIZE		(1)
#define SMD_GRAY_FLOAT_PIXEL_SIZE	(4)
#define SMD_RGB_PIXEL_SIZE		(3)
#define SMD_ARGB_PIXEL_SIZE		(4)
#define SMD_MAX_PIXEL_VALUE		(255)
#define SMD_FACE_SIZE			(24)
#define SMD_FACE_AREA_SIZE		(576)
// create facebox by eye position
//#define SMD_PI						(3.141592)
#define SMD_PI	((double)3.14159265358979323846264338327950288419716939937510)
#define SMD_360_DEGREE			(360)
#define SMD_EYE_LEN_RATE		(0.50)
#define SMD_EYE_LEFT_RATE		(0.25)
#define SMD_EYE_RIGHT_RATE		(0.25)
#define SMD_EYE_TOP_RATE		(0.20)

// Macro
#define SMD_MAX(x,y)            ( (x > y) ? (x) : (y) )

// Debug
#define SMD_DO_PRINT		(0)
#define SMD_DO_TIME		(0)
#ifdef SMD_USE_IMAGEMAGICK
#define SMD_DO_DUMP		(0)
#define SMD_DO_DUMP_FACE	(0)
#else
#define SMD_DO_DUMP		(0)
#define SMD_DO_DUMP_FACE	(0)
#endif
#define SMD_DO_DISPLAY		(0)
#define SMD_DO_LOG		(0)

#if SMD_DO_TIME
#include <sys/time.h>
double dSMDdifftv(timeval t1, timeval t0) {
  double s;
  double secs = t1.tv_sec - t0.tv_sec;
  double usecs = ((double)t1.tv_usec) - ((double)t0.tv_usec);
  usecs *= 1e-6;
  s = secs+usecs;
  return s;
}
#endif

//
// Struct
//
//struct SmileValue{
//  int smileMax;
//  int smileTotal;
//  float smileMean;
//};

//
// Class, Smile Detector
//
class MPSmile : public MPISearchFaceDetector {
 public:
  MPSmile();
  virtual ~MPSmile();

  //
  // Find Smiles with EyeFinder & FaceDetector
  //
  // pixels:   original image
  // faces:    face information
  // WINSHIFT: window shift size for mpisearch
  // mode:     mode for mpisearch
  // Return:   error code (SMD_***)
  //
  int findSmiles( const RImage<float> &pixels, VisualObject &faces,
		  float WINSHIFT=1, combine_mode mode=none );

  //
  // Find Smiles with specified Eye Position
  //
  // pixels:   original image
  // faces:    face information
  // WINSHIFT: window shift size for mpisearch
  // mode:     mode for mpisearch
  // fxleye:   X Left Eye
  // fyleye:   Y Left Eye
  // fxreye:   X Right Eye
  // fyreye:   Y Right Eye
  // Return:   error code (SMD_***)
  //
  int findSmiles( const RImage<float> &pixels, VisualObject &faces,
		  float fxleye, float fyleye, 
		  float fxreye, float fyreye,
		  float WINSHIFT=1, combine_mode mode=none );

  //
  // Find Smiles from Patch
  //
  // pixels:   face image
  // faces:    face information
  // WINSHIFT: window shift size for mpisearch
  // mode:     mode for mpisearch
  // Return:   error code (SMD_***)
  //
  int findSmilesFromPatch( const RImage<float> &pixels, VisualObject &faces,
			   float WINSHIFT=1, combine_mode mode=none );

#ifdef SMD_USE_IMAGEMAGICK	
  //
  // Find Smiles using ImageMagick
  //
  // pimage:   ImageMagick image pointer
  // pVOFace:  face information
  // WINSHIFT: window shift size for mpisearch
  // mode:     mode for mpisearch
  // Return:   error code (SMD_***)
  //
  int findSmiles( Image* pimage, VisualObject* pVOFaces,
		  float WINSHIFT=1, combine_mode mode=none );
#endif	

  // Overload the initStream and resetStream to do the eye detector
  virtual void initStream(const int width, const int height, double WINSHIFT=1.0);
  virtual void resetStream(const int width, const int height, double WINSHIFT=1.0);
  virtual void releaseStream();

 protected:
  //
  // GetSmiles
  //
  int iGetSmiles( const RImage<float> &pixels, VisualObject &faces );

  //
  // Search Smile
  //
  // pixels -- 			the image to be scanned
  // faces  -- 			structure to store the detected objects (faces)
  // flags  -- 			an array with a value corresponding to each window at all scales
  // output_values -- 		Stick the output of the classifier up to this point here
  // box -- 			flag to return the found face boxes.  Set to 0 to get activation only.
  // Note: flags and output_values are being used during learning, only used from Matlab right now
  int iSearchSmile( const RImage<float> &pixels, FaceBoxList &face, int USE_BLOCK_FLAGS = 1,
		    float WINSHIFT = 1.25, double* index_flags = NULL,
		    double* output_values = NULL, int box = 1 );

#ifdef SMD_USE_IMAGEMAGICK	
  //
  // Get Smiles using ImageMagick
  //
  int iGetSmiles( Image* pimage, const RImage<float> &pixels, VisualObject &faces );

  //
  // Adjust FaceBox by using Eye Position
  //
  int iGetFaceFromEye( Image* pimage, 
		       float fxleye, float fyleye, 
		       float fxreye, float fyreye );

  // ImageMagick Grayscale
  int iGrayscale_ImageMagick( Image* pimage );

  // ImageMagick Crop
  int iCrop_ImageMagick( Image* pimage, 
			 int iWidth, int Height, 
			 int iXOffset, int iYOffset );
	
  // ImageMagick Resize
  int iResize_ImageMagick( Image* pimage, int iWidth, int iHeight );
	
  // ImageMagick Rotate
  int iRotate_ImageMagick( Image* pimage, double dAngle );
	
  // ImageMagick Write Face Image to Buffer
  int iWrite_ImageMagick( Image* pimage, const RImage<float>* prPixels );		
#endif // SMD_USE_IMAGEMAGICK

#if SMD_DO_DUMP_FACE
  void vDumpFace( void );
#endif	
#if SMD_DO_DUMP	
  void vDumpPixels( const RImage<float>* prPixels );
#endif	

   
 private:
  // EyeFinder
  MPEyeFinder* m_pCEyeFinder;
  RImage<float>* m_prPixels;
  int m_iLastImgWidth;
  int m_iLastImgHeight;

  // SmileDetector
  RImage<float>* m_prFacePixels;
  //SmileValue m_rSmileVal;
  //double m_dActivation;

  // Interpolate
  CIntegralInterpolate<float>* m_pfCIntegInter;

  // Debug
  int m_iCount;
};

#endif // __SMILE_H__


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
