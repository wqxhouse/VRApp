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
#include <osg/CullFace>

#include <osg/LightSource>

#include "ShadowCallback.h"

ShadowGroup::ShadowGroup(osg::Camera *mainCamera, osg::Group *geoms, const osg::BoundingBox &sceneAABB)
: _mainCamera(mainCamera), _geoms(geoms), _giLight(NULL), _sceneAABB(sceneAABB)
{
    _isGIEnabled = true;
    
    // dummy
    _nearPlane = -10;
    _farPlane = 20;
    _shadowProjection.makeOrtho(-5, 5, -5, 5, _nearPlane, _farPlane);
    
    _depthMapShader = new osg::Program();
    _depthMapShader->addShader(osgDB::readShaderFile("orthoDepthMap.vert"));
    _depthMapShader->addShader(osgDB::readShaderFile("orthoDepthMap.frag"));
    _depthTexWidth = 1024;
    _depthTexHeight = 1024;
    
    _blurShaderX = new osg::Program();
    _blurShaderX->addShader(osgDB::readShaderFile("gBlur.vert"));
    _blurShaderX->addShader(osgDB::readShaderFile("gBlurX.frag"));
   
    _blurShaderY = new osg::Program();
    _blurShaderY->addShader(osgDB::readShaderFile("gBlur.vert"));
    _blurShaderY->addShader(osgDB::readShaderFile("gBlurY.frag"));
    
    _rsmShader = new osg::Program();
    _rsmShader->addShader(osgDB::readShaderFile("rsm.vert"));
    _rsmShader->addShader(osgDB::readShaderFile("rsm.frag"));
    
    _rsmTexWidth = 512;
    _rsmTexHeight = 512;
    
    _shadowCameras = new osg::Group;
    _blurCameras = new osg::Group;
    _shadowRootGroup = new osg::Group;
    
    _rsmCam = new osg::Camera;
    
    _shadowRootGroup->addChild(_shadowCameras);
    _shadowRootGroup->addChild(_blurCameras);
    _shadowRootGroup->addChild(_rsmCam);
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

void ShadowGroup::addBlurCamera(osg::TextureRectangle *outDepthTex)
{
    osg::ref_ptr<osg::Camera> blurCamera(new osg::Camera);
    blurCamera->setClearColor(osg::Vec4());
    blurCamera->setClearMask(GL_DEPTH_BUFFER_BIT); // we cannot clear color buffer
    
    blurCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    blurCamera->setRenderOrder(osg::Camera::PRE_RENDER);
   
    blurCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    blurCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));
    blurCamera->setViewMatrix(osg::Matrix::identity());
    blurCamera->setViewport(0, 0, _depthTexWidth, _depthTexHeight);
    blurCamera->attach(osg::Camera::COLOR_BUFFER0, outDepthTex);
    
    blurCamera->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    
    osg::ref_ptr<osg::Group> yDir = createQuad();
    osg::ref_ptr<osg::Group> xDir = createQuad();
    
    // create two screen Quad for two blurring direction
    configBlurQuadStateSet(yDir, 'y', outDepthTex);
    configBlurQuadStateSet(xDir, 'x', outDepthTex);
    blurCamera->addChild(yDir);
    blurCamera->addChild(xDir);
    
    _blurCameras->addChild(blurCamera);
}


