// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Radu Serban, Rainer Gericke
// =============================================================================
//
// Main driver function for the MAN truck models.
//
// The vehicle reference frame has Z up, X towards the front of the vehicle, and
// Y pointing to the left.
//
// =============================================================================

#include "chrono/utils/ChUtilsInputOutput.h"

#include "chrono_vehicle/ChConfigVehicle.h"
#include "chrono_vehicle/ChVehicleModelData.h"
#include "chrono_vehicle/driver/ChDataDriver.h"
#include "chrono_vehicle/driver/ChInteractiveDriverIRR.h"
#include "chrono_vehicle/terrain/RigidTerrain.h"
#include "chrono_vehicle/wheeled_vehicle/ChWheeledVehicleVisualSystemIrrlicht.h"

#include "chrono_models/vehicle/man/MAN_5t.h"
#include "chrono_models/vehicle/man/MAN_7t.h"
#include "chrono_models/vehicle/man/MAN_10t.h"

#include "chrono_thirdparty/filesystem/path.h"

using namespace chrono;
using namespace chrono::irrlicht;
using namespace chrono::vehicle;
using namespace chrono::vehicle::man;

// =============================================================================

// Select one of the MAN truck models (5, 7, or 10)
#define TRUCK 10

// Initial vehicle location and orientation
ChVector3d initLoc(0, 0, 0.7);
ChQuaternion<> initRot(1, 0, 0, 0);

enum DriverMode { DEFAULT, RECORD, PLAYBACK };
DriverMode driver_mode = DEFAULT;

// Visualization type for vehicle parts (PRIMITIVES, MESH, or NONE)
VisualizationType chassis_vis_type = VisualizationType::MESH;
VisualizationType suspension_vis_type = VisualizationType::PRIMITIVES;
VisualizationType steering_vis_type = VisualizationType::PRIMITIVES;
VisualizationType wheel_vis_type = VisualizationType::MESH;
VisualizationType tire_vis_type = VisualizationType::MESH;

// Collision type for chassis (PRIMITIVES, MESH, or NONE)
CollisionType chassis_collision_type = CollisionType::NONE;

// Type of engine model (SHAFTS, SIMPLE, SIMPLE_MAP)
EngineModelType engine_model = EngineModelType::SIMPLE;

// Type of transmission model (SHAFTS, SIMPLE_MAP)
TransmissionModelType transmission_model = TransmissionModelType::AUTOMATIC_SIMPLE_MAP;

// Type of tire model (TMEASY, TMSIMPLE)
TireModelType tire_model = TireModelType::TMSIMPLE;

// Type of break model (SIMPLE, SHAFTS)
BrakeType brake_model = BrakeType::SHAFTS;

// Rigid terrain
RigidTerrain::PatchType terrain_model = RigidTerrain::PatchType::BOX;
double terrainHeight = 0;      // terrain height (FLAT terrain only)
double terrainLength = 200.0;  // size in X direction
double terrainWidth = 200.0;   // size in Y direction

// Point on chassis tracked by the camera
ChVector3d trackPoint(0.0, 0.0, 1.75);

// Contact method
ChContactMethod contact_method = ChContactMethod::SMC;
bool contact_vis = false;

// Simulation step sizes
double step_size = 1e-3;
double tire_step_size = step_size;

// Simulation end time
double t_end = 1000;

// Time interval between two render frames
double render_step_size = 1.0 / 50;  // FPS = 50

// Debug logging
bool debug_output = false;
double debug_step_size = 1.0 / 1;  // FPS = 1

// POV-Ray output
bool povray_output = false;

// =============================================================================

