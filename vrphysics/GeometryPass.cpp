//
//  GeometryPass.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/11/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//

#include "GeometryPass.h"
#include <osg/Depth>

// protected
GeometryPass::GeometryPass(osg::Camera *mainCamera, osg::TextureRectangle *defaultTexture, osg::Node *worldObjects)
: ScreenPass(mainCamera)
{
    osg::Matrix projMatrix = mainCamera->getProjectionMatrix();
    float dummy;
    projMatrix.getFrustum(dummy, dummy, dummy, dummy, _nearPlaneDist, _farPlaneDist);
    
    _defaultTexture = defaultTexture;
    _worldObjects = worldObjects;
 
    ScreenPass::setShader("gbuffer.vert", "gbuffer.frag");
    ScreenPass::setupCamera();
    // TODO: figure out why higher prcision textures doesn't work
//    _out_albedo_tex_id = ScreenPass::addOutTexture();
    _out_albedo_tex_id = addOutTexture(false);
//    _out_normal_depth_tex_id = ScreenPass::addOutTexture();
    _out_normal_depth_tex_id = addOutTexture(true);
    _out_position_tex_id = addOutTexture(true);
    
    _stateSet = _rttCamera->getOrCreateStateSet(); // important, at the geom pass, shader is bind to the camera
    _rttCamera->attach(osg::Camera::COLOR_BUFFER0, getAlbedoOutTexture());
    _rttCamera->attach(osg::Camera::COLOR_BUFFER1, getNormalDepthOutTexture());
    _rttCamera->attach(osg::Camera::COLOR_BUFFER2, getPositionOutTexure());
    
    osg::ref_ptr<osg::Group> worldObjectGeomPassNode(new osg::Group);
    worldObjectGeomPassNode->addChild(worldObjects);
    auto worldObjectGeomPassSS = worldObjectGeomPassNode->getOrCreateStateSet();
    worldObjectGeomPassSS->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
    osg::ref_ptr<osg::Depth> depth(new osg::Depth);
    depth->setWriteMask(true);
    worldObjectGeomPassSS->setAttributeAndModes(depth, osg::StateAttribute::ON);
    
    _rttCamera->addChild(worldObjectGeomPassNode);
    
    _rootGroup->addChild(_rttCamera);
    configureStateSet();
}

GeometryPass::~GeometryPass()
{
}

void GeometryPass::configureStateSet()
{
    _stateSet->setAttributeAndModes(_shaderProgram, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    _stateSet->setTextureAttributeAndModes(0, _defaultTexture, osg::StateAttribute::ON);
    
    _stateSet->addUniform(new osg::Uniform("u_texture", 0));
    _stateSet->addUniform(new osg::Uniform("u_farDistance", _farPlaneDist));
}

int GeometryPass::addOutTexture(bool isDepth)
{
    osg::ref_ptr<osg::TextureRectangle> tex = new osg::TextureRectangle;
    
    tex->setTextureSize(_screenWidth, _screenHeight);
    tex->setSourceType(GL_FLOAT);
    tex->setSourceFormat(GL_RGBA);
    if(isDepth)
    {
        tex->setInternalFormat(GL_RGBA32F_ARB);
    }
    else
    {
        tex->setInternalFormat(GL_RGBA16F_ARB);
    }
    
    tex->setFilter(osg::TextureRectangle::MIN_FILTER,osg::TextureRectangle::LINEAR);
    tex->setFilter(osg::TextureRectangle::MAG_FILTER,osg::TextureRectangle::LINEAR);
    tex->setWrap(osg::TextureRectangle::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::TextureRectangle::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    
    _screenOutTexture.push_back(tex);
    
    return (int)_screenOutTexture.size() - 1;
}

