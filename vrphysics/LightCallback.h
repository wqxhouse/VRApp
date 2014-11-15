//
//  LightCallback.h
//  vrphysics
//
//  Created by Robin Wu on 11/13/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__LightCallback__
#define __vrphysics__LightCallback__

#include <stdio.h>
#include <stdio.h>
#include <osg/NodeVisitor>
#include <osg/Uniform>
#include <osg/Camera>

#include "PointLight.h"
#include "LightGroup.h"
#include "Utils.h"

class LightCallback : public osg::StateSet::Callback
{
public:
    LightCallback(osg::Camera *mainCamera, PointLight *pointLight)
    {
        _mainCamera = mainCamera;
        _light = pointLight;
    }
    
    ~LightCallback() {}
    
    virtual void operator()(osg::StateSet *ss, osg::NodeVisitor *nv);
    
 
private:
    osg::ref_ptr<osg::Camera> _mainCamera;
    PointLight *_light;
};

#endif /* defined(__vrphysics__LightCallback__) */
