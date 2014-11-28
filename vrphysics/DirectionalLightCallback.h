//
//  DirectionalLightCallback.h
//  vrphysics
//
//  Created by Robin Wu on 11/27/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__DirectionalLightCallback__
#define __vrphysics__DirectionalLightCallback__

#include <stdio.h>
#include <stdio.h>
#include <osg/NodeCallback>
#include <osg/Camera>
#include "DirectionalLight.h"

class DirectionalLightCallback : public osg::StateSet::Callback
{
public:
    DirectionalLightCallback(osg::Camera *mainCamera, DirectionalLight *light);
    virtual ~DirectionalLightCallback() {}
    virtual void operator()(osg::StateSet* ss, osg::NodeVisitor* nv);
    
private:
    osg::ref_ptr<osg::Camera> _mainCamera;
    DirectionalLight *_light;
};

#endif /* defined(__vrphysics__DirectionalLightCallback__) */
