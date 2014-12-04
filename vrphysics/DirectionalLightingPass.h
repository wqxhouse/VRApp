//
//  DirectionalLightingPass.h
//  vrphysics
//
//  Created by Robin Wu on 11/27/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__DirectionalLightingPass__
#define __vrphysics__DirectionalLightingPass__

#include <stdio.h>
#include <osg/Camera>
#include <osg/TextureRectangle>
#include <osg/Group>

#include "ScreenPass.h"
#include "DirectionalLightGroup.h"

class DirectionalLightingPass : public ScreenPass
{
public:
    DirectionalLightingPass(osg::Camera *mainCamera, osg::TextureRectangle *position_tex, osg::TextureRectangle *diffuse_tex, osg::TextureRectangle *normal_tex, DirectionalLightGroup *lightGroup);
    virtual ~DirectionalLightingPass();
    
    osg::ref_ptr<osg::TextureRectangle> getLightingOutTexture()
    {
        return getOutputTexture(_out_lighting_tex_id);
    }
    
    // overriding
    virtual int addOutTexture();
    
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
    
    DirectionalLightGroup *_lightGroup;
    osg::ref_ptr<osg::Group> _lightPassGroupNode;
    
    int _light_shader_id;
};

#endif /* defined(__vrphysics__DirectionalLightingPass__) */
