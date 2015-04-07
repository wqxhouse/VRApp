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
#include "FrustumData.h"

class ShadowCallback : public osg::StateSet::Callback
{
public:
    ShadowCallback(osg::Camera *mainCamera, osg::Camera *lightCamera, osg::Vec2 shadowMapSize)
        : _mainCamera(mainCamera), _lightCamera(lightCamera), _shadowMapSize(shadowMapSize),
    _dirLight(NULL), _pointLight(NULL) { };
    
    ~ShadowCallback() {};
   
    void setPointLight(PointLight *pl);
    void setDirectionalLight(DirectionalLight *dirLight);
    
    void operator()(osg::StateSet *ss, osg::NodeVisitor* nv);
    
private:
    // std::pair<float, float> getNearFar();

    osg::ref_ptr<osg::Camera> _mainCamera;
    osg::ref_ptr<osg::Camera> _lightCamera;
    // osg::Matrix _shadowProjection;
    osg::Vec2 _shadowMapSize;
    
    DirectionalLight *_dirLight;
    PointLight *_pointLight;
    
    void processCascades();
    // const osg::BoundingBox &_sceneAABB;
    // osg::Matrix calcProjectionMatrix(const osg::Matrix &viewInverse);
    void makeGlobalShadowMatrix(osg::Matrixd &shadowOrthoMat, osg::Matrixd &shadowViewMat, float &nearVal, float &farVal);
};

class RSMCallback : public osg::StateSet::Callback
{
public:
    RSMCallback(osg::Camera *mainCamera, osg::Camera *lightCamera, osg::Vec2 rsmSize, const osg::BoundingBox &sceneAABB)
    : _mainCamera(mainCamera), _lightCamera(lightCamera), _rsmSize(rsmSize),
    _dirLight(NULL), _sceneAABB(sceneAABB) { };
    
    ~RSMCallback() {};
    
    void setDirectionalLight(DirectionalLight *dirLight);
    
    void operator()(osg::StateSet *ss, osg::NodeVisitor* nv);
    
private:
    std::pair<float, float> getNearFar();
    
    osg::ref_ptr<osg::Camera> _mainCamera;
    osg::ref_ptr<osg::Camera> _lightCamera;
    
    osg::Vec2 _rsmSize;
    DirectionalLight *_dirLight;
    
    const osg::BoundingBox &_sceneAABB;
    
    osg::Matrix calcProjectionMatrix(const osg::Matrix &viewInverse);
};

#endif /* defined(__vrphysics__ShadowCallback__) */