int main(int argc, char* argv[]) {
    std::cout << "Copyright (c) 2019 projectchrono.org\nChrono version: " << CHRONO_VERSION << std::endl;

    // --------------
    // Create systems
    // --------------

#if !defined(TRUCK)
    #define TRUCK 5
#endif

#if defined(TRUCK) && (TRUCK == 5)
    std::string truck_name = "MAN_5t";
    MAN_5t truck;
#endif

#if defined(TRUCK) && (TRUCK == 7)
    std::string truck_name = "MAN_7t";
    MAN_7t truck;
    truck.SetDriveline6WD(false);
#endif

#if defined(TRUCK) && (TRUCK == 10)
    std::string truck_name = "MAN_10t";
    MAN_10t truck;
    truck.SetDriveline8WD(false);
#endif

    truck.SetContactMethod(contact_method);
    truck.SetChassisCollisionType(chassis_collision_type);
    truck.SetChassisFixed(false);
    truck.SetInitPosition(ChCoordsys<>(initLoc, initRot));
    truck.SetEngineType(engine_model);
    truck.SetTransmissionType(transmission_model);
    truck.SetTireType(tire_model);
    truck.SetBrakeType(brake_model);
    truck.SetTireStepSize(tire_step_size);
    truck.Initialize();

    truck.SetChassisVisualizationType(chassis_vis_type);
    truck.SetSuspensionVisualizationType(suspension_vis_type);
    truck.SetSteeringVisualizationType(steering_vis_type);
    truck.SetWheelVisualizationType(wheel_vis_type);
    truck.SetTireVisualizationType(tire_vis_type);

    // Associate a collision system
    truck.GetSystem()->SetCollisionSystemType(ChCollisionSystem::Type::BULLET);

    // Create the terrain
    RigidTerrain terrain(truck.GetSystem());

    ChContactMaterialData minfo;
    minfo.mu = 0.9f;
    minfo.cr = 0.01f;
    minfo.Y = 2e7f;
    auto patch_mat = minfo.CreateMaterial(contact_method);

    std::shared_ptr<RigidTerrain::Patch> patch;
    switch (terrain_model) {
        case RigidTerrain::PatchType::BOX:
            patch = terrain.AddPatch(patch_mat, CSYSNORM, terrainLength, terrainWidth);
            patch->SetTexture(vehicle::GetDataFile("terrain/textures/tile4.jpg"), 200, 200);
            break;
        case RigidTerrain::PatchType::HEIGHT_MAP:
            patch = terrain.AddPatch(patch_mat, CSYSNORM, vehicle::GetDataFile("terrain/height_maps/test64.bmp"), 128,
                                     128, 0, 4);
            patch->SetTexture(vehicle::GetDataFile("terrain/textures/grass.jpg"), 16, 16);
            break;
        case RigidTerrain::PatchType::MESH:
            patch = terrain.AddPatch(patch_mat, CSYSNORM, vehicle::GetDataFile("terrain/meshes/test.obj"));
            patch->SetTexture(vehicle::GetDataFile("terrain/textures/grass.jpg"), 100, 100);
            break;
    }
    patch->SetColor(ChColor(0.8f, 0.8f, 0.5f));

    terrain.Initialize();

    // Create the vehicle Irrlicht interface
    auto vis = chrono_types::make_shared<ChWheeledVehicleVisualSystemIrrlicht>();
    vis->SetWindowTitle(truck_name + " Truck Demo");
    vis->SetChaseCamera(trackPoint, 10.0, 0.5);
    vis->Initialize();
    vis->AddLightDirectional();
    vis->AddSkyBox();
    vis->AddLogo();
    vis->AttachVehicle(&truck.GetVehicle());

    // -----------------
    // Initialize output
    // -----------------

    const std::string out_dir = GetChronoOutputPath() + truck_name;
    const std::string pov_dir = out_dir + "/POVRAY";

    if (!filesystem::create_directory(filesystem::path(out_dir))) {
        std::cout << "Error creating directory " << out_dir << std::endl;
        return 1;
    }
    if (povray_output) {
        if (!filesystem::create_directory(filesystem::path(pov_dir))) {
            std::cout << "Error creating directory " << pov_dir << std::endl;
            return 1;
        }
        terrain.ExportMeshPovray(out_dir);
    }

    std::string driver_file = out_dir + "/driver_inputs.txt";
    utils::ChWriterCSV driver_csv(" ");

    // ------------------------
    // Create the driver system
    // ------------------------

    // Create the interactive driver system
    ChInteractiveDriverIRR driver(*vis);

    // Set the time response for steering and throttle keyboard inputs.
    double steering_time = 1.0;  // time to go from 0 to +1 (or from 0 to -1)
    double throttle_time = 1.0;  // time to go from 0 to +1
    double braking_time = 0.3;   // time to go from 0 to +1
    driver.SetSteeringDelta(render_step_size / steering_time);
    driver.SetThrottleDelta(render_step_size / throttle_time);
    driver.SetBrakingDelta(render_step_size / braking_time);

    // If in playback mode, attach the data file to the driver system and
    // force it to playback the driver inputs.
    if (driver_mode == PLAYBACK) {
        driver.SetInputDataFile(driver_file);
        driver.SetInputMode(ChInteractiveDriverIRR::InputMode::DATAFILE);
    }

    driver.Initialize();

    // ---------------
    // Simulation loop
    // ---------------

    if (debug_output) {
        std::cout << "\n\n============ System Configuration ============\n";
        truck.LogHardpointLocations();
    }

    truck.GetVehicle().LogSubsystemTypes();
    std::cout << "\nVehicle mass: " << truck.GetVehicle().GetMass() << std::endl;

    // Number of simulation steps between miscellaneous events
    int render_steps = (int)std::ceil(render_step_size / step_size);
    int debug_steps = (int)std::ceil(debug_step_size / step_size);

    // Initialize simulation frame counters
    int step_number = 0;
    int render_frame = 0;

    if (contact_vis) {
        vis->SetSymbolScale(1e-4);
        vis->EnableContactDrawing(ContactsDrawMode::CONTACT_FORCES);
    }

    truck.GetVehicle().EnableRealtime(true);
    while (vis->Run()) {
        double time = truck.GetSystem()->GetChTime();

        // End simulation
        if (time >= t_end)
            break;

        // Render scene and output POV-Ray data
        if (step_number % render_steps == 0) {
            vis->BeginScene();
            vis->Render();
            vis->EndScene();

            if (povray_output) {
                // Zero-pad frame numbers in file names for postprocessing
                std::ostringstream filename;
                filename << pov_dir << "/data_" << std::setw(4) << std::setfill('0') << render_frame + 1 << ".dat";
                utils::WriteVisualizationAssets(truck.GetSystem(), filename.str());
            }

            render_frame++;
        }

        // Debug logging
        if (debug_output && step_number % debug_steps == 0) {
            std::cout << "\n\n============ System Information ============\n";
            std::cout << "Time = " << time << "\n\n";
            truck.DebugLog(OUT_SPRINGS | OUT_SHOCKS | OUT_CONSTRAINTS);
        }

        // Collect output data from modules (for inter-module communication)
        DriverInputs driver_inputs = driver.GetInputs();

        // Driver output
        if (driver_mode == RECORD) {
            driver_csv << time << driver_inputs.m_steering << driver_inputs.m_throttle << driver_inputs.m_braking
                       << std::endl;
        }

        // Update modules (process inputs from other modules)
        driver.Synchronize(time);
        terrain.Synchronize(time);
        truck.Synchronize(time, driver_inputs, terrain);
        vis->Synchronize(time, driver_inputs);

        // Advance simulation for one timestep for all modules
        driver.Advance(step_size);
        terrain.Advance(step_size);
        truck.Advance(step_size);
        vis->Advance(step_size);

        // Increment frame number
        step_number++;
    }

    if (driver_mode == RECORD) {
        driver_csv.WriteToFile(driver_file);
    }

    return 0;
}
