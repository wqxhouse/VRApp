//
//  LightGroupAnimationCallback.h
//  vrphysics
//
//  Created by Robin Wu on 11/14/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__LightGroupAnimationCallback__
#define __vrphysics__LightGroupAnimationCallback__

#include <stdio.h>
#include <osg/NodeCallback>
#include <osg/Camera>

#include "LightGroup.h"

class LightGroupAnimationCallback : public osg::NodeCallback
{
public:
    LightGroupAnimationCallback(LightGroup *group);
    
    virtual ~LightGroupAnimationCallback() {} ;
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
    
private:
    LightGroup *_group;
    float _angle;
};

#endif /* defined(__vrphysics__LightGroupAnimationCallback__) */
