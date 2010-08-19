/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeIncludes.h"
#include "DrawingLifeApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main(int argc, const char* argv[] ){

    int w;
    int h;

    if(argc > 1)
    {
        if(argc == 3)
        {
            w = atoi(argv[1]);
            h = atoi(argv[2]);
        }
        else
        {
            std::cout << "Error: wrong command line arguments!\n" << std::endl;
            return 1;
        }
    }
    else
    {
        w = 1440;
        h = 900;
    }

    ofAppGlutWindow window;
//    ofSetupOpenGL(&window, 1680,1000, OF_GAME_MODE);
	ofSetupOpenGL(&window, w, h, OF_WINDOW);			// <-------- setup the GL context

	ofRunApp( new DrawingLifeApp());
}
