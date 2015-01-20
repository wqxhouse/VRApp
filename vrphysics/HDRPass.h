//
//  HDRPass.h
//  vrphysics
//
//  Created by Robin Wu on 12/11/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__HDRPass__
#define __vrphysics__HDRPass__

#include <stdio.h>
#include <osg/Camera>

#include "ScreenPass.h"
class HDRPass : public ScreenPass
{
public:
    HDRPass(osg::Camera *mainCamera, osg::TextureRectangle *inScreenTex);
    virtual ~HDRPass();
   
    // override
    virtual int addOutTexture();
    
protected:
    // overriden
    virtual void setupCamera();
    void configRTTCamera();
    virtual void configureStateSet();
   
private:
    int _in_screenTex;
    int _out_screenTex;
    int _hdr_shader;
    osg::ref_ptr<osg::Group> _screenQuad;
};

#endif /* defined(__vrphysics__HDRPass__) */