void ShadowGroup::configBlurQuadStateSet(osg::Group *g, char dir, osg::TextureRectangle *outDepthTex)
{
    osg::ref_ptr<osg::StateSet> ss = g->getOrCreateStateSet();
    if(dir == 'y')
    {
        ss->setAttributeAndModes(_blurShaderY, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    }
    else if(dir == 'x')
    {
        ss->setAttributeAndModes(_blurShaderX, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    }
    else
    {
        fprintf(stderr, "Shadow Group, blur dir err\n");
        exit(0);
    }
    ss->addUniform(new osg::Uniform("u_texture", 0));
    ss->setTextureAttribute(0, outDepthTex);
}

void ShadowGroup::configRSMCamera()
{
    osg::ref_ptr<osg::StateSet> ss = _rsmCam->getOrCreateStateSet();
    ss->setAttributeAndModes(_rsmShader, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
    _rsmCam->setViewport(0, 0, _rsmTexWidth, _rsmTexHeight);
    
    // TODO: refactor
    _rsmCam->attach(osg::Camera::COLOR_BUFFER0, getDirLightDirFluxTexture(0));
    _rsmCam->attach(osg::Camera::COLOR_BUFFER1, getDirLightViewWorldPosTexture(0));
    
    _rsmCam->setClearColor(osg::Vec4(0, 0, 0, 1));
    _rsmCam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _rsmCam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    _rsmCam->setRenderOrder(osg::Camera::PRE_RENDER);
    _rsmCam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    
    ss->addUniform(new osg::Uniform("u_lightViewInverseMatrix", osg::Matrixf()));
    ss->addUniform(new osg::Uniform("u_lightPos", osg::Vec3()));
    
    ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
    osg::ref_ptr<osg::CullFace> cullFace(new osg::CullFace);
    cullFace->setMode(osg::CullFace::BACK);
    ss->setAttribute(cullFace);
    
    osg::Vec2 depthTexSize(_depthTexWidth, _depthTexHeight);
    osg::ref_ptr<RSMCallback> rsmCallback(new RSMCallback(_mainCamera, _rsmCam, osg::Vec2(_rsmTexWidth, _rsmTexHeight), _sceneAABB));
    rsmCallback->setDirectionalLight(_giLight);
    ss->setUpdateCallback(rsmCallback);
    
    _rsmCam->addChild(_geoms);
}

void ShadowGroup::addBasicShadowCam(osg::TextureRectangle *outDepthTex, osg::TextureRectangle *outFluxTex, osg::TextureRectangle *outPosTex, const osg::Matrixf &shadowMV, const osg::Matrixf &shadowMVP, DirectionalLight *dirLight)
{
    osg::ref_ptr<osg::Camera> cam(new osg::Camera);
    cam->setProjectionMatrix(_shadowProjection);
    
    osg::ref_ptr<osg::StateSet> ss = cam->getOrCreateStateSet();
    ss->setAttributeAndModes(_depthMapShader, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
    cam->setViewport(0, 0, _depthTexWidth, _depthTexHeight);
    cam->attach(osg::Camera::COLOR_BUFFER0, outDepthTex);
    
//    if(_isGIEnabled)
//    {
//        cam->attach(osg::Camera::COLOR_BUFFER1, outFluxTex);
//        cam->attach(osg::Camera::COLOR_BUFFER2, outPosTex);
//    }
    
    cam->setClearColor(osg::Vec4(0, 0, 0, 1));
    cam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    cam->setRenderOrder(osg::Camera::PRE_RENDER);
    cam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    cam->setViewMatrix(shadowMV);
   
    // 1) need to use osg::Matrixf as uniform, or invalid operation, since mat4 is float
    // 2) TODO: figure out why u_nearDistance cannot be passed to the shader
    // symptom: u_nearDistance seems to be "overriden" by the uniform of the same name defined in the geometry pass
    // Thus, for this reason, change u_nearDistance to u_nearDistance_s

    // to be modified in the callback
    ss->addUniform(new osg::Uniform("u_nearDistance_s", 0.0f));
    ss->addUniform(new osg::Uniform("u_zLength", 0.0f));
    
    ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
    osg::ref_ptr<osg::CullFace> cullFace(new osg::CullFace);
    cullFace->setMode(osg::CullFace::BACK);
    ss->setAttribute(cullFace);
   
    osg::Vec2 depthTexSize(_depthTexWidth, _depthTexHeight);
    osg::ref_ptr<ShadowCallback> shadowCallback(new ShadowCallback(_mainCamera, cam, depthTexSize));
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
            // make the only the first light gi light
            if(_dir_lightDir_fluxMaps.empty())
            {
                osg::ref_ptr<osg::TextureRectangle> fluxTex = createLightDirFluxTexture(_rsmTexWidth, _rsmTexHeight);// TODO: optimization
                _dir_lightDir_fluxMaps.insert(std::make_pair(light_id, fluxTex));
                osg::ref_ptr<osg::TextureRectangle> posTex = createLightPositionTexture(_rsmTexWidth, _rsmTexHeight);
                _dir_worldPos_Maps.insert(std::make_pair(light_id, posTex));
                
                _giLight = dirLight;
                configRSMCamera();
            }
            
            // addBasicShadowCam(depthTex, fluxTex, posTex, shadowView, shadowMVP, dirLight);
            
            // TODO: refactor
            addBasicShadowCam(depthTex, NULL, NULL, shadowView, shadowMVP, dirLight);
            addBlurCamera(depthTex);
        }
        else
        {
            addBasicShadowCam(depthTex, NULL, NULL, shadowView, shadowMVP, dirLight);
            addBlurCamera(depthTex);
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

osg::ref_ptr<osg::Group> ShadowGroup::createQuad()
{
    osg::ref_ptr<osg::Group> top_group = new osg::Group;
    osg::ref_ptr<osg::Geode> quad_geode = new osg::Geode;
    
    osg::ref_ptr<osg::Vec3Array> quad_coords = new osg::Vec3Array; // vertex coords
    // counter-clockwise
    // TODO: config for calvr 
    quad_coords->push_back(osg::Vec3d(0, 0, -1));
    quad_coords->push_back(osg::Vec3d(1, 0, -1));
    quad_coords->push_back(osg::Vec3d(1, 1, -1));
    quad_coords->push_back(osg::Vec3d(0, 1, -1));
    
    osg::ref_ptr<osg::Geometry> quad_geom = new osg::Geometry;
    osg::ref_ptr<osg::DrawArrays> quad_da = new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4);
    
    quad_geom->setVertexArray(quad_coords.get());
    quad_geom->addPrimitiveSet(quad_da.get());
    
    quad_geode->addDrawable(quad_geom);
    top_group->addChild(quad_geode);
    return top_group;
}
