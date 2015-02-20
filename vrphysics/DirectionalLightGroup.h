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

class ShadowGroup;
class DirectionalLightGroup
{
public:
    DirectionalLightGroup(ShadowGroup *shadowGroup);
    ~DirectionalLightGroup();
    
    int addLight(const osg::Vec3 &pos, const osg::Vec3 &lookAt, const osg::Vec3 &color, bool castShadow);
    std::vector<DirectionalLight *> &getDirectionalLightsReference();
    DirectionalLight *getDirectionalLight(int _id);
    void addMultipleLights(std::vector<DirectionalLight *> lights);
    
    std::vector<int> getAllLightIds();
    
private:
    
    int _lightnum;
    osg::ref_ptr<osg::Group> _directionalLightGroup;
    std::vector<DirectionalLight *> _directionalLights;
    std::map<int, DirectionalLight *> _directionalLightsMap;
    
    ShadowGroup *_shadowGroup;
};

#endif /* defined(__vrphysics__DirectionalLightGroup__) */
