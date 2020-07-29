// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2020 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Rainer Gericke
// =============================================================================
// Vulkan Scene Graph viewer, this class will hopefully draw the system to the
// screen and handle input some day
// =============================================================================

#include "chrono_vsg/core/ChApiVSG.h"

#include "ChVSGApp.h"

using namespace chrono::vsg;

ChVSGApp::ChVSGApp(ChSystem* system) : m_system(system) {
    GetLog() << "System = " << m_system << "\n";

    m_windowTraits = ::vsg::WindowTraits::create();
    m_windowTraits->windowTitle = "Chrono VSG Viewer";

    m_searchPaths = ::vsg::getEnvPaths("VSG_FILE_PATH");

    // analyze system, look for bodies and assets
}

ChVSGApp::~ChVSGApp() {
    ;
}