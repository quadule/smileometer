#pragma once

#include "smile.h"
#include "rimage.h"

class ofxSmile {
public:
	static bool getSmile(ofImage img, float& amount) {
		img.setImageType(OF_IMAGE_GRAYSCALE);
		unsigned char* cpixels = img.getPixels();
		int n = (int) (img.width * img.height);
		float* fpixels = new float[n];
		for (int i = 0; i < n; i++)
			fpixels[i] = (float) cpixels[i];

		MPSmile smileFinder;
		VisualObject faces;
		RImage<float> rimage(fpixels, img.width, img.height);
		bool smileFound = false;
		amount = 0;
		if(!smileFinder.findSmiles(rimage, faces)) {
			if (faces.size() == 1) {
				FaceObject* face = static_cast<FaceObject*>(*(faces.begin()));
				amount = face->activation;
				smileFound = true;
			}
		}

		delete [] fpixels;

		return smileFound;
	}
};
