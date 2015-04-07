//
//  Frustum.h
//  vrphysics
//
//  Created by Robin Wu on 3/3/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#ifndef __vrphysics__Frustum__
#define __vrphysics__Frustum__

#include <stdio.h>
#include <osg/Matrix>
#include <osg/Vec3>
#include <osg/Geode>

// for cascaded shadow map
struct LightFrustraConsts
{
    static const int numCascades = 4;
    osg::Vec3 _camPosWS;
    osg::Matrix _shadowMatrix;
    float _cascadeSplits[numCascades];
    osg::Vec4 _cascadeOffsets[numCascades];
    osg::Vec4 _cascadeScales[numCascades];
};

class FrustumData
{
public:
    FrustumData();
    ~FrustumData();
    
    inline osg::ref_ptr<osg::Geode> getGeode()
    {
        return _frustumGeode;
    }
    
    inline void updateMatrix(const osg::Matrixd *const viewMat, const osg::Matrixd *const projMat)
    {
        _viewMatrixPtr = viewMat;
        _projectionMatrixPtr = projMat;
        calcCorners();
        calcCenter();
    }
    
    inline osg::Vec3 getCenter()
    {
        return _center;
    }
    
    inline osg::Vec3 corner(int i)
    {
        return osg::Vec3((*_frustumVertices)[i].x(),
                         (*_frustumVertices)[i].y(),
                         (*_frustumVertices)[i].z());
    }
    
    float getBSphereRadius();
    
private:
    void resetVertices();
    void calcCenter();
    void calcCorners();
    void setupGeode();
    // osg::Vec3 _corners[8];
    const osg::Matrixd *_viewMatrixPtr;
    const osg::Matrixd *_projectionMatrixPtr;
   
    osg::Vec3 _center;
    osg::ref_ptr<osg::Geode> _frustumGeode;
    osg::ref_ptr<osg::Geometry> _frustumGeometry;
    osg::ref_ptr<osg::Vec4Array> _frustumVertices;
    osg::ref_ptr<osg::DrawElementsUInt> _frustumIndices;
    
    static const osg::Vec4 _unitCubeCorners[8];
};

#endif /* defined(__vrphysics__Frustum__) */
