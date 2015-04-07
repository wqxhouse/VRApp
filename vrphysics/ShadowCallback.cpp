//
//  ShadowCallback.cpp
//  vrphysics
//
//  Created by Robin Wu on 1/18/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#include "ShadowCallback.h"
#include <float.h>

#include "FrustumData.h"

void ShadowCallback::setPointLight(PointLight *pl)
{
    _pointLight = pl;
    _dirLight = NULL;
}

void ShadowCallback::setDirectionalLight(DirectionalLight *dirLight)
{
    _dirLight = dirLight;
    _pointLight = NULL;
}

//std::pair<float, float> ShadowCallback::getNearFar()
//{
//    float dummy;
//    float near;
//    float far;
//    _shadowProjection.getOrtho(dummy, dummy, dummy, dummy, near, far);
//   
//    return std::make_pair(near, far);
//}

//osg::Matrix ShadowCallback::calcProjectionMatrix(const osg::Matrix &viewMatrix)
//{
//    float xMin, xMax, yMin, yMax, zMin, zMax;
//    xMin = 1000000000;
//    xMax = -1000000000;
//    yMin = 1000000000;
//    yMax = -1000000000;
//    zMin = 1000000000;
//    zMax = -1000000000;
//   
//    for(int i = 0; i < 8; i++)
//    {
//        osg::Vec3 v = _sceneAABB.corner(i);
//        osg::Vec3 viewV = v * viewMatrix;
//        
//        if(viewV.x() > xMax) xMax = viewV.x();
//        if(viewV.x() < xMin) xMin = viewV.x();
//        
//        if(viewV.y() > yMax) yMax = viewV.y();
//        if(viewV.y() < yMin) yMin = viewV.y();
//        if(viewV.z() > zMax) zMax = viewV.z();
//        if(viewV.z() < zMin) zMin = viewV.z();
//    }
//    osg::Matrix proj;
//    float near, far;
//    if(zMax < 0 && zMin < 0)
//    {
//        near = -zMax;
//        far = -zMin;
//    }
//    else // zMax > 0 && zMin > 0 || zMax > 0 zMin < 0
//    {
//        near = zMin;
//        far = zMax;
//    }
//    
//    proj.makeOrtho(xMin, xMax, yMin, yMax, near, far);
//    return proj;
//}

void ShadowCallback::makeGlobalShadowMatrix(osg::Matrixd &shadowOrthoMat, osg::Matrixd &shadowViewMat, float &nearVal, float &farVal)
{
    FrustumData fdata;

    fdata.updateMatrix(&_mainCamera->getViewMatrix(), &_mainCamera->getProjectionMatrix());
    
    osg::Vec3 ws_viewFrustumCentroid = fdata.getCenter();
    osg::Vec3 ws_lightDir = _dirLight->getPosition() - _dirLight->getLookAt();
    ws_lightDir.normalize();
    
    // use constant z-up for stability.
    osg::Vec3 upDir = osg::Vec3(0, 0, 1);
    
    float bsphereRad = fdata.getBSphereRadius();
    osg::Vec3 maxExtents = osg::Vec3(bsphereRad, bsphereRad, bsphereRad);
    osg::Vec3 minExtents = -maxExtents;
 
    osg::Vec3 extent = maxExtents - minExtents;
 
    // stablize vpl sampling, could be further improved
    ws_viewFrustumCentroid.normalize();
    // Utils::print(ws_viewFrustumCentroid, ' ');
    
    osg::Vec3 shadowCamPos = ws_viewFrustumCentroid + ws_lightDir * -minExtents.z();
    shadowOrthoMat = osg::Matrixd::ortho(minExtents.x(), maxExtents.x(), minExtents.y(), maxExtents.y(), 0.0, extent.z());

    shadowViewMat = osg::Matrixd::lookAt(shadowCamPos, ws_viewFrustumCentroid, upDir);
//    Utils::print(shadowViewMat);
    
    nearVal = 0.0;
    farVal = extent.z();
    
    osg::Matrix shadowVPMat = shadowViewMat * shadowOrthoMat;
    osg::Vec4 origin(0.0f, 0.0f, 0.0f, 1.0f);
    origin = origin * shadowVPMat;
    origin.x() *= _shadowMapSize.x() / 2.0f;
    origin.y() *= _shadowMapSize.y() / 2.0f;
    
    osg::Vec4 roundedOrigin;
    roundedOrigin.x() = osg::round(origin.x());
    roundedOrigin.y() = osg::round(origin.y());
    roundedOrigin.z() = osg::round(origin.z());
    roundedOrigin.w() = osg::round(origin.w());
    
    osg::Vec4 roundOffset = roundedOrigin - origin;
    roundOffset.x() *= 2.0f / _shadowMapSize.x();
    roundOffset.y() *= 2.0f / _shadowMapSize.y();
    roundOffset.z() = 0.0f;
    roundOffset.w() = 0.0f;
    
    shadowOrthoMat(3, 0) += roundOffset.x();
    shadowOrthoMat(3, 1) += roundOffset.y();
    
    // debug - fix light cam
//    shadowOrthoMat.makeOrtho(-10, 10, -10, 10, 0, 10);
//    shadowViewMat.makeLookAt(_dirLight->getPosition(), _dirLight->getLookAt(), osg::Vec3(0, 0, 1));
}

