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
    _isGIEnabled = true;
    _nearPlane = -10;
    _farPlane = 20;
    _shadowProjection.makeOrtho(-5, 5, -5, 5, _nearPlane, _farPlane);
    // gi test settings
//    _nearPlane = 14;
//    _farPlane = 55;
//    _shadowProjection.makePerspective(40, 1.0f, _nearPlane, _farPlane);
    
    _depthMapShader = new osg::Program();
    _depthMapShader->addShader(osgDB::readShaderFile("orthoDepthMap.vert"));
    _depthMapShader->addShader(osgDB::readShaderFile("orthoDepthMap.frag"));
    _depthTexWidth = 512;
    _depthTexHeight = 512;
   
    // TODO, currently not working, since MRT only support a single resolution
    // Therefore, rsm and shadow maps are using the same resolution,
    // later, they needs to be separated for performance and quality.
    _rsmTexWidth = _depthTexWidth;
    _rsmTexHeight = _depthTexHeight;
    
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
    tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::NEAREST);
    tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::NEAREST);
    
    return tex;
}




osg::ref_ptr<osg::TextureRectangle> ShadowGroup::createLightDirFluxTexture(int width, int height)
{
    osg::ref_ptr<osg::TextureRectangle> tex = new osg::TextureRectangle;
    tex->setTextureSize(width, height);
    
    tex->setSourceType(GL_FLOAT);
    tex->setSourceFormat(GL_RGBA);
    tex->setInternalFormat(GL_RGBA32F_ARB);
    
    // ! must use nearest, or flux will be interpolated, making flux sphere irregular
    tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::NEAREST);
    tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::NEAREST);
    
    return tex;
}

osg::ref_ptr<osg::TextureRectangle> ShadowGroup::createLightPositionTexture(int width, int height)
{
    osg::ref_ptr<osg::TextureRectangle> tex = new osg::TextureRectangle;
    tex->setTextureSize(width, height);
    
    tex->setSourceType(GL_FLOAT);
    tex->setSourceFormat(GL_RGBA);
    tex->setInternalFormat(GL_RGBA32F_ARB);
    tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::NEAREST);
    tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::NEAREST);
    
    return tex;
}

osg::ref_ptr<osg::Texture2D> ShadowGroup::createFluxMipmapTexture(int width, int height)
{
    osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;
    tex->setTextureSize(width, height);
    
    tex->setSourceType(GL_FLOAT);
    tex->setSourceFormat(GL_RGBA);
    tex->setInternalFormat(GL_RGBA8);
    tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
    tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
    
    return tex;
}

void ShadowGroup::setDepthMapResolution(float width, float height)
{
    _depthTexWidth = width;
    _depthTexHeight = height;
}

void ShadowGroup::addBasicShadowCam(osg::TextureRectangle *outDepthTex, osg::TextureRectangle *outFluxTex, osg::TextureRectangle *outPosTex, const osg::Matrixf &shadowMV, const osg::Matrixf &shadowMVP, DirectionalLight *dirLight)
{
    osg::ref_ptr<osg::Camera> cam(new osg::Camera);
    cam->setProjectionMatrix(_shadowProjection);
    
    osg::StateSet *ss = cam->getOrCreateStateSet();
    ss->setAttributeAndModes(_depthMapShader, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
    cam->setViewport(0, 0, _depthTexWidth, _depthTexHeight);
    cam->attach(osg::Camera::COLOR_BUFFER0, outDepthTex);
    
    if(_isGIEnabled)
    {
        cam->attach(osg::Camera::COLOR_BUFFER1, outFluxTex);
        cam->attach(osg::Camera::COLOR_BUFFER2, outPosTex);
    }
    
    cam->setClearColor(osg::Vec4(0, 0, 0, 1));
//    cam->setClearColor(osg::Vec4(0.5, 0, 1, 1));
    cam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    cam->setRenderOrder(osg::Camera::PRE_RENDER);
    cam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    cam->setViewMatrix(shadowMV);
   
    // 1) need to use osg::Matrixf as uniform, or invalid operation, since mat4 is float
    // 2) TODO: figure out why u_nearDistance cannot be passed to the shader
    // symptom: u_nearDistance seems to be "overriden" by the uniform of the same name defined in the geometry pass
    // Thus, for this reason, change u_nearDistance to u_nearDistance_s
    ss->addUniform(new osg::Uniform("u_nearDistance_s", _nearPlane));
    ss->addUniform(new osg::Uniform("u_farDistance_s", _farPlane));
    ss->addUniform(new osg::Uniform("u_lightViewMatrix", shadowMV));
    ss->addUniform(new osg::Uniform("u_lightViewProjectionMatrix", shadowMVP));
  
    ss->addUniform(new osg::Uniform("u_lightViewInverseMatrix", osg::Matrixf::inverse(shadowMV)));
    ss->addUniform(new osg::Uniform("u_lightProjectionMatrix", osg::Matrixf(_shadowProjection)));
    
    // if gi enabled TODO: add switches
    ss->addUniform(new osg::Uniform("u_lightPos", dirLight->getPosition()));
    
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

osg::ref_ptr<osg::TextureRectangle> ShadowGroup::getDirLightDirFluxTexture(int light_id)
{
    std::map<int, osg::ref_ptr<osg::TextureRectangle> >::iterator it = _dir_lightDir_fluxMaps.find(light_id);
    if(it != _dir_lightDir_fluxMaps.end())
    {
        return it->second;
    }
    else
    {
        return NULL;
    }
}

osg::ref_ptr<osg::TextureRectangle> ShadowGroup::getDirLightViewWorldPosTexture(int light_id)
{
    std::map<int, osg::ref_ptr<osg::TextureRectangle> >::iterator it = _dir_worldPos_Maps.find(light_id);
    if(it != _dir_worldPos_Maps.end())
    {
        return it->second;
    }
    else
    {
        return NULL;
    }
}


osg::ref_ptr<osg::Texture2D> ShadowGroup::getDirMipmapFluxTexture(int light_id)
{
    std::map<int, osg::ref_ptr<osg::Texture2D> >::iterator it = _dir_mipmapFluxMaps.find(light_id);
    if(it != _dir_mipmapFluxMaps.end())
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
        _dir_depthMaps.insert(std::make_pair(light_id, depthTex));
        if(_isGIEnabled)
        {
            osg::ref_ptr<osg::TextureRectangle> fluxTex = createLightDirFluxTexture(_depthTexWidth, _depthTexWidth);// TODO: optimization
            _dir_lightDir_fluxMaps.insert(std::make_pair(light_id, fluxTex));
            osg::ref_ptr<osg::TextureRectangle> posTex = createLightPositionTexture(_depthTexWidth, _depthTexWidth);
            _dir_worldPos_Maps.insert(std::make_pair(light_id, posTex));
            
            addBasicShadowCam(depthTex, fluxTex, posTex, shadowView, shadowMVP, dirLight);
        }
        else
        {
            addBasicShadowCam(depthTex, NULL, NULL, shadowView, shadowMVP, dirLight);
        }
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
