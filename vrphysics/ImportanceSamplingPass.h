//
//  ImportanceSamplingPass.h
//  vrphysics
//
//  Created by Robin Wu on 1/24/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#ifndef __vrphysics__ImportanceSamplingPass__
#define __vrphysics__ImportanceSamplingPass__

#include <stdio.h>
#include <osg/Texture2D>

#include "ScreenPass.h"
#include "DirectionalLightGroup.h"

class ShadowGroup;
class DirectionalLightGroup;
class ImportanceSamplingPass : public ScreenPass
{
public:
    ImportanceSamplingPass(osg::Camera *mainCamera, ShadowGroup *sg, DirectionalLightGroup *dlg);
    virtual ~ImportanceSamplingPass();
    
    inline osg::ref_ptr<osg::Texture2D> getImportanceSampleTexture(int light_id)
    {
        std::map<int, osg::ref_ptr<osg::Texture2D> >::iterator it;
        osg::ref_ptr<osg::Texture2D> tex = ((it = _out_sample_textures.find(light_id)) != _out_sample_textures.end()) ? it->second : NULL;
        
        return tex;
    }
    
    // TODO: experimental
    inline osg::ref_ptr<osg::Texture2D> getFluxMipMapTexture()
    {
        return _mipMapOut;
    }
    
    inline osg::ref_ptr<osg::Texture2D> getPossiowTexture()
    {
        return _poissowTex;
    }
    
    inline int getNumOutImportanceSampleTextures()
    {
        return (int)_out_sample_textures.size();
    }
    
    inline bool isImportanceSampleEnabled()
    {
        return _impSampleEnabled;
    }
    
    inline float getSplatsSizeRow()
    {
        return _splatsSize;
    }
    
    inline int getRSMWidth()
    {
        return _rsmWidth;
    }
    
    inline int getRSMHeight()
    {
        return _rsmHeight;
    }
    
    std::vector<int> getSampledLightIds();
    
protected:
    virtual void configureStateSet();
    void configRTTCamera();
    
private:
    
    void addImportanceSampleCamera(osg::Texture2D *mipMapIn, osg::Texture2D *outTex);
    void addMipMapCamera(osg::TextureRectangle *fluxTex, osg::Texture2D *mipMapOut);
    
    osg::ref_ptr<osg::Texture2D> createImportanceSampleOutTexture();
    osg::ref_ptr<osg::Texture2D> createMipMapTexture();
    void loadPoissonTexture();
    void generatePoissonTexture();
    
     void configDebugPoints();
    
    int _rsmWidth;
    int _rsmHeight;
    
    int _splatsSize;
    
    osg::ref_ptr<osg::Texture2D> _poissowTex;
    
    std::map<int, int> _in_flux_mipmap_ids; // left, light_id || right, texture_id
//    std::map<int, int> _out_sample_tex_ids;
    std::map<int, osg::ref_ptr<osg::Texture2D> > _out_sample_textures;
    
    osg::ref_ptr<osg::Group> _mipMapCameraGroup;
    osg::ref_ptr<osg::Group> _importanceSampleCameraGroup;
    
    osg::ref_ptr<osg::Group> _screenQuad;
    
    int _mipMapShaderId;
    int _importanceSamplingShaderId;
    
    ShadowGroup *_shadowGroup;
    DirectionalLightGroup *_dirLightGroup;
    
    osg::ref_ptr<osg::Texture2D> _mipMapOut;
    
    bool _impSampleEnabled;
    
    // debug visualization
     osg::ref_ptr<osg::Group> _debugPoints;
    
};

#endif /* defined(__vrphysics__ImportanceSamplingPass__) */
