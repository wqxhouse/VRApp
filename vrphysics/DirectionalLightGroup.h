//
//  DirectionalLightGroup.h
//  vrphysics
//
//  Created by Robin Wu on 11/27/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__DirectionalLightGroup__
#define __vrphysics__DirectionalLightGroup__

#include <stdio.h>
#include <vector>
#include <osg/Group>
#include "DirectionalLight.h"

class DirectionalLightGroup
{
public:
    DirectionalLightGroup();
    ~DirectionalLightGroup();
    
    void addLight(const osg::Vec3 &pos, const osg::Vec3 &lookAt, const osg::Vec3 &color);
    std::vector<DirectionalLight *> &getDirectionalLightsReference();
    
    void addMultipleLights(const std::vector<DirectionalLight *> &lights);
private:
    osg::ref_ptr<osg::Group> _directionalLightGroup;
    std::vector<DirectionalLight *> _directionalLights;
    int _lightnum;
};

#endif /* defined(__vrphysics__DirectionalLightGroup__) */
