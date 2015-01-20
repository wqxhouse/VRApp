////
////  SpotLightGroup.cpp
////  vrphysics
////
////  Created by Robin Wu on 1/7/15.
////  Copyright (c) 2015 WSH. All rights reserved.
////
//
//#include "SpotLightGroup.h"
//
//SpotLightGroup::SpotLightGroup()
//: _lightnum(0)
//{
//    _spotLightGroup = new osg::Group;
//}
//
//SpotLightGroup::~SpotLightGroup()
//{
//}
//
//std::vector<SpotLight *> & SpotLightGroup::getDirectionalLightsReference()
//{
//    return _spotLights;
//}
//
//void SpotLightGroup::addMultipleLights(const std::vector<SpotLight *> &lights)
//{
//    _lightnum += lights.size();
//    for(unsigned long i = 0; i < lights.size(); i++)
//    {
//        _spotLightGroup->addChild(lights[i]->_lightGeomTransform);
//        _spotLights.push_back(lights[i]);
//    }
//}
//
//void SpotLightGroup::addLight(const osg::Vec3 &pos, const osg::Vec3 &lookAt, const osg::Vec3 &color)
//{
//    SpotLight *light = new SpotLight();
//    light->setPosition(pos);
//    light->setDirection(lookAt);
//    light->setDiffuse(color.x(), color.y(), color.z());
//    light->setSpecular(0, 0, 0);
//    light->intensity = 0.8f;
//    
////    light->genGeomTransform(0.25);
//    _spotLightGroup->addChild(light->_lightGeomTransform);
//    light->orbitAxis = osg::Vec3(0, 0, 1);
//    
//    _spotLights.push_back(light);
//    _lightnum++;
//}