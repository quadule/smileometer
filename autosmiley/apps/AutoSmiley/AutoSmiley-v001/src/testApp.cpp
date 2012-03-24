#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(24);
	ofBackground(30,30,30);

    vision.setupCamera(0, 320, 240);
	
	//lets load the font from our app bundle
	ofSetDataPathRoot("./");
	ttf.loadFont("MONACO.TTF", 48);
	ttfSmall.loadFont("MONACO.TTF", 14);

	//lets restore it
	ofSetDataPathRoot("../../../");

	img.allocate(320, 240, OF_IMAGE_GRAYSCALE);
	
	lastTime = ofGetElapsedTimef()- 10.0;
    lastFlushTime = lastTime;
		
	smilePct = 0.0;
	smoothPct = 0.0;
    printf("========INITIALIZED========\n");
}

//--------------------------------------------------------------
void testApp::update(){
    vision.update();
	
	img.setFromPixels(vision.gray.getPixels(), vision.gray.width, vision.gray.height, OF_IMAGE_GRAYSCALE);
		
	ofxSmile::getSmile(img, smilePct);
	
	smoothPct *= 0.8;
	smoothPct += MAX(0, smilePct) * 0.2;
	
    float elapsed = ofGetElapsedTimef();
    
	if (elapsed - lastTime > 2.0 && smoothPct > 0.5) {
        printf(":)\n");
        fflush(stdout);
        lastTime = elapsed;
	} else if(elapsed - lastFlushTime > 1.0) {
        printf("\n");
        fflush(stdout);
        lastFlushTime = elapsed;
    }
	
}

//--------------------------------------------------------------
void testApp::draw(){

	ofSetColor(0xffffff);
    vision.color.draw(0,0);
	
	ofDrawBitmapString("smile        = " + ofToString(smilePct, 2), 20, 20);
	ofDrawBitmapString("smile smooth = " + ofToString(smoothPct, 2), 20, 36);
	
	if( ofGetElapsedTimef() - lastTime < 1.0 ){
		ofSetColor(255, 255, 255, 100);
		ttf.drawString(":)", -50 + img.width/2, 30 + img.height/2);
	}

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
    if(key == ' '){
        vision.togglePlayback();
    }

    //if you need to adjust the camera properties
    if(key == 's'){
        vision.camera.videoSettings();
    }
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(){

}
