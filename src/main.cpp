/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeIncludes.h"
#include "DrawingLifeApp.h"

#ifndef TARGET_OSX
#include <tclap/CmdLine.h>
#endif

//========================================================================
int main(int argc, const char* argv[] ){

    string appStr(string(APP_NAME_STR) + string(", ") + string(APP_AUTHOR_STR)
                  + string(", ") + string(APP_DATE));
    std::cout << appStr << std::endl;
    std::cout << "The drawing of my life, plan b\n" << std::endl;
#ifndef TARGET_OSX
    try
    {
        TCLAP::CmdLine cmd(appStr, '=', APP_VERSION_STR);

        TCLAP::ValueArg<int> widthArg(
            "", "width", "Application window width (default: 1024)",
            false, 1024, "width");
        TCLAP::ValueArg<int> heightArg(
            "", "height", "Application window height (default: 768)",
            false, 768, "height");
        TCLAP::ValueArg<std::string> settingsArg(
            "c", "config", "Configuration file name (default: AppSettings.xml)",
            false, "AppSettings.xml", "file-name");

        cmd.add(heightArg);
        cmd.add(widthArg);
        cmd.add(settingsArg);

        cmd.parse(argc, argv);

        int width = widthArg.getValue();
        int height = heightArg.getValue();
        std::string settingsFile = settingsArg.getValue();

        ofSetupOpenGL(width, height, OF_WINDOW);
        ofRunApp( new DrawingLifeApp(settingsFile));
    }
    catch (TCLAP::ArgException &e)  // catch any exceptions
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
#else
    ofSetupOpenGL(1024, 768, OF_WINDOW);  // <-------- setup the GL context
    ofRunApp( new DrawingLifeApp("AppSettings.xml"));
#endif
}
