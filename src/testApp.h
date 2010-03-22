/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "Logger.h"
//#include "ofxOpenCv.h"
//#include "ofxDirList.h"
//#include "ofxVectorMath.h"
//#include "ofxNetwork.h"
//#include "ofxOsc.h"
//#include "ofxThread.h"
#include "ofxXmlSettings.h"
//#include "ofx3DModelLoader.h"
//#include "ofxSQLiteHeaders.h"
#include "DBReader.h"

class testApp : public ofBaseApp{
	
public:
	
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void resized(int w, int h);
	
	
	ofxXmlSettings settings;
	DBReader* m_dbReader;
};

#endif
