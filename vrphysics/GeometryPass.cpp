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
GeometryPass::GeometryPass(osg::Camera *mainCamera, AssetDB *assetDB)
: ScreenPass(mainCamera), _assetDB(assetDB)
{
    osg::Matrix projMatrix = mainCamera->getProjectionMatrix();
    float dummy;
    projMatrix.getFrustum(dummy, dummy, dummy, dummy, _nearPlaneDist, _farPlaneDist);
    
    _worldObjects = _assetDB->getGeomRoot();
    
    //ScreenPass::setShader("gbuffer.vert", "gbuffer.frag");
    _gbuffer_notex_shader = addShader("gbuffer.vert", "gbuffer.frag");
    _gbuffer_tex_shader = addShader("gbuffertex.vert", "gbuffertex.frag");
    
    ScreenPass::setupCamera();
    _out_albedo_tex_id = addOutTexture(false);
    _out_normal_depth_tex_id = addOutTexture(true);
    _out_position_tex_id = addOutTexture(true);
    
    _stateSet = _rttCamera->getOrCreateStateSet(); // important, at the geom pass, shader is bind to the camera
    _rttCamera->attach(osg::Camera::COLOR_BUFFER0, getAlbedoOutTexture());
    _rttCamera->attach(osg::Camera::COLOR_BUFFER1, getNormalDepthOutTexture());
    _rttCamera->attach(osg::Camera::COLOR_BUFFER2, getPositionOutTexure());
   
    configSharedDepthStencilTexture();
    _rttCamera->attach(osg::Camera::PACKED_DEPTH_STENCIL_BUFFER, _sharedDepthStencilTex);
    
    osg::ref_ptr<osg::Group> worldObjectGeomPassNode(new osg::Group);
    worldObjectGeomPassNode->addChild(_worldObjects);
    auto worldObjectGeomPassSS = worldObjectGeomPassNode->getOrCreateStateSet();
    worldObjectGeomPassSS->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
    osg::ref_ptr<osg::Depth> depth(new osg::Depth);
    depth->setWriteMask(true);
//    depth->setZNear(0.0);
//    depth->setZFar(1.0);
    worldObjectGeomPassSS->setAttributeAndModes(depth, osg::StateAttribute::ON);
    
    _rttCamera->addChild(worldObjectGeomPassNode);
    
    _rootGroup->addChild(_rttCamera);
    configureStateSet();
}

GeometryPass::~GeometryPass()
{
}

void GeometryPass::configSharedDepthStencilTexture()
{
    _sharedDepthStencilTex = new osg::Texture2D;
    _sharedDepthStencilTex->setTextureSize(_screenWidth, _screenHeight);
    _sharedDepthStencilTex->setInternalFormat(GL_DEPTH32F_STENCIL8);
    _sharedDepthStencilTex->setSourceFormat(GL_DEPTH_STENCIL);
    _sharedDepthStencilTex->setSourceType(GL_FLOAT_32_UNSIGNED_INT_24_8_REV);
}

