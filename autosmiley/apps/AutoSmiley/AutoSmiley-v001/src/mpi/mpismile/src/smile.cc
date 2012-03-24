/*
 *  smile.cc
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

//
// Inclue
//
#include <iostream>
#include <string.h> 
#include <math.h>
#include <list>
#include "smile.h"
#include "smile_wts.h"

//
// Constructor
//
MPSmile::MPSmile() :
  MPISearchFaceDetector (0),
  m_prPixels( NULL ),
  m_iLastImgWidth( 0 ),
  m_iLastImgHeight( 0 ),
  m_iCount( 0 ),
  m_pCEyeFinder( 0 )
{
  // Load Data for SmileDetector
  smile_wts::assignData(data);
  //printMyData(0);
  // Init SmileDetector memory
  stream.init(24, 24, m_imagetypes, data, 1);

  // Init EyeFinder
  m_pCEyeFinder = new MPEyeFinderBinary(); // Note: pass in "1" to make it use adaboost facedetector, which 
         // is a bit slower but more accurate for eyes since it was originally trained with the adaboost face detector

  // Init Face Pixels
  m_prFacePixels = new RImage<float>(SMD_FACE_SIZE, SMD_FACE_SIZE);

  // Init IntegralInterpolate Class
  m_pfCIntegInter = new CIntegralInterpolate<float>();

  // init struct
  //memset( &m_rSmileval, 0, sizeof(SmileValue) );
}

//
// Destructor
//
MPSmile::~MPSmile(){
  // Delete EyeFinder
  if( m_pCEyeFinder ){
    delete m_pCEyeFinder;
    m_pCEyeFinder = NULL;
  }

  // Delete Original Image
  if( m_prPixels ){
    delete m_prPixels;
    m_prPixels = NULL;
  }

  // Delete Face Image
  if( m_prFacePixels ){
    delete m_prFacePixels;
    m_prFacePixels = NULL;
  }

  // Delete IntegralInterpolate Class
  if( m_pfCIntegInter ){
    delete m_pfCIntegInter;
    m_pfCIntegInter = NULL;
  }
}

//
// Find Smiles
//
// pixels:   original image
// faces:    face information
// WINSHIFT: window shift size for mpisearch
// mode:     mode for mpisearch
// Return:   error code (SMD_***)
//
int MPSmile::findSmiles( const RImage<float> &pixels, VisualObject &faces,
			 float WINSHIFT, combine_mode mode )
{
  int iErr = SMD_NO_ERR;

  // First, find the faces
  if( pixels.width != m_iLastImgWidth || pixels.height != m_iLastImgHeight ){
    m_pCEyeFinder->resetStream(pixels.width, pixels.height);
    m_iLastImgWidth = pixels.width;
    m_iLastImgHeight = pixels.height;
  }
#if SMD_DO_DUMP
  vDumpPixels( &pixels );
#endif

#if SMD_DO_TIME
  struct timeval tv1, tv2;
  gettimeofday(&tv1,0);
#endif
  m_pCEyeFinder->findEyes(pixels, faces, WINSHIFT, mode);
#if SMD_USE_RUBI
  vision_save_faceDetect_req = 1; // find face
#endif
  if(!faces.size()){
#if SMD_USE_RUBI
    vision_save_faceDetect_req = 0; // not find face
#endif

#if SMD_DO_PRINT
    printf(" No Face\n");
#endif
    return iErr;
  }
#if SMD_DO_TIME
  gettimeofday(&tv2,0);
  printf(" EyeFinder took %g seconds.\n", dSMDdifftv(tv2, tv1) );
#endif

#if SMD_DO_TIME
  gettimeofday(&tv1,0);
#endif
  iErr = iGetSmiles(pixels, faces);
  if( iErr ){
    //printf(" iGetSmiles error\n");
    return iErr;
  }
#if SMD_DO_TIME
  gettimeofday(&tv2,0);
  printf(" SmileDetector took %g seconds.\n", dSMDdifftv(tv2, tv1) );
#endif

  return iErr;
}

//
// Find Smiles with check label
//
// pixels:   original image
// faces:    face information
// WINSHIFT: window shift size for mpisearch
// mode:     mode for mpisearch
// fxleye:   Labeled X Left Eye
// fyleye:   Labeled Y Left Eye
// fxreye:   Labeled X Right Eye
// fyreye:   Labeled Y Right Eye
// Return:   error code (SMD_***)
//
/*int MPSmile::findSmilesWithCheckLabel( const RImage<float> &pixels, VisualObject &faces,
				       float fxleye, float fyleye, 
				       float fxreye, float fyreye,
				       float WINSHIFT=1, combine_mode mode=none )
{
  int iErr = SMD_NO_ERR;

  // First, find the faces
  if( pixels.width != m_iLastImgWidth || pixels.height != m_iLastImgHeight ){
    m_pCEyeFinder->resetStream(pixels.width, pixels.height);
    m_iLastImgWidth = pixels.width;
    m_iLastImgHeight = pixels.height;
  }
#if SMD_DO_DUMP
  vDumpPixels( &pixels );
#endif

#if SMD_DO_TIME
  struct timeval tv1, tv2;
  gettimeofday(&tv1,0);
#endif
  m_pCEyeFinder->findEyes(pixels, faces, WINSHIFT, mode);
  if(!faces.size()){
#if SMD_DO_PRINT
    printf(" No Face\n");
#endif
    return iErr;
  }
#if SMD_DO_TIME
  gettimeofday(&tv2,0);
  printf(" EyeFinder took %g seconds.\n", dSMDdifftv(tv2, tv1) );
#endif

  // Prepare Original FaceBox from labeled eye position
  CFourCoordinates<float> fC4Points;
  float fyavg = ( fyleye + fyreye ) / 2;
  iErr = m_pfCIntegInter->iGetFaceFromEye( pixels, 
					   fxleye, fyavg, fxreye, fyavg,
					   &fC4Points );


#if SMD_DO_TIME
  gettimeofday(&tv1,0);
#endif
  iErr = iGetSmiles(pixels, faces);
  if( iErr ){
    //printf(" iGetSmiles error\n");
    return iErr;
  }
#if SMD_DO_TIME
  gettimeofday(&tv2,0);
  printf(" SmileDetector took %g seconds.\n", dSMDdifftv(tv2, tv1) );
#endif

  return iErr;
}*/

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
int MPSmile::findSmiles( const RImage<float> &pixels, VisualObject &faces,
			 float fxleye, float fyleye, 
			 float fxreye, float fyreye,
			 float WINSHIFT, combine_mode mode )
{
  int iErr = SMD_NO_ERR;

  // First, find the faces
  if( pixels.width != m_iLastImgWidth || pixels.height != m_iLastImgHeight ){
    m_pCEyeFinder->resetStream(pixels.width, pixels.height);
    m_iLastImgWidth = pixels.width;
    m_iLastImgHeight = pixels.height;
  }
#if SMD_DO_DUMP
  vDumpPixels( &pixels );
#endif

#if SMD_DO_TIME
  struct timeval tv1, tv2;
  gettimeofday(&tv1,0);
#endif
  // For create Integral Image, not using EyeFinder result
  m_pCEyeFinder->findEyes(pixels, faces, WINSHIFT, mode);
#if SMD_DO_TIME
  gettimeofday(&tv2,0);
  printf(" EyeFinder took %g seconds.\n", dSMDdifftv(tv2, tv1) );
#endif

  // put 1 face eye position information
#if 1
  faces.clear();
  FaceObject *current_face = new FaceObject();
  current_face->eyes.rightEye = true;
  current_face->eyes.leftEye = true;
  current_face->eyes.xLeft = fxleye;
  current_face->eyes.yLeft = fyleye;
  current_face->eyes.xRight = fxreye;
  current_face->eyes.yRight = fyreye;
  faces.push_front(current_face);
#else
  list<VisualObject *>::iterator it = faces.begin();
  FaceObject * f = static_cast<FaceObject*>(*it);
  f->eyes.xLeft = fxleye;
  f->eyes.yLeft = fyleye; 
  f->eyes.xRight = fxreye;
  f->eyes.yRight = fyreye;
#endif

#if SMD_DO_TIME
  //struct timeval tv1, tv2;
  gettimeofday(&tv1,0);
#endif
  iErr = iGetSmiles(pixels, faces);
  if( iErr ){
    //printf(" iGetSmiles error\n");
    return iErr;
  }
#if SMD_DO_TIME
  gettimeofday(&tv2,0);
  printf(" SmileDetector took %g seconds.\n", dSMDdifftv(tv2, tv1) );
#endif

  return iErr;
}

