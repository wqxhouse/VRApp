//
//  ShadowGroup.cpp
//  vrphysics
//
//  Created by Robin Wu on 1/7/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#include "ShadowGroup.h"
#include <osgDB/ReadFile>
#include <osg/TextureRectangle>

#include "ShadowCallback.h"

ShadowGroup::ShadowGroup(osg::Camera *mainCamera, osg::Group *geoms)
: _mainCamera(mainCamera), _geoms(geoms)
{
    _nearPlane = -10;
    _farPlane = 20;
    _shadowProjection.makeOrtho(-20, 20, -20, 10, _nearPlane, _farPlane);
    
    _depthMapShader = new osg::Program();
    _depthMapShader->addShader(osgDB::readShaderFile("orthoDepthMap.vert"));
    _depthMapShader->addShader(osgDB::readShaderFile("orthoDepthMap.frag"));
    _depthTexWidth = 1024;
    _depthTexHeight = 1024;
    
    _shadowCameras = new osg::Group;
}

osg::ref_ptr<osg::TextureRectangle> ShadowGroup::createShadowTexture(int width, int height)
{
    osg::ref_ptr<osg::TextureRectangle> tex = new osg::TextureRectangle;
    tex->setTextureSize(width, height);
    
    tex->setSourceType(GL_FLOAT);
//    tex->setSourceFormat(GL_RED);
//    tex->setInternalFormat(GL_R32F);
    tex->setSourceFormat(GL_RGBA);
    tex->setInternalFormat(GL_RGBA32F_ARB);
//    tex->setSourceType(GL_FLOAT);
//    tex->setSourceFormat(GL_RGBA);
//    tex->setInternalFormat(GL_RGBA);
    tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
    tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
    
    return tex;
}

void ShadowGroup::setDepthMapResolution(float width, float height)
{
    _depthTexWidth = width;
    _depthTexHeight = height;
}

void ShadowGroup::addBasicShadowCam(osg::TextureRectangle *outDepthTex, const osg::Matrixf &shadowMV, const osg::Matrixf &shadowMVP, DirectionalLight *dirLight)
{
    osg::ref_ptr<osg::Camera> cam(new osg::Camera);
    cam->setProjectionMatrix(_shadowProjection);
    
    osg::StateSet *ss = cam->getOrCreateStateSet();
    ss->setAttributeAndModes(_depthMapShader, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
    cam->attach(osg::Camera::COLOR_BUFFER0, outDepthTex);
    
    cam->setClearColor(osg::Vec4(1, 1, 1, 1));
    cam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    cam->setRenderOrder(osg::Camera::PRE_RENDER);
    cam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    cam->setViewMatrix(osg::Matrix::identity());
    cam->setViewport(0, 0, _depthTexWidth, _depthTexHeight);
   
    // need to use osg::Matrixf as uniform, or invalid operation
    ss->addUniform(new osg::Uniform("u_nearDistance", _nearPlane));
    ss->addUniform(new osg::Uniform("u_farDistance", _farPlane));
    ss->addUniform(new osg::Uniform("u_lightViewMatrix", shadowMV));
    ss->addUniform(new osg::Uniform("u_lightViewProjectionMatrix", shadowMVP));
    
    osg::ref_ptr<ShadowCallback> shadowCallback(new ShadowCallback(_mainCamera, _shadowProjection));
    shadowCallback->setDirectionalLight(dirLight);
    ss->setUpdateCallback(shadowCallback);
    
    cam->addChild(_geoms);
    _shadowCameras->addChild(cam);
}

osg::ref_ptr<osg::TextureRectangle> ShadowGroup::getDirLightShadowTexture(int light_id)
{
    std::map<int, osg::ref_ptr<osg::TextureRectangle> >::iterator it = _dir_depthMaps.find(light_id);
    if(it != _dir_depthMaps.end())
    {
        return it->second;
    }
    else
    {
        return NULL;
    }
}

void ShadowGroup::addDirectionalLight(DirectionalLight *dirLight, enum ShadowMode mode)
{
    int light_id = dirLight->_id;
    osg::Matrixf shadowView;
    
    if(mode == BASIC)
    {
        shadowView.makeLookAt(dirLight->getPosition(), dirLight->getLookAt(), osg::Vec3(0, 0, 1)); // z-up
        osg::Matrixf shadowMVP = shadowView * _shadowProjection;
        dirLight->_lightViewMatrix = shadowView;
        dirLight->_lightProjectionMatrix = _shadowProjection;
        dirLight->_lightNearDistance = _nearPlane;
        dirLight->_lightFarDistance = _farPlane;
        
        osg::ref_ptr<osg::TextureRectangle> depthTex = createShadowTexture(_depthTexWidth, _depthTexHeight);
        addBasicShadowCam(depthTex, shadowView, shadowMVP, dirLight);
        
        _dir_depthMaps.insert(std::make_pair(light_id, depthTex));
    }
    
}

void ShadowGroup::addMultipleDirectionalLights(std::vector<DirectionalLight *> lights, enum ShadowMode mode)
{
    for(int i = 0; i < lights.size(); i++)
    {
        DirectionalLight *light = lights[i];
        addDirectionalLight(light, mode);
    }
}