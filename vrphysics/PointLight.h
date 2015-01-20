//
//  PointLight.h
//  vrphysics
//
//  Created by Robin Wu on 11/7/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__PointLight__
#define __vrphysics__PointLight__

#include <stdio.h>

#include <iostream>
#include <osg/Vec3f>
#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include "Utils.h"

class PointLight {
public:
    static const int maxEffectiveRadius = 8;
    
    PointLight();
    
    void setGeometryRadius(float radius)
    {
        _light_geom_radius = radius;
    }
    
    void setMaxEffectiveRadius(float radius)
    {
        _light_max_effective_radius = radius;
        _light_effective_radius = _light_max_effective_radius * intensity;
    }
    
    void setAmbient(float r, float g, float b, float a=1.0f) {
        ambient[0] = r;
        ambient[1] = g;
        ambient[2] = b;
        ambient[3] = a;
    }
    
    void setDiffuse(float r, float g, float b, float a=1.0f) {
        diffuse[0] = r;
        diffuse[1] = g;
        diffuse[2] = b;
        diffuse[3] = a;
    }
    
    void setSpecular(float r, float g, float b, float a=1.0f) {
        specular[0] = r;
        specular[1] = g;
        specular[2] = b;
        specular[3] = a;
    }
    
    void setAttenuation(float constant, float linear, float exponential) {
        attenuation[0] = constant;
        attenuation[1] = linear;
        attenuation[2] = exponential;
    }
    
    void setPosition(const osg::Vec3f &pos) {
        position = pos;
        
        //osg::Matrix m = _rootTransform->getMatrix();
        //m.setTrans(pos);
        //_rootTransform->setMatrix(m);
    }
    
    osg::Vec3f getPosition() {
        return position;
    }
    
    void genGeometry()
    {
        osg::ref_ptr<osg::Sphere> sphere(new osg::Sphere(osg::Vec3(0, 0, 0), 1.0));
        osg::ref_ptr<osg::ShapeDrawable> shapeDraw(new osg::ShapeDrawable);
        shapeDraw->setShape(sphere);
        osg::ref_ptr<osg::Geode> sphereGeode(new osg::Geode);
        sphereGeode->addDrawable(shapeDraw);
        
        osg::Matrix translate;
        translate.makeTranslate(getPosition());
        _sphereGeode = sphereGeode;
        //_rootTransform = new osg::MatrixTransform(translate);
        //_rootTransform->addChild(sphereGeode);
    }
    
//    void genGeomTransform(float scaleFactor)
//    {
//        osg::ref_ptr<osg::MatrixTransform> mt(new osg::MatrixTransform);
//        osg::Matrix m;
//        m.makeScale(osg::Vec3(scaleFactor, scaleFactor, scaleFactor));
//        m.translate(position);
//        mt->setMatrix(m);
//        mt->addChild(getRoot());
//        _lightGeomTransform = mt;
//    }
    void genGeomTransform(float scaleFactor)
    {
        osg::ref_ptr<osg::MatrixTransform> mt;
        if(_lightGeomTransform.get() == NULL)
        {
            mt = new osg::MatrixTransform;
            _lightGeomTransform = mt;
            mt->addChild(getRoot());
        }
        else
        {
            mt = _lightGeomTransform;
        }
        
        osg::Matrix m;
        m.postMultScale(osg::Vec3(scaleFactor, scaleFactor, scaleFactor));
        m.postMultTranslate(position);
        mt->setMatrix(m);
    }
    
    
    void genLightSphereTransform(float scaleFactor)
    {
        osg::ref_ptr<osg::MatrixTransform> mt;
        if(_lightSphereTransform.get() == NULL)
        {
            mt = new osg::MatrixTransform;
            _lightSphereTransform = mt;
            mt->addChild(getRoot());
        }
        else
        {
            mt = _lightSphereTransform;
        }
        
        osg::Matrix m;
        m.postMultScale(osg::Vec3(scaleFactor, scaleFactor, scaleFactor));
        m.postMultTranslate(position);
        mt->setMatrix(m);
    }
    
    osg::ref_ptr<osg::Geode> getRoot()
    {
        return _sphereGeode;
    }
    
    osg::Vec4 getAmbient()
    {
        return osg::Vec4(ambient[0], ambient[1], ambient[2], ambient[3]);
    }
    
    osg::Vec4 getDiffuse()
    {
        return osg::Vec4(diffuse[0], diffuse[1], diffuse[2], diffuse[3]);
    }
    
    osg::Vec4 getSpecular()
    {
        return osg::Vec4(specular[0], specular[1], specular[2], specular[3]);
    }
    
    osg::Vec3 getAttenuation()
    {
        return osg::Vec3(attenuation[0], attenuation[1], attenuation[2]);
    }
    
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float attenuation[3];
    
    float intensity;
    
    osg::Vec3f orbitAxis;
    osg::Vec3f position;
    
    osg::ref_ptr<osg::Geode> _sphereGeode;
    osg::ref_ptr<osg::MatrixTransform> _lightSphereTransform;
    osg::ref_ptr<osg::MatrixTransform> _lightGeomTransform;
    
    int _id;
    static int _highest_id;
    
    float _light_geom_radius;
    float _light_effective_radius;
    float _light_max_effective_radius;
};

#endif /* defined(__deferred__pointlight__) */


