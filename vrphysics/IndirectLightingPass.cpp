//
//  IndirectLightingPass.cpp
//  vrphysics
//
//  Created by Robin Wu on 2/6/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#include "IndirectLightingPass.h"
#include "ImportanceSamplingPass.h"
#include "VPLGroup.h"
#include <osgDB/WriteFile>

#include "DirectionalLight.h"

IndirectLightingPass::IndirectLightingPass(osg::Camera *mainCamera, ImportanceSamplingPass *impPass, osg::TextureRectangle *lightDirTex, osg::TextureRectangle *worldPosTex, osg::TextureRectangle *viewPositionTex, osg::TextureRectangle *viewNormalTex, osg::Texture2D *depthBufferTex, DirectionalLight *mainLight, osg::Texture2D *sharedDepthBuffer)
: ScreenPass(mainCamera), _importanceSamplingEnabled(true), _impPass(impPass)
{
    // load sample tex
    // TODO: support multiple lights later
//    std::vector<int> sampledLightIds = impPass->getSampledLightIds();
//    int numImpTex = impPass->getNumOutImportanceSampleTextures();
//    for(int i = 0; i < numImpTex; i++)
//    {
//        osg::ref_ptr<osg::Texture2D> impTex = impPass->getImportanceSampleTexture(sampledLightIds[i]);
//        _importanceSampleTextures.insert(std::make_pair(sampledLightIds[i], impTex));
//    }
   
    _mainLight = mainLight;
   
    _sharedDepthBufferTex = sharedDepthBuffer;
    _impSampleTex = impPass->getImportanceSampleTexture(0);
    _depthBufferTex = depthBufferTex;
    _splats = impPass->getSplatsSizeRow();
    _nSplats = _splats * _splats;
    _rsmWidth = _impPass->getRSMWidth();
    _rsmHeight = _impPass->getRSMHeight();
    
    _splatSampleNum = 32;
    createSampleTexcoordMap();
    
    // normal sample pattern, used when importance sampling is disabled
    _sampleTex = _impPass->getPossiowTexture();
    
    _in_lightDirTex_id = addInTexture(lightDirTex);
    _in_worldPosTex_id = addInTexture(worldPosTex);
    _in_viewPositionTex_id = addInTexture(viewPositionTex);
    _in_viewNormalTex_id = addInTexture(viewNormalTex);
    
    _out_indirectLightingTex_id = addOutTexture();
    
    _indirectLightShader = addShader("indirectLighting.vert", "indirectLighting.frag");
    
    _vplGroup = new VPLGroup;
    _vplGroup->setNumVpls(_splatSampleNum * _splatSampleNum);
    
    setupCamera();
    
    configureStateSet();
}

void IndirectLightingPass::setupCamera()
{
    _rttCamera->addChild(_vplGroup->getLightSphereGeode());
    //     _rttCamera->addChild(createTexturedQuad());
    _rootGroup->addChild(_rttCamera);
    
    // config camera
    // config shared depth buffer and stencil
    _rttCamera->setClearStencil(0);
    _rttCamera->setClearMask(GL_COLOR_BUFFER_BIT);
//    _rttCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//    _rttCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _rttCamera->attach(osg::Camera::PACKED_DEPTH_STENCIL_BUFFER, _sharedDepthBufferTex);
    
    _rttCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f,1.0f));
    
    // viewport
    _rttCamera->setViewport(0, 0, _screenWidth, _screenHeight);
    _rttCamera->setRenderOrder(osg::Camera::PRE_RENDER);
    _rttCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    
    _rttCamera->attach(osg::Camera::COLOR_BUFFER0, getOutputTexture(_out_indirectLightingTex_id));
}

IndirectLightingPass::~IndirectLightingPass()
{
    delete _vplGroup;
}

// create a 1 x #instance texture for texcoord
void IndirectLightingPass::createSampleTexcoordMap()
{
    _sampleTexcoordMap = new osg::TextureRectangle();
    _sampleTexcoordMap->setTextureSize(_splats, _splats);
    _sampleTexcoordMap->setInternalFormat(GL_RGB16);
    _sampleTexcoordMap->setSourceFormat(GL_RGB);
    _sampleTexcoordMap->setSourceType(GL_FLOAT);
    _sampleTexcoordMap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
    _sampleTexcoordMap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
    _sampleTexcoordMap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    _sampleTexcoordMap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
   
    osg::ref_ptr<osg::Image> image = new osg::Image;
    image->allocateImage(_nSplats, 1, 1, GL_RGB, GL_FLOAT);
    image->setInternalTextureFormat(GL_RGB16);
    _sampleTexcoordMap->setImage(image);
    
    // TODO: find way to make the image only use red and green channel for space
    // currently GL_RG16 doesn't work
    //osg::Vec4* dataPtr = (osg::Vec4*)image->data();
    
    // allocate an image with the texture coordinate
    for ( int j = 0; j < _splats; j++ )
    {
        for ( int i = 0; i < _splats; i++ )
        {
            osg::Vec3 v(
                ((float)i + 0.5f) / (float)_splats,
                ((float)j + 0.5f) / (float)_splats,
                0.0f);
            int index = j * _splats + i;
            osg::Vec3* data = (osg::Vec3*)image->data(index, 0);
            *data = v; // be careful of the boundary, very easy to corrupt memory.
        }
    }
}