//
// Find Smiles from Patch
//
// pixels:   face image
// faces:    face information
// WINSHIFT: window shift size for mpisearch
// mode:     mode for mpisearch
// Return:   error code (SMD_***)
//
int MPSmile::findSmilesFromPatch( const RImage<float> &pixels, VisualObject &faces,
				  float WINSHIFT, combine_mode mode )
{
  int iErr = SMD_NO_ERR;
  FaceBoxList face;

  // put 1 dummy faceobject
  faces.clear();
  FaceObject *current_face = new FaceObject();
  current_face->eyes.rightEye = true;
  current_face->eyes.leftEye = true;
  faces.push_front(current_face);

  // get dummy faceobject
  list<VisualObject *>::iterator it = faces.begin();
  FaceObject * f = static_cast<FaceObject*>(*it);

  // search smile
  double activation;
  iSearchSmile(pixels, face, 0, 1.0f, 0, &activation, 0);
  f->activation = activation;
  return iErr;
}

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
int MPSmile::findSmiles( Image* pimage, VisualObject* pVOFaces,
			 float WINSHIFT, combine_mode mode )
{
  int iErr = SMD_NO_ERR;

  // allocate our buffer
  if( m_prPixels ){
    delete m_prPixels;
    m_prPixels = NULL;
  }
  if( ( m_prPixels = new RImage<float>(pimage->columns(), pimage->rows()) ) == NULL ){
    //printf(" RImage new error\n");
    return SMD_ERR;
  }

  // Grayscale
  iErr = iGrayscale_ImageMagick( pimage );
  if( iErr ){
    //printf(" iGrayscale_ImageMagick error\n");
    return iErr;
  }

  // Write the pixels 
  iErr = iWrite_ImageMagick( pimage, m_prPixels );
  if( iErr ){
    //printf(" iWrite_ImageMagick error\n");
    return iErr;
  }
#if SMD_DO_DUMP
  vDumpPixels( m_prPixels );
#endif

  // Set size for eyefinder
  //static int lastimgWidth = 0, lastimgHeight = 0;
  if (m_prPixels->width != m_iLastImgWidth || m_prPixels->height != m_iLastImgHeight) {
    m_pCEyeFinder->resetStream(m_prPixels->width, m_prPixels->height);
    m_iLastImgWidth = m_prPixels->width;
    m_iLastImgHeight = m_prPixels->height;
  }

  // First, find faces and eyes
  m_pCEyeFinder->findEyes(*m_prPixels, *pVOFaces, WINSHIFT, mode);
  if( !pVOFaces->size() ){
#if SMD_DO_PRINT
    printf(" No Facer\n");
#endif
    return iErr;
  }
#if SMD_DO_DUMP
  //vDumpPixels( m_prPixels );
#endif

  //get_eyes(*m_prPixels, *pVOFaces);
  iErr = iGetSmiles(pimage, *m_prPixels, *pVOFaces);
  if( iErr ){
    //printf(" iGetSmiles error\n");
    return iErr;
  }

  return iErr;
}
#endif // SMD_USE_IMAGEMAGICK

