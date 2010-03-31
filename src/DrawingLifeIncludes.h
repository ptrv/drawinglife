/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DRAWINGLIFEINCLUDES_H_
#define _DRAWINGLIFEINCLUDES_H_

#define SAFE_DELETE(p) {if(p) {delete (p); (p)=NULL;}}
#define SAFE_DELETE_ARRAY(p) {if(p) {delete[] (p); (p)=NULL;}}

#endif // _DRAWINGLIFEINCLUDES_H_
