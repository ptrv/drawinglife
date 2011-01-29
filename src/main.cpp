/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeIncludes.h"
#include "DrawingLifeApp.h"
#include "ofAppGlutWindow.h"

#ifndef TARGET_OSX
#include <tclap/CmdLine.h>
#endif

//========================================================================
int main(int argc, const char* argv[] ){

    std::cout << "DrawingLifeApp, avp::ptr, 2010" << std::endl;
    std::cout << "The drawing of my life, plan b\n" << std::endl;
#ifndef TARGET_OSX
    try {

        TCLAP::CmdLine cmd("DrawingLifeApp, avp::ptr, 2010\nThe drawing of my life, plan b", '=', APP_VERSION_STR);

        TCLAP::ValueArg<int> widthArg("","width","Application window width (default: 1024)",false, 1024, "width");
        TCLAP::ValueArg<int> heightArg("","height","Application window height (default: 768)",false, 768, "height");
        TCLAP::ValueArg<std::string> settingsArg("c", "config", "Configuration file name (default: AppSettings.xml)", false, "AppSettings.xml", "file-name");

        cmd.add( heightArg );
        cmd.add( widthArg );
        cmd.add( settingsArg );


        // Parse the argv array.
        cmd.parse( argc, argv );

        int width = widthArg.getValue();
        int height = heightArg.getValue();
        std::string settingsFile = settingsArg.getValue();
        ofAppGlutWindow window;
    //    ofSetupOpenGL(&window, 1680,1000, OF_GAME_MODE);
        ofSetupOpenGL(&window, width, height, OF_WINDOW);			// <-------- setup the GL context

        ofRunApp( new DrawingLifeApp(settingsFile));

	} catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
#else
	ofAppGlutWindow window;
    //    ofSetupOpenGL(&window, 1680,1000, OF_GAME_MODE);
	ofSetupOpenGL(&window, 1024, 768, OF_WINDOW);			// <-------- setup the GL context
	
	ofRunApp( new DrawingLifeApp("AppSettings.xml"));
	
#endif

}
	