//
// GetSmiles
//
int MPSmile::iGetSmiles( const RImage<float> &pixels, VisualObject &faces )
{
  int iErr = SMD_NO_ERR;
  list<VisualObject *>::iterator it = faces.begin();
  FaceBoxList face;
#if SMD_DO_TIME
  struct timeval tv1, tv2;
#endif

  for (; it != faces.end(); ++it) {
    FaceObject * f = static_cast<FaceObject*>(*it);
    if (f->eyes.rightEye && f->eyes.leftEye){
      // Get FaceBox from eye position
#if SMD_DO_PRINT
      //printf(" iGetSmiles eye[%f %f %f %f]\n", 
      //     f->eyes.xLeft, f->eyes.yLeft, f->eyes.xRight, f->eyes.yRight);
#endif
#if SMD_DO_TIME
      gettimeofday(&tv1,0);
#endif
      iErr = m_pfCIntegInter->iGetFaceFromEye( pixels,
					       f->eyes.xLeft, f->eyes.yLeft, 
					       f->eyes.xRight, f->eyes.yRight );
      if( iErr ){
	//printf(" iGetFaceFromEye error\n");
	return SMD_ERR;
      }
#if SMD_DO_TIME
      gettimeofday(&tv2,0);
      printf(" iGetFaceFromEye took %g seconds.\n", dSMDdifftv(tv2, tv1) );
#endif

      // Get interporated image
#if SMD_DO_TIME
      gettimeofday(&tv1,0);
#endif
      RIntegral<float> * pfIntegral = m_pCEyeFinder->getIntegralPtr();
      iErr = m_pfCIntegInter->iIntegralInterpolate( pixels, *m_prFacePixels,
						    pfIntegral, SMD_FACE_SIZE, SMD_FACE_SIZE );
      if( iErr ){
	return SMD_ERR;
      }
#if SMD_DO_TIME
      gettimeofday(&tv2,0);
      printf(" iIntegralInterpolate took %g seconds.\n", dSMDdifftv(tv2, tv1) );
#endif
#if SMD_DO_DUMP_FACE
      vDumpFace();
#endif
      double activation;
      // search smile
      iSearchSmile(*m_prFacePixels, face, 0, 1.0f, 0, &activation, 0);
      f->activation = activation;
    }
    else{
      f->activation = 0;
    }
    m_iCount++; // counter
  }

  return iErr;
}

