//
//  DirectionalLight.h
//  vrphysics
//
//  Created by Robin Wu on 11/27/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#ifndef __vrphysics__DirectionalLight__
#define __vrphysics__DirectionalLight__

#include <stdio.h>
#include <osg/Vec3f>
#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Geometry>

class DirectionalLight
{
public:
    
    DirectionalLight();
    
    void setGeometryRadius(float radius)
    {
        _light_geom_radius = radius;
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
    
    void setPosition(const osg::Vec3f &pos) {
        position = pos;
    }
    
    osg::Vec3f getPosition() {
        return position;
    }
    
    void setLookAt(const osg::Vec3f &lookAt)
    {
        this->lookAt = lookAt;
    }
    
    osg::Vec3f getLookAt()
    {
        return this->lookAt;
    }
    
    inline int getId()
    {
        return _id;
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
    }
    
    void genRenderQuad(int width, int height)
    {
        osg::ref_ptr<osg::Geode> quad_geode = new osg::Geode;
        
        osg::ref_ptr<osg::Vec3Array> quad_coords = new osg::Vec3Array; // vertex coords
        // counter-clockwise
        quad_coords->push_back(osg::Vec3d(0, 0, -1));
        quad_coords->push_back(osg::Vec3d(width, 0, -1));
        quad_coords->push_back(osg::Vec3d(width, height, -1));
        quad_coords->push_back(osg::Vec3d(0, height, -1));
        
        osg::ref_ptr<osg::Geometry> quad_geom = new osg::Geometry;
        osg::ref_ptr<osg::DrawArrays> quad_da = new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4);
        
        osg::ref_ptr<osg::Vec4Array> quad_colors = new osg::Vec4Array;
        quad_colors->push_back(osg::Vec4(1.0f,0.0f,1.0f,1.0f));
        
        quad_geom->setVertexArray(quad_coords.get());
        quad_geom->addPrimitiveSet(quad_da.get());
        quad_geom->setColorArray(quad_colors.get(), osg::Array::BIND_OVERALL);
        
        auto stateSet = quad_geom->getOrCreateStateSet();
        stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        quad_geode->addDrawable(quad_geom);
        _renderQuad = quad_geode;
    }

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
    
    float ambient[4];
    float diffuse[4];
    float specular[4];
    
    float intensity;
    
    osg::Vec3f orbitAxis;
    osg::Vec3f position;
    osg::Vec3f lookAt;
    
    osg::ref_ptr<osg::Geode> _sphereGeode;
    osg::ref_ptr<osg::Geode> _renderQuad;
    osg::ref_ptr<osg::MatrixTransform> _lightGeomTransform;
    
    osg::Matrixf _lightViewMatrix;
    osg::Matrixf _lightProjectionMatrix;
    osg::Matrixf _lightMVP;
    float _lightNearDistance;
    float _lightFarDistance;
    
    int _id;
    static int _highest_id;
    
    float _light_geom_radius;
};

#endif /* defined(__vrphysics__DirectionalLight__) */
