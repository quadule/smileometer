#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxVectorMath.h"

class computerVision{

	public:

        //----------
        void loadVideo(string path){
            video.loadMovie(path);
            video.play();

            color.allocate(video.width, video.height);
            gray.allocate(video.width, video.height);

            bPauseVideo = false;
            frameCount  = 0;
            bUseCamera  = false;
        }

        //------------
        void setupCamera(int which, int width, int height){
            camera.setDeviceID(which);
            camera.initGrabber(width, height);

            color.allocate(camera.width, camera.height);
            gray.allocate(camera.width, camera.height);

            bPauseVideo = false;
            frameCount  = 0;
            bUseCamera  = true;
        }

        //-----------
        void update(){

            if(bPauseVideo ) return;

            if(bUseCamera){
				camera.grabFrame();

				if(camera.isFrameNew()){
					color.setFromPixels(camera.getPixels(), camera.width, camera.height);
				}
				
            }else{

                video.idleMovie();
                if(video.isFrameNew()){
                     frameCount++;
                }else{
                    return;
                }

                color.setFromPixels(video.getPixels(), video.width, video.height);
            }

            //convert the color image to grayscale
            gray = color;
			
			//reduce the contrast
			gray.convertToRange(30, 230);
        }

        //-----------
        void draw(float x, float y){
            ofSetColor(0xFFFFFF);
            gray.draw(x, y);
        }


        //-----------
        void togglePlayback(){

           bPauseVideo = !bPauseVideo;
           if(bPauseVideo){
                video.setPaused(true);
           }else{
                video.setPaused(false);
           }
        }

        ofxCvGrayscaleImage gray;
        ofxCvColorImage color;

        ofVideoPlayer video;
        ofVideoGrabber camera;

        float threshAmnt, fadeAmnt;
        bool bPauseVideo;
        bool bUseCamera;
        int frameCount;

};


