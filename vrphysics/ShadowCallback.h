//
//  ShadowCallback.h
//  vrphysics
//
//  Created by Robin Wu on 1/18/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#ifndef __vrphysics__ShadowCallback__
#define __vrphysics__ShadowCallback__

#include <stdio.h>
#include <osg/StateSet>
#include <osg/Camera>
#include "DirectionalLight.h"
#include "PointLight.h"

class ShadowCallback : public osg::StateSet::Callback
{
public:
    ShadowCallback(osg::Camera *lightCamera, osg::Matrix shadowProjection, const osg::BoundingBox &sceneAABB)
        : _lightCamera(lightCamera), _shadowProjection(shadowProjection),
    _dirLight(NULL), _pointLight(NULL), _sceneAABB(sceneAABB) { };
    
    ~ShadowCallback() {};
   
    void setPointLight(PointLight *pl);
    void setDirectionalLight(DirectionalLight *dirLight);
    
    void operator()(osg::StateSet *ss, osg::NodeVisitor* nv);
    
private:
    std::pair<float, float> getNearFar();

    osg::ref_ptr<osg::Camera> _lightCamera;
    osg::Matrix _shadowProjection;
    
    DirectionalLight *_dirLight;
    PointLight *_pointLight;
    
    const osg::BoundingBox &_sceneAABB;
    
    osg::Matrix calcProjectionMatrix(const osg::Matrix &viewInverse);
};

#endif /* defined(__vrphysics__ShadowCallback__) */