void GeometryPass::configureStateSet()
{
    //    _stateSet->setAttributeAndModes(getShader(_gbuffer_notex_shader), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    _stateSet->setAttributeAndModes(getShader(_gbuffer_notex_shader), osg::StateAttribute::ON );
    _stateSet->addUniform(new osg::Uniform("u_nearDistance", _nearPlaneDist));
    _stateSet->addUniform(new osg::Uniform("u_farDistance", _farPlaneDist));
    osg::ref_ptr<GeometryPassCallback> geomPassCallback(new GeometryPassCallback(_mainCamera));
    _stateSet->setUpdateCallback(geomPassCallback);
    
    _stateSet->addUniform(new osg::Uniform("u_inv_viewMat", osg::Matrixf(_mainCamera->getInverseViewMatrix())));
    _stateSet->addUniform(new osg::Uniform("u_viewMat", osg::Matrixf(_mainCamera->getViewMatrix())));
    _stateSet->addUniform(new osg::Uniform("u_projMat", osg::Matrixf(_mainCamera->getProjectionMatrix())));
    
    // attach shader to objects other than light geom
    // TODO: consider the shading of the flying light object, currently overriden by _stateSet
    auto nodeMaterialPairs = _assetDB->getGeometryNodesAndMaterials();
    for(unsigned long i = 0; i < nodeMaterialPairs.size(); i++)
    {
        auto p = nodeMaterialPairs[i];
        osg::Node *node = p.first;
        Material *mat = p.second;
        if(mat->hasTexture())
        {
            osg::StateSet *ss = node->getOrCreateStateSet();
            // TODO: figure out why must protected here --> b/c _stateSet is overriding
            //ss->setAttributeAndModes(getShader(_gbuffer_tex_shader), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE);
            ss->setAttributeAndModes(getShader(_gbuffer_tex_shader), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
            osg::Texture2D *tex = mat->getAlbedoTexture();
            ss->addUniform(new osg::Uniform ("u_nearDistance", _nearPlaneDist));
            ss->addUniform(new osg::Uniform("u_farDistance", _farPlaneDist));
            ss->addUniform(new osg::Uniform("u_texture", 0));
            ss->setTextureAttribute(0, tex);
            
            ss->setUpdateCallback(geomPassCallback);
            
            //            auto d = node->asTransform()->getChild(0)->asGeode()->getDrawable(0);
            //            auto g = d->asGeometry();
            //            auto arr = g->getTexCoordArrayList();
            ss->addUniform(new osg::Uniform("u_inv_viewMat", osg::Matrixf(_mainCamera->getInverseViewMatrix())));
            ss->addUniform(new osg::Uniform("u_viewMat", osg::Matrixf(_mainCamera->getViewMatrix())));
            ss->addUniform(new osg::Uniform("u_projMat", osg::Matrixf(_mainCamera->getProjectionMatrix())));
        }
        else
        {
            osg::StateSet *ss = node->getOrCreateStateSet();
            //ss->setAttributeAndModes(getShader(_gbuffer_notex_shader), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE );
            ss->setAttributeAndModes(getShader(_gbuffer_notex_shader), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
            ss->addUniform(new osg::Uniform("u_nearDistance", _nearPlaneDist));
            ss->addUniform(new osg::Uniform("u_farDistance", _farPlaneDist));
            
            ss->setUpdateCallback(geomPassCallback);
            
            ss->addUniform(new osg::Uniform("u_inv_viewMat", osg::Matrixf(_mainCamera->getInverseViewMatrix())));
            ss->addUniform(new osg::Uniform("u_viewMat", osg::Matrixf(_mainCamera->getViewMatrix())));
            ss->addUniform(new osg::Uniform("u_projMat", osg::Matrixf(_mainCamera->getProjectionMatrix())));
            
        }
    }
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
        tex->setInternalFormat(GL_RGBA8);
    }
    
    tex->setFilter(osg::TextureRectangle::MIN_FILTER,osg::TextureRectangle::NEAREST);
    tex->setFilter(osg::TextureRectangle::MAG_FILTER,osg::TextureRectangle::NEAREST);
    tex->setWrap(osg::TextureRectangle::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::TextureRectangle::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    
    _screenOutTexture.push_back(tex);
    
    return (int)_screenOutTexture.size() - 1;
}

GeometryPassCallback::GeometryPassCallback(osg::Camera *camera)
: _mainCamera(camera)
{
}

GeometryPassCallback::~GeometryPassCallback()
{
}

void GeometryPassCallback::operator()(osg::StateSet *ss, osg::NodeVisitor *visitor)
{
    getNearFarPlane();
    ss->getUniform("u_nearDistance")->set(_nearPlane);
    ss->getUniform("u_farDistance")->set(_farPlane);
    
    ss->getUniform("u_viewMat")->set(osg::Matrixf(_mainCamera->getViewMatrix()));
    ss->getUniform("u_inv_viewMat")->set(osg::Matrixf(_mainCamera->getInverseViewMatrix()));
    ss->getUniform("u_projMat")->set(osg::Matrixf(_mainCamera->getProjectionMatrix()));
}

void GeometryPassCallback::getNearFarPlane()
{
    double dummy;
    double nearPlane;
    double farPlane;
    const osg::Matrix &projectionMatrix = _mainCamera->getProjectionMatrix();
    projectionMatrix.getFrustum(dummy, dummy, dummy, dummy, nearPlane, farPlane);
    _nearPlane = nearPlane;
    _farPlane = farPlane;
}