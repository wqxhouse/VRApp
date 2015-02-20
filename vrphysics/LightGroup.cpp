//
//  ObjectGroup.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/11/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "LightGroup.h"
#include "LightGroupAnimationCallback.h"

LightGroup::LightGroup() : _lightNum(0)
{
    _geomTransformLightGroup = new osg::Group;
    _geomTransformLightGroup->setUpdateCallback(new LightGroupAnimationCallback(this));
}
LightGroup::~LightGroup()
{
    for(int i = 0; i < _pointLights.size(); i++)
    {
        delete _pointLights[i];
    }
}

int LightGroup::addLight(const osg::Vec3 &position, const osg::Vec3 &color, const osg::Vec3 &orbitAxis, const osg::Vec3 &attenuation, float intensity)
{
    PointLight *light = new PointLight();
    light->setPosition(position);
    light->setDiffuse(color.x(), color.y(), color.z());
    light->setSpecular(color.x(), color.y(), color.z());
    light->setAttenuation(attenuation.x(), attenuation.y(), attenuation.z()); // set constant, linear, and exponential attenuation
    light->setIntensity(intensity);
    
    _geomTransformLightGroup->addChild(light->getGeomTransformNode());
    light->setAnimOrbitAxis(orbitAxis);
    
    _pointLights.push_back(light);
    _pointLightsMap.insert(std::make_pair(light->getId(), light));
    _lightNum++;
    
    return light->getId();
}

void LightGroup::addMultipleLights(std::vector<PointLight *> lights)
{
    _lightNum += lights.size();
    for(unsigned long i = 0; i < lights.size(); i++)
    {
        _geomTransformLightGroup->addChild(lights[i]->getGeomTransformNode());
        _pointLights.push_back(lights[i]);
        _pointLightsMap.insert(std::make_pair(lights[i]->getId(), lights[i]));
    }
}

void LightGroup::addRandomAnimLightWithBoundingSphere(const osg::BoundingSphere &boundSphere)
{
    // create a random light that is positioned on bounding sphere of scene (skRadius)
    PointLight *l = new PointLight;
    
    l->setAnimated(true);
    osg::Vec3f posOnSphere(randomf(-1.0f, 1.0f), randomf(-1.0f, 1.0f), randomf(-1.0f, 1.0f));
    posOnSphere.normalize();
    
    posOnSphere *= randomf(0.95f, 1.05f);
    
    osg::Vec3f orbitAxis(randomf(0.0f, 1.0f), randomf(0.0f, 1.0f), randomf(0.0f, 1.0f));
    orbitAxis.normalize();
    l->setAnimOrbitAxis(orbitAxis);
   
    if(boundSphere.valid())
    {
        float radius = boundSphere.radius();
        posOnSphere *= radius;
    }
    else
    {
        posOnSphere *= skRadius-1;
    }
    
    l->setPosition(posOnSphere);
    l->setAmbient(0.0f, 0.0f, 0.0f);
    
    // osg::Vec3f col(randomf(0.3f, 0.5f), randomf(0.2f, 0.4f), randomf(0.7f, 1.0f));
    
    // randomize color
//    osg::Vec3 col = osg::Vec3(randomf(0.4f, 1.0f), randomf(0.1f, 1.0f), randomf(0.3f, 1.0f));
    osg::Vec3 col = osg::Vec3(randomf(0.0f, 1.0f), randomf(0.0f, 1.0f), randomf(0.0f, 1.0f));
    
    l->setDiffuse(col.x(), col.y(), col.z());
    l->setSpecular(col.x(), col.y(), col.z());
    l->setAttenuation(0.3f, 0.3f, 0.6f); // set constant, linear, and exponential attenuation
    l->setIntensity(0.8f);
    
    _geomTransformLightGroup->addChild(l->getLightSphereTransformNode());
    _pointLights.push_back(l);
    _pointLightsMap.insert(std::make_pair(l->getId(), l));
    _lightNum++;
}

void LightGroup::addRandomAnimLight()
{
    osg::BoundingSphere sp;
    sp.radius() = -1.0f; // invalid bsphere
    addRandomAnimLightWithBoundingSphere(sp);
}

osg::ref_ptr<osg::Group> LightGroup::getGeomTransformLightGroup()
{
    return _geomTransformLightGroup;
}

std::vector<PointLight *> & LightGroup::getPointLightsArrayReference()
{
    return _pointLights;
}

PointLight *LightGroup::getPointLight(int _id)
{
    std::map<int, PointLight *>::iterator it;
    return ((it = _pointLightsMap.find(_id)) != _pointLightsMap.end()) ? it->second : NULL;
}