//
// Search Smile
//
// pixels -- 			the image to be scanned
// faces  -- 			structure to store the detected objects (faces)
// flags  -- 			an array with a value corresponding to each window at all scales
// output_values -- 		Stick the output of the classifier up to this point here
// box -- 			flag to return the found face boxes.  Set to 0 to get activation only.
// Note: flags and output_values are being used during learning, only used from Matlab right now
int MPSmile::iSearchSmile( const RImage<float> &pixels, FaceBoxList &face, int USE_BLOCK_FLAGS,
			   float WINSHIFT, double* index_flags,
			   double* output_values, int box )
{
  int iRet = 0;

  // search smile, return numWindows
#if SMD_DO_TIME
  struct timeval tv1, tv2;
  gettimeofday(&tv1,0);
#endif
  //f->activation = 0;
  iRet = search( pixels, face, USE_BLOCK_FLAGS, WINSHIFT, index_flags, output_values, box );
#if SMD_DO_TIME
  gettimeofday(&tv2,0);
  printf(" search took %g seconds.\n", dSMDdifftv(tv2, tv1) );
#endif
#if SMD_DO_PRINT
  printf(" activation[%f]\n", *output_values);
#endif
#if SMD_DO_LOG
  FILE *fp = NULL;
  if( ( fp = fopen("smilelog","a") ) == NULL ){
    printf(" SMD_DO_LOG fopen error\n");
  }
  else{
#if SMD_DO_PRINT
    fprintf(fp, " activation[%f]\n", *output_values);
#endif
  }
  fclose(fp);
#endif
  // minus thresh to adjust smile is positive, nonsmile is negative.
  // now data.numcascades == 1, so threshold is only one.
  *output_values -= data.cascades[ data.numcascades-1 ].thresh; 

  return iRet;
}

#ifdef SMD_USE_IMAGEMAGICK	
//
// Get Smiles Using ImageMagick
//
int MPSmile::iGetSmiles( Image* pimage, const RImage<float> &pixels, VisualObject &faces ){
  int iErr = SMD_NO_ERR;
  list<VisualObject *>::iterator it = faces.begin();
  FaceBoxList face;

  for (; it != faces.end(); ++it) {
    FaceObject * f = static_cast<FaceObject*>(*it);
    Image tmpimage = *pimage;

    if (f->eyes.rightEye && f->eyes.leftEye){
      /// Get Face Box Using Eye Position
      // GENKI/626.tiff, eyePosition
#if 0
      f->eyes.xLeft = 149;
      f->eyes.yLeft = 86; 
      f->eyes.xRight = 113;
      f->eyes.yRight = 88;
#endif
      iErr = iGetFaceFromEye( &tmpimage, 
			      f->eyes.xLeft, f->eyes.yLeft, 
			      f->eyes.xRight, f->eyes.yRight );
      if( iErr ){
	return iErr;
      }

      // Write the pixels 
      iErr = iWrite_ImageMagick( &tmpimage, m_prFacePixels );
      //iErr = iWrite_ImageMagick( pimage, m_prFacePixels );
      if( iErr ){
	//printf(" iWrite_ImageMagick error\n");
	return iErr;
      }
#if SMD_DO_DUMP_FACE
      vDumpFace();
#endif

      // search smile, return numWindows
      double activation;
      search(*m_prFacePixels, face, 0, 1.0f, 0, &activation, 0);
      // minus thresh to adjust smile is positive, nonsmile is negative.
      // now data.numcascades == 1, so threshold is only one.
      f->activation = activation - data.cascades[ data.numcascades-1 ].thresh;
    }
    else{
      f->activation = 0.0f;
    }

    m_iCount++; // counter
  }

  return iErr;
}

