//
//  Utils.h
//  vrphysics
//
//  Created by Robin Wu on 11/12/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef vrphysics_Utils_h
#define vrphysics_Utils_h

#include <iostream>
#include <sstream>
#include <iomanip>
//#include <conio.h>

//#include <osg/Viewer>

#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

#include <osgDB/ReadFile>

class Utils
{
public:
    //////////////////////////////////////////////////////////////////////////
    // 输出各种数据
    static void print(osg::Vec3 v3, char ch);
    static void print(osg::Vec4 v4, char ch);
    static void print(osg::Matrix mat);
    static void print(osg::Quat q);
};

#endif