int IndirectLightingPass::addOutTexture()
{
    osg::ref_ptr<osg::TextureRectangle> tex = new osg::TextureRectangle;
    
    tex->setTextureSize(_screenWidth, _screenHeight);
//    tex->setSourceType(GL_UNSIGNED_BYTE); // TODO: consider HDR cases
//    tex->setSourceFormat(GL_RGBA);
//    tex->setInternalFormat(GL_RGBA);
    
    tex->setSourceType(GL_FLOAT); // TODO: consider HDR cases
    tex->setSourceFormat(GL_RGBA);
    tex->setInternalFormat(GL_RGBA16F_ARB);
    
    
    tex->setFilter(osg::TextureRectangle::MIN_FILTER,osg::TextureRectangle::LINEAR);
    tex->setFilter(osg::TextureRectangle::MAG_FILTER,osg::TextureRectangle::LINEAR);
    
    _screenOutTexture.push_back(tex);
    
    return (int)_screenOutTexture.size() - 1;
}

void IndirectLightingPass::configureStateSet()
{
    _stateSet = _rttCamera->getOrCreateStateSet();
    _stateSet->setAttributeAndModes(getShader(_indirectLightShader), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    
    _stateSet->addUniform(new osg::Uniform("u_samplePosTex", 0));
    if(_impPass->isImportanceSampleEnabled())
    {
        _stateSet->setTextureAttributeAndModes(0, _impSampleTex);
    }
    else
    {
        _stateSet->setTextureAttributeAndModes(0, _sampleTex);
    }
    
    _stateSet->addUniform(new osg::Uniform("u_worldPosTex", 1));
    _stateSet->addUniform(new osg::Uniform("u_lightDirTex", 2));
    _stateSet->setTextureAttributeAndModes(1, getInTexture(_in_worldPosTex_id));
    _stateSet->setTextureAttributeAndModes(2, getInTexture(_in_lightDirTex_id));
    
    _stateSet->addUniform(new osg::Uniform("u_matVP", osg::Matrixf(_mainCamera->getViewMatrix() * _mainCamera->getProjectionMatrix())));
    _stateSet->addUniform(new osg::Uniform("u_viewInverseMatrix", osg::Matrixf(_mainCamera->getInverseViewMatrix())));
    
    // hard coded defaults
    float indirectFactor = 0.125f;
    float indirectSample = _splatSampleNum * _splatSampleNum;
    float indirectScale = indirectFactor * 256.0f / (float)indirectSample;
    
    _stateSet->addUniform(new osg::Uniform("u_scaleIndirect", indirectScale));
    _stateSet->addUniform(new osg::Uniform("u_rsm_wh", osg::Vec2(_rsmWidth, _rsmHeight)));
    _stateSet->addUniform(new osg::Uniform("u_render_wh", osg::Vec2(_screenWidth, _screenHeight)));
    _stateSet->addUniform(new osg::Uniform("u_render_wh_inv", osg::Vec2(1.0f / _screenWidth, 1.0f / _screenHeight)));
    _stateSet->addUniform(new osg::Uniform("u_camPos", getCameraPosition()));
    
    _stateSet->addUniform(new osg::Uniform("u_sampleTexcoordTex", 3));
    _stateSet->setTextureAttributeAndModes(3, _sampleTexcoordMap);
    
    _stateSet->addUniform(new osg::Uniform("u_viewPositionTex", 4));
    _stateSet->addUniform(new osg::Uniform("u_viewNormalTex", 5));
    _stateSet->setTextureAttributeAndModes(4, getInTexture(_in_viewPositionTex_id));
    _stateSet->setTextureAttributeAndModes(5, getInTexture(_in_viewNormalTex_id));
   
    if(_mainLight != NULL) // TODO: refactor
    {
        _stateSet->addUniform(new osg::Uniform("u_mainLightWorldPos", _mainLight->getPosition()));
    }
    
//    osg::ref_ptr<osg::Uniform> a = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "u_arr", 256);
//    for(int i = 0; i < _splatSampleNum; ++i)
//    {
//        for(int j = 0; j < _splatSampleNum; ++j)
//        {
//            float x = -(float)_splatSampleNum + j * 2;
//            float z = -(float)_splatSampleNum + i * 2;
//            float y = 0.0f;
//            
//            std::cout << x << " " << z << std::endl;
//            osg::Vec3 p = osg::Vec3(x, y, z);
//            
//            int index = i * _splatSampleNum + j;
//            a->setElement(index, p);
//        }
//    }
//    _stateSet->addUniform(a);
    
    _stateSet->setUpdateCallback(new IndirectLightingCallback(_mainCamera, _mainLight));
}

IndirectLightingCallback::IndirectLightingCallback(osg::Camera *mainCamera, DirectionalLight *mainLight)
: _mainCamera(mainCamera), _mainLight(mainLight)
{
}

void IndirectLightingCallback::operator()(osg::StateSet *ss, osg::NodeVisitor *nv)
{
    ss->getUniform("u_camPos")->set(getCameraPosition());
    // TODO: add switches to enable/disable gi
    ss->getUniform("u_matVP")->set(osg::Matrixf(_mainCamera->getViewMatrix() * _mainCamera->getProjectionMatrix()));
    ss->getUniform("u_viewInverseMatrix")->set(osg::Matrixf(_mainCamera->getInverseViewMatrix()));
    
    if(_mainLight != NULL) // TODO: refactor
    {
        ss->getUniform("u_mainLightWorldPos")->set(_mainLight->getPosition());
    }
}

// TODO: refactor
osg::Vec3 IndirectLightingCallback::getCameraPosition()
{
    osg::Matrix &mat = _mainCamera->getViewMatrix();
    osg::Vec3 eye;
    osg::Vec3 dummy;
    mat.getLookAt(eye, dummy, dummy);
    
    // printf("%f, %f, %f\n", eye.x(), eye.y(), eye.z());
    
    return eye;
}

osg::Vec3 IndirectLightingPass::getCameraPosition()
{
    osg::Matrix &mat = _mainCamera->getViewMatrix();
    osg::Vec3 eye;
    osg::Vec3 dummy;
    mat.getLookAt(eye, dummy, dummy);
    
    return eye;
}
