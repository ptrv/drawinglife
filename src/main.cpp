/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeIncludes.h"
#include "DrawingLifeApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){
    int w;
    int h;
    ofxXmlSettings settings;
    settings.loadFile("AppSettings.xml");
    if(settings.getValue("settings:window:mode", "normal").compare("fullscreen") == 0)
    {
        w = settings.getValue("settings:window:size1:width", 1024);
        h = settings.getValue("settings:window:size1:height", 768);
    }
    else
    {
        w = settings.getValue("settings:window:size2:width", 1024);
        h = settings.getValue("settings:window:size2:height", 768);
    }

    ofAppGlutWindow window;
//    ofSetupOpenGL(&window, 1680,1000, OF_GAME_MODE);
	ofSetupOpenGL(&window, w, h, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new DrawingLifeApp(settings));
}
