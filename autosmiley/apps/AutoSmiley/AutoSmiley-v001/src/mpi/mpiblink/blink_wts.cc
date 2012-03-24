/*
 * blink_wts.cc					11/20 500 features
 *
 *  Copyright (c) 2002 Machine Perception Laboratory 
 *  University of California San Diego.
 * 
 * Please read the disclaimer and notes about redistribution 
 * at the end of this file.
 *
 */

#include "blink_wts.h"

void blink_wts::assignData(FeatureData &d){
	d.patchsize=23;
	d.patch_width=45;
	d.patch_height=23;
	d.plus_minus_one=0;
	d.numfeatures=500;
	d.numcascades=1;
	d.cascades = casc;
	d.features = f;
	d.normOffset.top = 0;
	d.normOffset.left = 0;
	d.normOffset.right = 0;
	d.normOffset.bottom = 0;
	d.numStdAdjusts=50;
	d.stdAdjusts = stdAdj;
	d.real_fun = true;
	d.nl=64; //// number of bins 
	d.minval = 1; /// fix for matlab vs. C 0-1 indexing bug
	d.maxval = 64;  /// fix for matlab vs. C 0-1 indexing bug
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