//
// Adjust FaceBox by using Eye Position	
//
int MPSmile::iGetFaceFromEye( Image* pimage, 
			      float fxleye, float fyleye, 
			      float fxreye, float fyreye )
{
  int iErr = SMD_NO_ERR;

#if SMD_DO_PRINT
  printf(" --- iGetFaceFromEye ---\n");
  printf(" eye[ %f %f %f %f ]\n", fxleye, fyleye, fxreye, fyreye);
#endif

  float fxEyeLenRate = SMD_EYE_LEN_RATE;
  //float fxEyeLeftRate = SMD_EYE_LEFT_RATE;
  //float fxEyeRightRate = SMD_EYE_RIGHT_RATE;
  float fyEyeTopRate = SMD_EYE_TOP_RATE;
  float fxEyeLen = sqrt( (double)( fxleye - fxreye ) * (double)( fxleye - fxreye ) 
			 + (double)( fyleye - fyreye ) *  (double)( fyleye - fyreye ) );
  float fxEyeAvg = ( fxleye + fxreye ) / 2;  
  float fyEyeAvg = ( fyleye + fyreye ) / 2;
  
  /// Step1: Degree
  double dDegree = atan2( (double)( fyleye - fyreye ),  (double)( fxleye - fxreye ) );
  
  /// Step2: BigBox
  int iBigFaceSizeHalf = (int)( fxEyeLen / fxEyeLenRate * 1.0 ); // FaceSize
  if( fxEyeAvg - iBigFaceSizeHalf < 0 ){ // Adjust FaceSize
    iBigFaceSizeHalf = (int)fxEyeAvg;
  }
  if( fyEyeAvg - iBigFaceSizeHalf < 0 ){ // Adjust FaceSize
    iBigFaceSizeHalf = (int)fyEyeAvg;
  }
  pimage->crop( Geometry( iBigFaceSizeHalf * 2, iBigFaceSizeHalf * 2, 
			  (int)(fxEyeAvg - iBigFaceSizeHalf), 
			  (int)(fyEyeAvg - iBigFaceSizeHalf) ) );
#if SMD_DO_DISPLAY
  printf(" BigBox\n");
  pimage->display();
#endif
#if SMD_DO_DUMP
  pimage->write( "eyeadj_bigbox.tiff" );
#endif	
  
  /// Step3: Rotate
  //double drotate = dDegree / 2 / SMD_PI * SMD_360_DEGREE;
  pimage->rotate( -dDegree / 2 / SMD_PI * SMD_360_DEGREE );
#if SMD_DO_DISPLAY
  printf(" Rotate %g degree\n", -dDegree / 2 / SMD_PI * SMD_360_DEGREE);
  pimage->display();
#endif
#if SMD_DO_DUMP
  pimage->write( "eyeadj_rotate.tiff" );
#endif
	
  /// Step4: Crop Face Box
  int iFaceSize = (int)( fxEyeLen / fxEyeLenRate );	
  int iOffsetX = ( pimage->columns() - iFaceSize ) / 2;
  int iOffsetY = (int)( ( pimage->rows() / 2 ) - fxEyeLen * fyEyeTopRate / fxEyeLenRate );
  pimage->crop( Geometry( iFaceSize, iFaceSize, iOffsetX, iOffsetY ) );
#if SMD_DO_DISPLAY
  printf(" Crop\n");
  pimage->display();
#endif
#if SMD_DO_DUMP
  pimage->write( "eyeadj_crop.tiff" );
#endif
	
  /// Step5: Resize for Face Image
  iErr = iResize_ImageMagick( pimage, SMD_FACE_SIZE, SMD_FACE_SIZE );
  if( iErr ){
    return iErr;
  }
#if SMD_DO_DISPLAY
  printf(" Resize\n");
  pimage->display();
#endif
#if SMD_DO_DUMP	
  pimage->write( "eyeadj_face.tiff" );	
#endif
#if SMD_DO_PRINT
  printf(" -----------------------\n");
#endif
	
  return iErr;
}

