/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeIncludes.h"
#include "DrawingLifeApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){

    int w = 1024;
    int h = 768;

    ofAppGlutWindow window;
//    ofSetupOpenGL(&window, 1680,1000, OF_GAME_MODE);
	ofSetupOpenGL(&window, w, h, OF_WINDOW);			// <-------- setup the GL context

	ofRunApp( new DrawingLifeApp());
}
