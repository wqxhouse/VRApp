//
//  KeyboardHandler.h
//  vrphysics
//
//  Created by Robin Wu on 12/9/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__KeyboardHandler__
#define __vrphysics__KeyboardHandler__

#include <stdio.h>
#include <osgGA/GUIEventHandler>

class LightGroup;
class KeyboardHandler : public osgGA::GUIEventHandler
{
public:
    KeyboardHandler(osg::ref_ptr<osg::Group> sceneRoot, osg::ref_ptr<osg::Group> hud, LightGroup *lightGroup);
    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&);
    virtual void accept(osgGA::GUIEventHandlerVisitor& v)   { v.visit(*this); };
private:
    void offsetExpAttenuation(int offset);
    
    LightGroup *_pointLightGroup;
    osg::ref_ptr<osg::Group> _sceneRoot;
    osg::ref_ptr<osg::Group> _hud;
    bool _hudOn;
};


#endif /* defined(__vrphysics__KeyboardHandler__) */
