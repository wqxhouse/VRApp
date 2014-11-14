//
//  LightPassCameraCallback.h
//  vrphysics
//
//  Created by Robin Wu on 11/6/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__LightPassCameraCallback__
#define __vrphysics__LightPassCameraCallback__

#include <stdio.h>
#include <osg/NodeCallback>
#include <osg/Camera>

class LightPassCameraCallback : public osg::NodeCallback
{
public:
    LightPassCameraCallback();
    virtual ~LightPassCameraCallback();
    virtual void operator()(osg::Node *node, osg::NodeVisitor *nv);
};

#endif /* defined(__vrphysics__LightPassCameraCallback__) */
