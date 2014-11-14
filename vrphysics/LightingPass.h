//
//  LightingPass.h
//  vrphysics
//
//  Created by Robin Wu on 11/11/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__LightingPass__
#define __vrphysics__LightingPass__

#include <stdio.h>
#include <vector>
#include "ScreenPass.h"
#include "PointLight.h"
#include "LightGroup.h"

class LightingPass: public ScreenPass
{
public:
    LightingPass(osg::Camera *mainCamera, osg::TextureRectangle *position_tex, osg::TextureRectangle *diffuse_tex, osg::TextureRectangle *normal_tex, LightGroup *lightGroup);
    virtual ~LightingPass();
    
    osg::ref_ptr<osg::TextureRectangle> getLightingOutTexture()
    {
        return getOutputTexture(_out_lighting_tex_id);
    }
    
protected:
    virtual void configureStateSet();
    void configRTTCamera();
private:
    float _nearPlaneDist;
    float _farPlaneDist;
    
    int _diffuse_tex_id;
    int _normal_tex_id;
    int _position_tex_id;
    
    int _out_lighting_tex_id;
    
    LightGroup *_lightGroup;
    osg::ref_ptr<osg::Group> _lightPassGroupNode;
};

#endif /* defined(__vrphysics__LightingPass__) */
