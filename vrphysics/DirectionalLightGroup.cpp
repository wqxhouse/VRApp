//
//  DirectionalLightGroup.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/27/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "DirectionalLightGroup.h"
#include "ShadowGroup.h"

DirectionalLightGroup::DirectionalLightGroup(ShadowGroup *shadowGroup)
: _lightnum(0), _shadowGroup(shadowGroup)
{
    _directionalLightGroup = new osg::Group;
}

DirectionalLightGroup::~DirectionalLightGroup()
{
    for(int i = 0; i < _directionalLights.size(); i++)
    {
        delete _directionalLights[i];
    }
}

std::vector<DirectionalLight *> & DirectionalLightGroup::getDirectionalLightsReference()
{
    return _directionalLights;
}

void DirectionalLightGroup::addMultipleLights(std::vector<DirectionalLight *> lights)
{
    _lightnum += lights.size();
    for(unsigned long i = 0; i < lights.size(); i++)
    {
        _directionalLightGroup->addChild(lights[i]->_lightGeomTransform);
        _directionalLights.push_back(lights[i]);
        _directionalLightsMap.insert(std::make_pair(lights[i]->_id, lights[i]));
    }
}


DirectionalLight *DirectionalLightGroup::getDirectionalLight(int _id)
{
    std::map<int, DirectionalLight *>::iterator it;
    return ((it = _directionalLightsMap.find(_id)) != _directionalLightsMap.end()) ? it->second : NULL;
}

int DirectionalLightGroup::addLight(const osg::Vec3 &pos, const osg::Vec3 &lookAt, const osg::Vec3 &color, bool castShadow)
{
    DirectionalLight *light = new DirectionalLight();
    light->setPosition(pos);
    light->setLookAt(lookAt);
    light->setDiffuse(color.x(), color.y(), color.z());
    light->setSpecular(0, 0, 0);
    light->intensity = 0.8f;
    light->genGeomTransform(0.25);
    _directionalLightGroup->addChild(light->_lightGeomTransform);
    light->orbitAxis = osg::Vec3(0, 0, 1);
    
    _directionalLights.push_back(light);
    _directionalLightsMap.insert(std::make_pair(light->_id, light));
    _lightnum++;
    
    if (castShadow)
    {
        _shadowGroup->addDirectionalLight(light, ShadowGroup::BASIC);
    }
    
    return light->_id;
}


std::vector<int> DirectionalLightGroup::getAllLightIds()
{
    std::vector<int> res;
    res.resize(_directionalLights.size());
    for(int i = 0; i < _directionalLights.size(); i++)
    {
        res[i] = _directionalLights[i]->_id;
    }
    
    return res;
}