// ImageMagick Grayscale
int MPSmile::iGrayscale_ImageMagick( Image* pimage )
{
#if 1
  //pimage->quantizeColorSpace( GRAYColorspace );
  pimage->quantizeColors( 256 );
  pimage->quantize( );
#else
  pimage->type( GrayscaleType );
#endif

  return SMD_NO_ERR;
}

// ImageMagick Crop
int MPSmile::iCrop_ImageMagick( Image* pimage, 
				int iWidth, int iHeight, 
				int iXOffset, int iYOffset )
{
  pimage->crop( Geometry( iWidth, iHeight, iXOffset, iYOffset ) );
  return SMD_NO_ERR;	
}
	
// ImageMagick Resize
int MPSmile::iResize_ImageMagick( Image* pimage, int iWidth, int iHeight )
{
#if 0
  pimage->sample( Geometry( iWidth, iHeight, 0, 0 ) );
#else
  pimage->scale( Geometry( iWidth, iHeight, 0, 0 ) );
#endif

  return SMD_NO_ERR;
}
	
// ImageMagick Rotate
int MPSmile::iRotate_ImageMagick( Image* pimage, double dAngle )
{
  pimage->rotate( dAngle );
	
  return SMD_NO_ERR;
}

// ImageMagick Write Face Image to Buffer
int MPSmile::iWrite_ImageMagick( Image* pimage, const RImage<float>* prPixels )
{
  pimage->write( 0, 0, prPixels->width, prPixels->height, "I", FloatPixel, 
		 prPixels->array );

  return SMD_NO_ERR;
}
#endif // SMD_USE_IMAGEMAGICK

#if SMD_DO_DUMP_FACE
void MPSmile::vDumpFace( void )
{
#if 0
  FILE* fdump = NULL;
  if( ( fdump = fopen("dumpface","wb") ) == NULL ){
    printf(" vDumpFace fopen error\n");
    return;
  }

  printf(" vDumpFace pixels: width[%d] height[%d]\n", 
	 m_prFacePixels->width, m_prFacePixels->height );
  fwrite( m_prFacePixels->array, m_prFacePixels->width * m_prFacePixels->height, 
	  sizeof(float), fdump );
  
  fclose( fdump );
#else
  int iWidth = m_prFacePixels->width;
  int iHeight = m_prFacePixels->height;
  float fSrc;
  float fMax = 0;
  char acFileName[2048];

  ///--- If the picture is not normalized, Do normalize here.
  RImage<float> rtmpimg(iWidth,iHeight);
  for(int y = 0; y < iHeight; y++){
    for(int x = 0; x < iWidth; x++){
      fSrc = m_prFacePixels->getPixel(y*iWidth+x);
      fMax = SMD_MAX(fMax,fSrc);
      rtmpimg.setPixel( x,y, fSrc/2550 );
      //printf("%f ",fSrc);
    }
  }
  //printf("\n Max %f\n", fMax);

  ///--- Output face image
  try { 
    if( fMax > 2 ){
      ///--- Make filename from time and counter.
      time_t timer;
      struct tm *ptm;
      time(&timer);
      ptm = localtime(&timer);
      sprintf(acFileName, "faceimg/face_%04d%02d%02d%02d%02d%02d%02d.tiff",
	      ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, 
	      ptm->tm_hour, ptm->tm_min, ptm->tm_sec, m_iCount%10);

      // Use Normalized image
      Image imagew(iWidth, iHeight, "I", FloatPixel, rtmpimg.array );
#if SMD_DO_PRINT
      printf(" Output face: %s\n", acFileName);
#endif
      imagew.write(acFileName);

#if SMD_DO_LOG
      FILE *fp = NULL;
      if( ( fp = fopen("smilelog","a") ) == NULL ){
	printf(" SMD_DO_LOG fopen error\n");
      }
      else{
#if SMD_DO_PRINT
	fprintf(fp, " Output face: %s\n", acFileName);
#endif
      }
      fclose(fp);
#endif
    }
    else{
      ///--- Make filename from counter.
      sprintf(acFileName,"dumpface%d.tiff",m_iCount);

      // image is normalized.
      Image imagew(iWidth, iHeight, "I", FloatPixel, m_prFacePixels->array );
      printf(" Output face: %s\n", acFileName);
      imagew.write(acFileName);
    }
  }catch ( Exception &error ) {
    cerr << " Error writing file: " 
	 <<  acFileName
	 << ". ImageMagick said: " 
	 << error.what() 
	 << endl;
  }
#endif
}
#endif // SMD_DO_DUMP_FACE

