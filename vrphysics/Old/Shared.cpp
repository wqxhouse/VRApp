//
//  Shared.cpp
//  vrphysics
//
//  Created by Robin Wu on 11/7/14.
//  Copyright (c) 2014 WSH. All rights reserved.
//
#include "Shared.h"
#include <stdio.h>

int Shared::light_num = 100;

int Shared::win_width = 800;
int Shared::win_height = 600;

GBuffer Shared::gBuffer;
osgViewer::Viewer Shared::viewer;
std::vector<PointLight> Shared::pointLights;

float Shared::nearPlane;
float Shared::farPlane;