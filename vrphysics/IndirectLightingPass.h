//
//  IndirectLightingPass.h
//  vrphysics
//
//  Created by Robin Wu on 2/6/15.
//  Copyright (c) 2015 WSH. All rights reserved.
//

#ifndef __vrphysics__IndirectLightingPass__
#define __vrphysics__IndirectLightingPass__

#include <stdio.h>
#include <map>
#include "ScreenPass.h"

class VPLGroup;
class ImportanceSamplingPass;
class IndirectLightingPass : public ScreenPass
{
public:
    IndirectLightingPass(osg::Camera *mainCamera, ImportanceSamplingPass *impPass, osg::TextureRectangle *worldPosTex, osg::TextureRectangle *lightDirTex);
    virtual ~IndirectLightingPass();
    
    inline void setImportanceSamplingEnabled(bool tf)
    {
        _importanceSamplingEnabled = tf;
    }
    
    inline bool isImportanceSamplingEnabled()
    {
        return _importanceSamplingEnabled;
    }
    
    inline osg::ref_ptr<osg::TextureRectangle> getIndirectLightingTex()
    {
        return getOutputTexture(_out_indirectLightingTex_id);
    }
    
    // overriding
    virtual int addOutTexture();
    
protected:
    virtual void configureStateSet();
    
private:
    void loadRegularSampleTexture();
    
    int _in_worldPosTex_id;
    int _in_lightDirTex_id;
    
    osg::ref_ptr<osg::Texture2D> _sampleTex;
    std::map<int, osg::ref_ptr<osg::Texture2D> > _importanceSampleTextures; // for each light source
    
    int _out_indirectLightingTex_id;
    
    bool _importanceSamplingEnabled;
    
    int _indirectLightShader;
    
    ImportanceSamplingPass *_impPass;
    VPLGroup *_vplGroup;
};

#endif /* defined(__vrphysics__IndirectLightingPass__) */
