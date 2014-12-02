//
//  DirectionalLightGroup.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/27/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "DirectionalLightGroup.h"

DirectionalLightGroup::DirectionalLightGroup()
: _lightnum(0)
{
    _directionalLightGroup = new osg::Group;
}

DirectionalLightGroup::~DirectionalLightGroup()
{
}

std::vector<DirectionalLight *> & DirectionalLightGroup::getDirectionalLightsReference()
{
    return _directionalLights;
}

void DirectionalLightGroup::addMultipleLights(const std::vector<DirectionalLight *> &lights)
{
    _lightnum += lights.size();
    for(unsigned long i = 0; i < lights.size(); i++)
    {
        _directionalLightGroup->addChild(lights[i]->_lightGeomTransform);
        _directionalLights.push_back(lights[i]);
    }
}

void DirectionalLightGroup::addLight(const osg::Vec3 &pos, const osg::Vec3 &lookAt, const osg::Vec3 &color)
{
    DirectionalLight *light = new DirectionalLight();
    light->setPosition(pos);
    light->setLookAt(lookAt);
    light->setDiffuse(color.x(), color.y(), color.z());
    light->setSpecular(color.x(), color.y(), color.z());
    light->intensity = 0.8f;
    light->genGeomTransform(0.25);
    _directionalLightGroup->addChild(light->_lightGeomTransform);
    light->orbitAxis = osg::Vec3(0, 0, 1);
    
    _directionalLights.push_back(light);
    _lightnum++;
}