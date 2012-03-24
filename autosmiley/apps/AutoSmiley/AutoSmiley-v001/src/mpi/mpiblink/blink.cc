/*
 *  blink.cc
 *
 *  Author:Ian Fasel, Bret Fortenberry
 *  Fixes:
 *
 *  Copyright (c) 2003 Machine Perception Laboratory
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

#include <iostream>
#include <string.h>
#include <math.h>
#include <list>
#include <stdio.h>
#include "blink.h"
#include "blink_wts.h"

#define eye_width 44
#define eye_height 22
#define cushion 3 //border around interpolated image to shrink
#define eye_preWidth 44
#define eye_preHeight 22


MPBlink::MPBlink ( int use_ada ) : MPISearchFaceDetector(),
     eyefinder( use_ada ),
     eyes(eye_preWidth, eye_preHeight){
  blink_wts::assignData(data);
  activation = 0;
  bVal.openTotal = 0;
  bVal.blinkTotal = 0;
  imgCount = 0;
}

MPBlink::~MPBlink ( ){

}

double MPBlink::findBlinks(const RImage<float> &pixels, VisualObject &faces,
                           float WINSHIFT, combine_mode mode){
  // First, find the faces
  static int lastimgWidth = 0, lastimgHeight = 0;
  if (pixels.width != lastimgWidth || pixels.height != lastimgHeight) {
    eyefinder.resetStream(pixels.width, pixels.height);
    lastimgWidth = pixels.width;
    lastimgHeight = pixels.height;
  }

  eyefinder.findEyes(pixels, faces, WINSHIFT, mode);
  if(!faces.size())
    return 0;
  get_eyes(pixels, faces);

  return 1;
}


// Note: I think the image would be better quality if we were interpolating from the integral
// images, because it would be taking more than just neighboring pixels into account.
// However, if we try to do interpolation from the existing RIntegral in the face_detector.stream,
// it gets just as expensive to properly grab it from the Rintegral of squared pixels as to just do
// interpolation from the raw image and redo the integration.  I think. Let me know if you figure it out.

/*function to handle interpolation and rotation, if the eyes are rotated it handles how to rotate
the image to make the eyes perfectly aligned*/

#define eye_dest_width 44
#define eye_dest_height 22
#define h_off_pct .02
#define h_eye_factor 0.39f
#define v_off_pct .02
#define v_eye_factor 0.29f

void MPBlink::get_eyes(const RImage<float> &pixels, VisualObject &faces){
  list<VisualObject *>::iterator it = faces.begin();
  FaceBoxList face;
  double act[10];
  for (; it != faces.end(); ++it) {
    FaceObject * f = static_cast<FaceObject*>(*it);
    if (f->eyes.rightEye && f->eyes.leftEye){
      if( debug ){
	printf("eyes.xLeft[%f] yLeft[%f]\n", f->eyes.xLeft, f->eyes.yLeft);
	printf("eyes.xRight[%f] yRight[%f]\n", f->eyes.xRight, f->eyes.yRight);
      }
      double xd = f->eyes.xLeft - f->eyes.xRight;
      double yd = f->eyes.yLeft - f->eyes.yRight;
      float eye_dist = sqrt((xd*xd +yd*yd));
      float sin_d = -yd/xd;
      float cos_d = eye_dist/xd;

      float h_eye_zero = max(0.0,f->eyes.xRight-eye_dist*(h_eye_factor+h_off_pct));
      float v_eye_zero = max(0.0,f->eyes.yRight-eye_dist*(v_eye_factor+v_off_pct));
      float xwidth = eye_dist*(1 + 2*h_eye_factor);
      float ywidth = eye_dist*(2*v_eye_factor);

      int xedges;
      int yedges;
      float xfrac;
      float yfrac;
      float yoff[eye_dest_width];
      double xstep = xwidth/static_cast<double>(eye_dest_width-1);
      double ystep = ywidth/static_cast<double>(eye_dest_height-1);

      float j;
      int i;
      for(i = 0, j = 0; i < eye_dest_width; ++i, j+=xstep){
        yoff[i] = -sin_d * j;
      }

      float tempx, tempy;
      float *p = eyes.array;
      float f00, f01, f11, f10, fx0, fx1;
      for(int y=0; y < eye_dest_height; ++y){
        float xinit = (sin_d * ystep*y)+h_eye_zero;
        float yinit = v_eye_zero + ystep*y;
        for(int x=0; x < eye_dest_width; ++x){
          tempx = xinit + xstep*x;
          tempy = yinit + yoff[x];
          xedges = static_cast<int>(tempx);
          yedges = static_cast<int>(tempy);
          xfrac = tempx - xedges;
          yfrac = tempy - yedges;

          f00 = pixels.getPixel(xedges,yedges);
          f01 = pixels.getPixel(xedges,yedges + 1);
          f10 = pixels.getPixel(xedges + 1,yedges);
          f11 = pixels.getPixel(xedges + 1,yedges + 1);

          // xfrac and y frac are the remainders
          fx0 = f00 + xfrac*(f10-f00);
          fx1 = f01 + xfrac*(f11-f01);
          *(p++) = (fx0 + yfrac*(fx1-fx0));
	  // eyes.setPixel(x,y,(fx0 + yfrac*(fx1-fx0)));
        }
      }
      //eyes.print();
      debug = false;
      int num_blink_windows = search(eyes, face, 0, 1.0, 0, act,0);
      (*it)->activation = act[0];
      if( debug ){
       	printf("num_blink_windows: %d,  activation: %f\n",
               num_blink_windows, act[0]);
      }
      //f->activation = cluster.UpdateCluster(f->activation) - f->activation;
    }
    else
      f->activation = 0.0f;
  }
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

