//
//  CustomFirstPersonManipulator.cpp
//  vrphysics
//
//  Created by Robin Wu on 3/5/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#include "CustomFirstPersonManipulator.h"

CustomFirstPersonManipulator::CustomFirstPersonManipulator(int flag)
: osgGA::FirstPersonManipulator(flag)
{
    
}

CustomFirstPersonManipulator::~CustomFirstPersonManipulator()
{
    
}


bool CustomFirstPersonManipulator::handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
    if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Space )
    {
        flushMouseEventStack();
        _thrown = false;
        home(ea,us);
        return true;
    }
    if( ea.getKey() == 'w' )
    {
        osg::Vec3d eye;
        osg::Vec3d center;
        osg::Vec3d up;
//        getTransformation(eye, center, up);
//        printf("eye %f, %f, %f\n", eye.x(), eye.y(), eye.z());
//        printf("cen %f, %f, %f\n", center.x(), center.y(), center.z());
//        printf("up  %f, %f, %f\n", up.x(), up.y(), up.z());
        moveForward(5.0);
        return true;
    }
    if( ea.getKey() == 's' )
    {
        moveForward(-5.0);
        return true;
    }
    if( ea.getKey() == 'a' )
    {
        moveRight(-5.0);
        return true;
    }
    if( ea.getKey() == 'd' )
    {
        moveRight(5.0);
        return true;
    }
    return false;

}
