//
//  KeyboardHandler.cpp
//  vrphysics
//
//  Created by Robin Wu on 12/9/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "KeyboardHandler.h"

KeyboardHandler::KeyboardHandler(osg::ref_ptr<osg::Group> sceneRoot, osg::ref_ptr<osg::Group> hud)
:_sceneRoot(sceneRoot), _hud(hud), _hudOn(true)
{
}

bool KeyboardHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::KEYDOWN):
        {
            switch(ea.getKey())
            {
                case 'h':
                    if(_hudOn)
                    {
                        _sceneRoot->removeChild(_hud);
                        _hudOn = false;
                    }
                    else
                    {
                        _sceneRoot->addChild(_hud);
                        _hudOn = true;
                    }
                    return false; // ?
                    break;
                default:
                    return false;
            }
        }
        default:
            return false;
    }
}