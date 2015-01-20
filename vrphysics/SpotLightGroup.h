//
//  SpotLightGroup.h
//  vrphysics
//
//  Created by Robin Wu on 1/7/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#ifndef __vrphysics__SpotLightGroup__
#define __vrphysics__SpotLightGroup__

#include <stdio.h>
#include <stdio.h>
#include <vector>
#include <osg/Group>
#include "SpotLight.h"

class SpotLightGroup
{
public:
    SpotLightGroup();
    ~SpotLightGroup();
    
    void addLight(const osg::Vec3 &pos, const osg::Vec3 &lookAt, const osg::Vec3 &color);
    std::vector<SpotLight *> &getDirectionalLightsReference();
    
    void addMultipleLights(const std::vector<SpotLight *> &lights);
private:
    osg::ref_ptr<osg::Group> _spotLightGroup;
    std::vector<SpotLight *> _spotLights;
    int _lightnum;
};

#endif /* defined(__vrphysics__SpotLightGroup__) */
