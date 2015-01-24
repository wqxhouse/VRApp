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
    ShadowCallback(osg::Camera *mainCamera, osg::Matrix shadowProjection)
        : _mainCamera(mainCamera), _shadowProjection(shadowProjection), _dirLight(NULL), _pointLight(NULL) { };
    ~ShadowCallback() {};
   
    void setPointLight(PointLight *pl);
    void setDirectionalLight(DirectionalLight *dirLight);
    
    void operator()(osg::StateSet *ss, osg::NodeVisitor* nv);
    
private:

    osg::ref_ptr<osg::Camera> _mainCamera;
    osg::Matrix _shadowProjection;
    
    DirectionalLight *_dirLight;
    PointLight *_pointLight;
};

#endif /* defined(__vrphysics__ShadowCallback__) */