#if SMD_DO_DUMP	
void MPSmile::vDumpPixels( const RImage<float>* prPixels )
{
#if 0
  FILE* fdump = NULL;
  if( ( fdump = fopen("dumppixels","wb") ) == NULL ){
    printf(" vDumpPixels fopen error\n");
    return;
  }
  
  printf(" vDumpPixels pixels: width[%d] height[%d]\n", 
	 prPixels->width, prPixels->height );
  fwrite( prPixels->array, prPixels->width * prPixels->height, sizeof(float), fdump );
  
  fclose( fdump );
#elif 0
  Image imagew( prPixels->width, prPixels->height, "I", FloatPixel, prPixels->array );
  imagew.write("dumppixels.tiff");
#else
  int iWidth = prPixels->width;
  int iHeight = prPixels->height;
  float fSrc;
  float fMax = 0;
  char acFileName[2048];

  ///--- If the picture is not normalized, Do normalize here.
  RImage<float> rtmpimg(iWidth,iHeight);
  for(int y = 0; y < iHeight; y++){
    for(int x = 0; x < iWidth; x++){
      fSrc = prPixels->getPixel(y*iWidth+x);
      fMax = SMD_MAX(fMax,fSrc);
      rtmpimg.setPixel( x,y, fSrc/2550 );
      //printf("%f ",fSrc);
    }
  }
  //printf("\n Max %f\n", fMax);

  ///--- Output face image
  try { 
    if( fMax > 2 ){
      ///--- Make filename from time and counter.
      time_t timer;
      struct tm *ptm;
      time(&timer);
      ptm = localtime(&timer);
      sprintf(acFileName, "facedetect/facedetect_%04d%02d%02d%02d%02d%02d%02d.tiff",
	      ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, 
	      ptm->tm_hour, ptm->tm_min, ptm->tm_sec, m_iCount%10);

      // Use Normalized image
      Image imagew(iWidth, iHeight, "I", FloatPixel, rtmpimg.array );
#if SMD_DO_PRINT
      printf(" Output face: %s\n", acFileName);
#endif
      imagew.write(acFileName);

#if SMD_DO_PRINT
	fprintf(fp, " Output face: %s\n", acFileName);
#endif

    }
    else{
      ///--- Make filename from counter.
      sprintf(acFileName,"dumppixels%d.tiff",m_iCount);

      // image is normalized.
      Image imagew(iWidth, iHeight, "I", FloatPixel, prPixels->array );
      printf(" Output face: %s\n", acFileName);
      imagew.write(acFileName);
    }
  }catch ( Exception &error ) {
    cerr << " Error writing file: " 
	 <<  acFileName
	 << ". ImageMagick said: " 
	 << error.what() 
	 << endl;
  }
#endif
}
#endif // SMD_DO_DUMP


void MPSmile::initStream(const int width, const int height, double WINSHIFT){
  if(m_pCEyeFinder)
    m_pCEyeFinder->initStream(width, height, WINSHIFT);
}

void MPSmile::resetStream(const int width, const int height, double WINSHIFT){
  if(m_pCEyeFinder)
    m_pCEyeFinder->resetStream(width, height, WINSHIFT);
}

void MPSmile::releaseStream(){
  if(m_pCEyeFinder)
    m_pCEyeFinder->releaseStream();
}

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

