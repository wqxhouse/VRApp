//
//  FinalPass.h
//  vrphysics
//
//  Created by Robin Wu on 11/13/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__FinalPass__
#define __vrphysics__FinalPass__

#include <stdio.h>
#include <osg/Camera>

#include "ScreenPass.h"

class FinalPass : public ScreenPass
{
public:
    FinalPass(osg::Camera *mainCamera, osg::TextureRectangle *albedoTexture, osg::TextureRectangle *lightTexture);
    virtual ~FinalPass();
    
    osg::ref_ptr<osg::TextureRectangle> getFinalPassTexture()
    {
        return getOutputTexture(_out_finalpass_tex_id);
    }
    
protected:
    virtual void configureStateSet();

    // overriden
    virtual void setupCamera();
    void configRTTCamera();
    
    int _albedo_tex_id;
    int _light_tex_id;
    
    int _out_finalpass_tex_id;
};

#endif /* defined(__vrphysics__FinalPass__) */