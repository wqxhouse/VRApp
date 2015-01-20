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
    FinalPass(osg::Camera *mainCamera, osg::TextureRectangle *albedoTexture, osg::TextureRectangle *dirLightTexture, osg::TextureRectangle *lightTexture, osg::TextureRectangle *ssaoTexture);
    virtual ~FinalPass();
    
    osg::ref_ptr<osg::TextureRectangle> getFinalPassTexture()
    {
        return getOutputTexture(_out_finalpass_tex_id);
    }
    
    // override
    virtual int addOutTexture();
    
protected:
    virtual void configureStateSet();

    // overriden
    virtual void setupCamera();
    void configRTTCamera();
    
    int _albedo_tex_id;
    int _dirLight_tex_id;
    int _light_tex_id; // point light
    int _ssao_tex_id;
    
    int _out_finalpass_tex_id;
    int _final_shader_id;
    
    osg::ref_ptr<osg::Group> _texQuad;
};

#endif /* defined(__vrphysics__FinalPass__) */
