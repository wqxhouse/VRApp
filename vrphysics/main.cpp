#include <osg/Group>

#include "Core.h"

#include "AssetDB.h"
#include "DirectionalLightGroup.h"

void handleGeometries(const osg::ref_ptr<osg::Group> geoms, Assets *const asset)
{
   asset->addGeometryWithFile("Testing/gi_test.dae");
//    asset->addGeometryWithFile("Testing/test2gi.dae");
//    asset->addGeometryWithFile("Testing/uuu.dae");
//    asset->addGeometryWithFile("Testing/cornell-box/engine/c.dae");
//    asset->addGeometryWithFile("Testing/testShadow.dae");
//    asset->addGeometryWithFile("Testing/testTorus.dae");
}

void handleDirLights(DirectionalLightGroup *const dirLightGroup)
{
//    dirLightGroup->addLight(osg::Vec3(5, 5, 5), osg::Vec3(0, 0, 0), osg::Vec3(1, 1, 1), true);
//    dirLightGroup->getDirectionalLight(0)->orbitAxis = osg::Vec3(0, 0, 1);
    
    dirLightGroup->getDirectionalLightsReference()[0]->setAnimated(true);
    dirLightGroup->getDirectionalLightsReference()[0]->orbitAxis = osg::Vec3(0, 0, 1);
    
//    dirLightGroup->getDirectionalLightsReference()[0]->setLookAt(osg::Vec3(0, -1, 0));
//    dirLightGroup->getDirectionalLightsReference()[0]->setPosition(osg::Vec3(0, 0, 0));
    
}

int main()
{
    Core core;
    core.setGeometryHandler(handleGeometries);
    core.setDirectionalLightHandler(handleDirLights);
    core.setWindowSize(800, 600);
    
    core.run();
}
