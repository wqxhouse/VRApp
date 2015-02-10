//
//  ImportanceSamplingPass.cpp
//  vrphysics
//
//  Created by Robin Wu on 1/24/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#include "ImportanceSamplingPass.h"
#include <osgDB/ReadFile>

#include "ShadowGroup.h"
#include "DirectionalLightGroup.h"

ImportanceSamplingPass::ImportanceSamplingPass(osg::Camera *mainCamera, ShadowGroup *sg, DirectionalLightGroup *dlg)
: ScreenPass(mainCamera), _shadowGroup(sg), _dirLightGroup(dlg)
{
    _splatsSize = 64;
    _rsmWidth = sg->getRsmWidth();
    _rsmHeight = sg->getRsmHeight();
    
    _mipMapCameraGroup = new osg::Group;
    _importanceSampleCameraGroup = new osg::Group;
    
    _importanceSamplingShaderId = addShader("importanceSampling.vert", "importanceSampling.frag");
    _mipMapShaderId = addShader("fluxMipmap.vert", "fluxMipmap.frag");
    
    loadPoissowTexture();
   
    // screen Quad needs to be created before attaching it to the cameras
    _screenQuad = createTexturedQuad();
    
    // enqueue flux images
    std::vector<int> dirLightIds = _dirLightGroup->getAllLightIds();
    
    // TODO: this is hard code. when light group changes, need a way to notify to this class
    // TODO: add omni/spot lights later
    for(int i = 0; i < dirLightIds.size(); i++)
    {
        osg::ref_ptr<osg::TextureRectangle> inFluxTex = _shadowGroup->getDirLightFluxTexture( dirLightIds[i] );
        int in_tex_id = addInTexture(inFluxTex);
        _in_flux_mipmap_ids.insert(std::make_pair(dirLightIds[i], in_tex_id));
        
        osg::ref_ptr<osg::Texture2D> impOutTex = createImportanceSampleOutTexture();
        _out_sample_textures.insert(std::make_pair(dirLightIds[i], impOutTex));
        
        
        osg::ref_ptr<osg::Texture2D> mipMapOut = createMipMapTexture();
        _mipMapOut = mipMapOut;
        
        addMipMapCamera(inFluxTex, mipMapOut);
        addImportanceSampleCamera(mipMapOut, impOutTex);
    }
    
    // order dependent
    _rootGroup->addChild(_mipMapCameraGroup);
    _rootGroup->addChild(_importanceSampleCameraGroup);
}

ImportanceSamplingPass::~ImportanceSamplingPass()
{
}

osg::ref_ptr<osg::Texture2D> ImportanceSamplingPass::createImportanceSampleOutTexture()
{
    osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;
    
    tex->setTextureSize(_splatsSize, _splatsSize);
    tex->setSourceType(GL_FLOAT);
    tex->setSourceFormat(GL_RGBA);
    tex->setInternalFormat(GL_RGBA32F_ARB);
    
    tex->setFilter(osg::TextureRectangle::MIN_FILTER,osg::TextureRectangle::LINEAR);
    tex->setFilter(osg::TextureRectangle::MAG_FILTER,osg::TextureRectangle::LINEAR);
    
    return tex;
}

