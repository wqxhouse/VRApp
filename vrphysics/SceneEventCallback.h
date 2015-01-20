//
//  SceneEventCallback.h
//  vrphysics
//
//  Created by Robin Wu on 12/9/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__SceneEventCallback__
#define __vrphysics__SceneEventCallback__

#include <stdio.h>
#include <osg/NodeCallback>
#include "KeyboardHandler.h"
class SceneEventCallback : osg::NodeCallback
{
public:
    SceneEventCallback(KeyboardHandler *handler);
    ~SceneEventCallback();
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
    
private:
    KeyboardHandler *_keyboard;
};

#endif /* defined(__vrphysics__SceneEventCallback__) */
