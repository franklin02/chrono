#------------------------------------------------------------------------------
# Name:        pychrono example
# Purpose:
#
# Author:      Alessandro Tasora
#
# Created:     1/01/2019
# Copyright:   (c) ProjectChrono 2019
#------------------------------------------------------------------------------

print ("Example: create OpenCascade shapes and use them as rigid bodies");


import pychrono.core as chrono
import pychrono.irrlicht as chronoirr
import pychrono.cascade as cascade
try:
    from OCC.Core import BRepPrimAPI
    from OCC.Core import BRepAlgoAPI
except:
    from OCC import BRepPrimAPI, BRepAlgoAPI
# Change this path to asset path, if running from other working dir. 
# It must point to the data folder, containing GUI assets (textures, fonts, meshes, etc.)
chrono.SetChronoDataPath("../../../data/")


# ---------------------------------------------------------------------
#
#  Create the simulation system and add items
#

    
mysystem      = chrono.ChSystemNSC()


# Set the global collision margins. This is expecially important for very large or
# very small objects. Set this before creating shapes. Not before creating mysystem.
chrono.ChCollisionModel.SetDefaultSuggestedEnvelope(0.001);
chrono.ChCollisionModel.SetDefaultSuggestedMargin(0.001);


# create a 3dCAD shape using the OCC OpenCascade API (a torus cut by a cylinder)
my_torus    = BRepPrimAPI.BRepPrimAPI_MakeTorus(0.1,0.02).Shape()
my_cylinder = BRepPrimAPI.BRepPrimAPI_MakeCylinder(0.09,0.1).Shape()
my_shape    = BRepAlgoAPI.BRepAlgoAPI_Cut(my_torus, my_cylinder).Shape()

# use it to make a body with proper center of mass and inertia tensor,
# given the CAD shape. Also visualize it.
my_body = cascade.ChBodyEasyCascade(my_shape,# the CAD shape
                                  1000,    # the density
                                  True,    # must collide using the triangle mesh geometry?
                                  True)    # must be visualized?
mysystem.Add(my_body)

    
# Create a large cube as a floor.

my_floor = chrono.ChBodyEasyBox(1, 0.2, 1, 1000, True)
my_floor.SetPos(chrono.ChVectorD(0,-0.3,0))
my_floor.SetBodyFixed(True)
mysystem.Add(my_floor)

my_color = chrono.ChColorAsset(0.2, 0.2, 0.5)
my_floor.AddAsset(my_color)



# ---------------------------------------------------------------------
#
#  Create an Irrlicht application to visualize the system
#

myapplication = chronoirr.ChIrrApp(mysystem, 'Use OpenCascade shapes', chronoirr.dimension2du(1024,768))

myapplication.AddTypicalSky(chrono.GetChronoDataPath() + 'skybox/')
myapplication.AddTypicalLogo(chrono.GetChronoDataPath() + 'logo_pychrono_alpha.png')
myapplication.AddTypicalCamera(chronoirr.vector3df(0.2,0.2,-0.2))
myapplication.AddTypicalLights()

            # ==IMPORTANT!== Use this function for adding a ChIrrNodeAsset to all items
			# in the system. These ChIrrNodeAsset assets are 'proxies' to the Irrlicht meshes.
			# If you need a finer control on which item really needs a visualization proxy in
			# Irrlicht, just use application.AssetBind(myitem); on a per-item basis.

myapplication.AssetBindAll();

			# ==IMPORTANT!== Use this function for 'converting' into Irrlicht meshes the assets
			# that you added to the bodies into 3D shapes, they can be visualized by Irrlicht!

myapplication.AssetUpdateAll();


# ---------------------------------------------------------------------
#
#  Run the simulation
#

mysystem.SetSolverType(chrono.ChSolver.Type_SOR)

myapplication.SetTimestep(0.005)


while(myapplication.GetDevice().run()):
    myapplication.BeginScene()
    myapplication.DrawAll()
    myapplication.DoStep()
    myapplication.EndScene()





