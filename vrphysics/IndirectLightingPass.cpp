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

IndirectLightingPass::IndirectLightingPass(osg::Camera *mainCamera, ImportanceSamplingPass *impPass, osg::TextureRectangle *worldPosTex, osg::TextureRectangle *lightDirTex)
: ScreenPass(mainCamera), _importanceSamplingEnabled(true), _impPass(impPass)
{
    // load sample tex
    std::vector<int> sampledLightIds = impPass->getSampledLightIds();
    int numImpTex = impPass->getNumOutImportanceSampleTextures();
    for(int i = 0; i < numImpTex; i++)
    {
        osg::ref_ptr<osg::Texture2D> impTex = impPass->getImportanceSampleTexture(sampledLightIds[i]);
        _importanceSampleTextures.insert(std::make_pair(sampledLightIds[i], impTex));
    }
    
    // normal sample pattern, used when importance sampling is disabled
    _sampleTex = _impPass->getPossiowTexture();
    
    _in_lightDirTex_id = addInTexture(lightDirTex);
    _in_worldPosTex_id = addInTexture(worldPosTex);
    
    _out_indirectLightingTex_id = addOutTexture();
    
    _indirectLightShader = addShader("indirectLighting.vert", "indirectLighting.frag");
    
    _vplGroup = new VPLGroup;
    _rttCamera->addChild(_vplGroup->getLightSphereGeode());
    _rootGroup->addChild(_rttCamera);
}

IndirectLightingPass::~IndirectLightingPass()
{
    delete _vplGroup;
}

int IndirectLightingPass::addOutTexture()
{
    osg::ref_ptr<osg::TextureRectangle> tex = new osg::TextureRectangle;
    
    tex->setTextureSize(_screenWidth, _screenHeight);
    tex->setSourceType(GL_UNSIGNED_BYTE); // TODO: consider HDR cases
    tex->setSourceFormat(GL_RGBA);
    tex->setInternalFormat(GL_RGBA);
    
    tex->setFilter(osg::TextureRectangle::MIN_FILTER,osg::TextureRectangle::LINEAR);
    tex->setFilter(osg::TextureRectangle::MAG_FILTER,osg::TextureRectangle::LINEAR);
    
    _screenOutTexture.push_back(tex);
    
    return (int)_screenOutTexture.size() - 1;
}

void IndirectLightingPass::configureStateSet()
{
    _stateSet = _rttCamera->getOrCreateStateSet();
    _stateSet->setAttributeAndModes(getShader(_indirectLightShader), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    
}