osg::ref_ptr<osg::Texture2D> ImportanceSamplingPass::createMipMapTexture()
{
    osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;
    
    tex->setTextureSize(_rsmWidth, _rsmHeight);
    tex->setSourceType(GL_FLOAT);
    tex->setSourceFormat(GL_RGBA);
    tex->setInternalFormat(GL_RGB8);
    
    tex->setFilter(osg::TextureRectangle::MIN_FILTER,osg::TextureRectangle::NEAREST_MIPMAP_LINEAR);
    tex->setFilter(osg::TextureRectangle::MAG_FILTER,osg::TextureRectangle::LINEAR);
    tex->setWrap(osg::TextureRectangle::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::TextureRectangle::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    
    return tex;
}

void ImportanceSamplingPass::configureStateSet()
{
}

void ImportanceSamplingPass::configRTTCamera()
{
}

void ImportanceSamplingPass::addImportanceSampleCamera(osg::Texture2D *mipMapIn, osg::Texture2D *outTex)
{
    osg::Camera *cam = new osg::Camera;
    cam->addChild(_screenQuad);
    
    cam->setClearColor(osg::Vec4(1, 1, 1, 1));
    cam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    cam->setRenderOrder(osg::Camera::PRE_RENDER);
    
    cam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    cam->setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));
    
    cam->setViewport(0, 0, _splatsSize, _splatsSize);
    
    cam->attach(osg::Camera::COLOR_BUFFER0, outTex);
    _importanceSampleCameraGroup->addChild(cam);
    
    // set stateset
    osg::StateSet *impSS = cam->getOrCreateStateSet();
    impSS->setAttributeAndModes(getShader(_importanceSamplingShaderId), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    impSS->addUniform(new osg::Uniform("u_mipMapFlux", 0));
    impSS->addUniform(new osg::Uniform("u_mipMapSize", osg::Vec2(_rsmWidth, _rsmHeight)));
    impSS->addUniform(new osg::Uniform("u_splatSize", _splatsSize));
    impSS->addUniform(new osg::Uniform("u_poissowTex", 1));
    impSS->setTextureAttribute(0, mipMapIn);
    impSS->setTextureAttribute(1, _poissowTex);
}

void ImportanceSamplingPass::addMipMapCamera(osg::TextureRectangle *fluxMap, osg::Texture2D *mipTexOut)
{
    osg::Camera *mipMapCamera = new osg::Camera;
    mipMapCamera->addChild(_screenQuad);
   
    mipMapCamera->setClearColor(osg::Vec4());
    mipMapCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mipMapCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    mipMapCamera->setRenderOrder(osg::Camera::PRE_RENDER);
   
    mipMapCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    mipMapCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));
   
    mipMapCamera->setViewport(0, 0, _rsmWidth, _rsmHeight);
 
    mipMapCamera->attach(osg::Camera::COLOR_BUFFER0, mipTexOut);
    _mipMapCameraGroup->addChild(mipMapCamera);
   
    // set stateset
    osg::StateSet *mipSS = mipMapCamera->getOrCreateStateSet();
    mipSS->setAttributeAndModes(getShader(_mipMapShaderId), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    mipSS->addUniform(new osg::Uniform("u_rsmFlux", 0));
    mipSS->setTextureAttributeAndModes(0, fluxMap);
}

void ImportanceSamplingPass::loadPoissowTexture()
{
    _poissowTex = new osg::Texture2D;
    _poissowTex->setSourceFormat(GL_FLOAT);
    _poissowTex->setSourceType(GL_FLOAT);
    _poissowTex->setSourceFormat(GL_RGBA);
    _poissowTex->setInternalFormat(GL_RGBA32F_ARB);
    _poissowTex->setTextureSize(_splatsSize, _splatsSize);
  
    _poissowTex->setImage(osgDB::readImageFile("poissow.tga"));
    _poissowTex->setWrap(osg::TextureRectangle::WRAP_S, osg::TextureRectangle::REPEAT);
    _poissowTex->setWrap(osg::TextureRectangle::WRAP_T, osg::TextureRectangle::REPEAT);
    _poissowTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    _poissowTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
}

std::vector<int> ImportanceSamplingPass::getSampledLightIds()
{
    std::vector<int> res;
    res.resize(getNumOutImportanceSampleTextures());
    int count = 0;
    for(std::map<int, osg::ref_ptr<osg::Texture2D> >::iterator it = _out_sample_textures.begin(); it != _out_sample_textures.end(); ++it)
    {
        res[count] = it->first;
        count++;
    }
    return res;
}