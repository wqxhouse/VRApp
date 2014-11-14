//
//  Utils.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/13/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "Utils.h"

void Utils::print(osg::Vec3 v3, char ch)
{
    std::cout << std::setprecision(4)
    << std::setw(8) << std::setfill(' ') << v3.x() << ch
    << std::setw(8) << std::setfill(' ') << v3.y() << ch
    << std::setw(8) << std::setfill(' ') << v3.z() << std::endl;
}


void Utils::print(osg::Vec4 v4, char ch)
{
    std::cout << std::setprecision(4)
    << std::setw(8) << std::setfill(' ') << v4.x() << ch
    << std::setw(8) << std::setfill(' ') << v4.y() << ch
    << std::setw(8) << std::setfill(' ') << v4.z() << ch
    << std::setw(8) << std::setfill(' ') << v4.w() << std::endl;
}


void Utils::print(osg::Matrix mat)
{
    double* temp = mat.ptr();
    for(int i=0;i<13;i+=4)
    {
        print(osg::Vec4(temp[i],temp[i+1],temp[i+2],temp[i+3]),'\t');
    }
}

void Utils::print(osg::Quat q)
{
    std::cout << "_v    = " << std::setprecision(4)<< q._v[0] << " " << q._v[1] << " " << q._v[2] << " " << q._v[3] << std::endl;
    
    double angle;
    osg::Vec3 vec;
    q.getRotate(angle,vec);
    std::cout << "angle = " << std::setprecision(4) << osg::RadiansToDegrees(angle) << " (in degree)" << std::endl;
    std::cout << "vec   = (" << std::setprecision(4) << vec.x() << ", " << vec.y() << ", " << vec.z() << ")" << std::endl;
}