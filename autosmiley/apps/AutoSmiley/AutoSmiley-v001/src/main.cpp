#include "ofMain.h"
#include "testApp.h"


//========================================================================
int main( ){
    ofSetLogLevel(OF_LOG_SILENT);
	ofSetupOpenGL(320,240, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:

	ofRunApp(new testApp());

}