void ShadowCallback::processCascades()
{
    LightFrustraConsts *frustra = &_dirLight->_lightFrustraConsts;
    const float minDistance = 0.0f;
    const float maxDistance = 1.0f;
    float splits[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    
    // TODO: support in app modification of weights
    splits[0] = minDistance + 0.05 * maxDistance;
    splits[1] = minDistance + 0.15 * maxDistance;
    splits[2] = minDistance + 0.50 * maxDistance;
    splits[3] = minDistance + 1.00 * maxDistance;
    
    FrustumData fdata;
    fdata.updateMatrix(&_mainCamera->getViewMatrix(), &_mainCamera->getProjectionMatrix());
    osg::Vec3 corners[8];

    for(int casIndex = 0; casIndex < frustra->numCascades; casIndex++)
    {
        // renew corners
        for(int i = 0; i < 8; i++)
        {
            corners[i] = fdata.corner(i);
        }
        
        float prevSplitDist = casIndex == 0 ? minDistance : splits[casIndex - 1];
        float splitDist = splits[casIndex];
        
        // get corners of current cascade
        for(int i = 0; i < 4; i++)
        {
            osg::Vec3 cornerRay = corners[i + 4] - corners[i];
            osg::Vec3 nearCornerRay = cornerRay * prevSplitDist;
            osg::Vec3 farCornerRay = cornerRay * splitDist;
            corners[i+4] = corners[i] + farCornerRay;
            corners[i] = corners[i] + nearCornerRay;
        }
        
        osg::Vec3 ws_viewFrustumCentroid = fdata.getCenter();
        osg::Vec3 ws_lightDir = _dirLight->getPosition() - _dirLight->getLookAt();
        ws_lightDir.normalize();
        
        // use constant z-up for stability.
        osg::Vec3 upDir = osg::Vec3(0, 0, 1);
        
        float bsphereRad = fdata.getBSphereRadius();
        osg::Vec3 maxExtents = osg::Vec3(bsphereRad, bsphereRad, bsphereRad);
        osg::Vec3 minExtents = -maxExtents;
        
        osg::Vec3 extent = maxExtents - minExtents;
        
        // stablize vpl sampling, could be further improved
        ws_viewFrustumCentroid.normalize();
        // Utils::print(ws_viewFrustumCentroid, ' ');
        
        osg::Vec3 shadowCamPos = ws_viewFrustumCentroid + ws_lightDir * -minExtents.z();
//        shadowOrthoMat = osg::Matrixd::ortho(minExtents.x(), maxExtents.x(), minExtents.y(), maxExtents.y(), 0.0, extent.z());
//        
//        shadowViewMat = osg::Matrixd::lookAt(shadowCamPos, ws_viewFrustumCentroid, upDir);
//        //    Utils::print(shadowViewMat);
//        
//        nearVal = 0.0;
//        farVal = extent.z();
//        
//        osg::Matrix shadowVPMat = shadowViewMat * shadowOrthoMat;
//        osg::Vec4 origin(0.0f, 0.0f, 0.0f, 1.0f);
//        origin = origin * shadowVPMat;
//        origin.x() *= _shadowMapSize.x() / 2.0f;
//        origin.y() *= _shadowMapSize.y() / 2.0f;
//        
//        osg::Vec4 roundedOrigin;
//        roundedOrigin.x() = osg::round(origin.x());
//        roundedOrigin.y() = osg::round(origin.y());
//        roundedOrigin.z() = osg::round(origin.z());
//        roundedOrigin.w() = osg::round(origin.w());
//        
//        osg::Vec4 roundOffset = roundedOrigin - origin;
//        roundOffset.x() *= 2.0f / _shadowMapSize.x();
//        roundOffset.y() *= 2.0f / _shadowMapSize.y();
//        roundOffset.z() = 0.0f;
//        roundOffset.w() = 0.0f;
//        
//        shadowOrthoMat(3, 0) += roundOffset.x();
//        shadowOrthoMat(3, 1) += roundOffset.y();

    }
}

void ShadowCallback::operator()(osg::StateSet *ss, osg::NodeVisitor* nv)
{
//    osg::Matrixf shadowMV;
//    osg::Matrixf shadowViewInverse;
//    if(_dirLight != NULL)
//    {
//        shadowMV.makeLookAt(_dirLight->getPosition(), _dirLight->getLookAt(), osg::Vec3(0, 0, 1));
//        shadowViewInverse = osg::Matrixf::inverse(shadowMV);
//    }

    // _shadowProjection = calcProjectionMatrix(shadowMV);
    osg::Matrix shadowProj;
    osg::Matrix shadowView;
    float nearVal;
    float farVal;
    
    makeGlobalShadowMatrix(shadowProj, shadowView, nearVal, farVal);
    
    osg::Matrixf shadowVP = shadowView * shadowProj;
    osg::Matrixf shadowViewInverse = osg::Matrixf::inverse(shadowView);
    
//    ss->getUniform("u_lightViewMatrix")->set(osg::Matrixf::inverse(shadowView));
//    ss->getUniform("u_lightViewProjectionMatrix")->set(shadowMVP);
    _lightCamera->setViewMatrix(shadowView);
    _lightCamera->setProjectionMatrix(shadowProj);
    
//    ss->getUniform("u_lightViewInverseMatrix")->set(shadowViewInverse);
//    ss->getUniform("u_lightPos")->set(_dirLight->getPosition());
   
//    std::pair<float, float> nearFar = getNearFar();
    ss->getUniform("u_zLength")->set(farVal - nearVal);
    ss->getUniform("u_nearDistance_s")->set(nearVal);
    
    _dirLight->_lightNearDistance = nearVal;
    _dirLight->_lightFarDistance = farVal;
    
    _dirLight->_lightProjectionMatrix = shadowProj;
    _dirLight->_lightViewMatrix = shadowView;
    _dirLight->_lightMVP = shadowVP;
}

// ======================
// RSM
// ======================
void RSMCallback::operator()(osg::StateSet *ss, osg::NodeVisitor* nv)
{
    osg::Matrixf shadowMV;
    osg::Matrixf shadowViewInverse;
    if(_dirLight != NULL)
    {
        shadowMV.makeLookAt(_dirLight->getPosition(), _dirLight->getLookAt(), osg::Vec3(0, 0, 1));
        shadowViewInverse = osg::Matrixf::inverse(shadowMV);
    }
    
    osg::Matrix shadowProjection = calcProjectionMatrix(shadowMV);
    osg::Matrixf shadowMVP = shadowMV * shadowProjection;
    
    _lightCamera->setViewMatrix(shadowMV);
    _lightCamera->setProjectionMatrix(shadowProjection);
    ss->getUniform("u_lightViewInverseMatrix")->set(shadowViewInverse);
    ss->getUniform("u_lightPos")->set(_dirLight->getPosition());
}

void RSMCallback::setDirectionalLight(DirectionalLight *dirLight)
{
    _dirLight = dirLight;
}

osg::Matrix RSMCallback::calcProjectionMatrix(const osg::Matrix &viewInverse)
{
    float xMin, xMax, yMin, yMax, zMin, zMax;
    xMin = 1000000000;
    xMax = -1000000000;
    yMin = 1000000000;
    yMax = -1000000000;
    zMin = 1000000000;
    zMax = -1000000000;

    for(int i = 0; i < 8; i++)
    {
        osg::Vec3 v = _sceneAABB.corner(i);
        osg::Vec3 viewV = v * viewInverse;

        if(viewV.x() > xMax) xMax = viewV.x();
        if(viewV.x() < xMin) xMin = viewV.x();

        if(viewV.y() > yMax) yMax = viewV.y();
        if(viewV.y() < yMin) yMin = viewV.y();
        if(viewV.z() > zMax) zMax = viewV.z();
        if(viewV.z() < zMin) zMin = viewV.z();
    }
    osg::Matrix proj;
    float near, far;
    if(zMax < 0 && zMin < 0)
    {
        near = -zMax;
        far = -zMin;
    }
    else // zMax > 0 && zMin > 0 || zMax > 0 zMin < 0
    {
        near = zMin;
        far = zMax;
    }
    
    proj.makeOrtho(xMin, xMax, yMin, yMax, near, far);
    return proj;
}
