#pragma once
#include <iostream>
#include <fstream>
#include <chai3d.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <numeric>
#include <iostream>
#include "GenericScene.h"
#include "Scene1.h"
#include "Scene2.h"
#include "Scene3.h"
#include "DebugScene.h"
#include "SensorData.h"

using namespace std;
using namespace chai3d;

//---------------------------------------------------------------------------
// DISPLAY SETTINGS
//---------------------------------------------------------------------------
cStereoMode stereoMode = C_STEREO_DISABLED;
bool fullscreen = false;
bool mirroredDisplay = false;

enum MouseStates
{
    MOUSE_IDLE,
    MOUSE_MOVE_CAMERA
};
MouseStates mouseState = MOUSE_IDLE; // Mouse state
double mouseX, mouseY; // Last mouse position

//---------------------------------------------------------------------------
// CHAI3D VARIABLES
//---------------------------------------------------------------------------
cCamera* camera;
cSpotLight* light;
cHapticDeviceHandler* handler;
shared_ptr<cGenericHapticDevice> hapticDevice;
cGenericTool* tool;

// cLabel* labelHapticDeviceModel;
// cLabel* labelHapticDevicePosition;
// cLabel* labelRates;
cVector3d hapticDevicePosition;

//---------------------------------------------------------------------------
// OBJECTS
//---------------------------------------------------------------------------
GenericScene* main_scene;
Scene1* scene1;
Scene2* scene2;
Scene3* scene3;
DebugScene* debugScene;

//------------------------------------------------------------------------------
// DECLARED CONSTANTS
//------------------------------------------------------------------------------
const double SPHERE_RADIUS = 0.007;

//---------------------------------------------------------------------------
// GENERAL VARIABLES
//---------------------------------------------------------------------------
bool simulationRunning = false; // A flag to indicate if the simulation running
bool simulationFinished = true; // A flag to indicate if the simulation finished
cFrequencyCounter freqCounterGraphics; // A frequency counter to measure the smiluation graphic rate
cFrequencyCounter freqCounterHaptics; // A frequency counter to measure the simulation haptic rate
cThread* hapticsThread; // Haptic Thread
GLFWwindow* window = NULL; // A handle to window display context

int width = 0; // Current width of the window
int height = 0; // Current height of the window
int swapInterval = 1;// swap interval for the display context (vertical synchronization)

//---------------------------------------------------------------------------
// EXPERIMENT VARIABLE
//---------------------------------------------------------------------------

string subject_num;
int game_scene;
int control_mode;
string fuzzy_params;


//---------------------------------------------------------------------------
// DECLARED FUNCTIONS
//---------------------------------------------------------------------------
void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height); // Callback when the window display is resized
void errorCallback(int error, const char* a_description); // Callback when an GLFW error occurs
void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods); // Callback when a key is pressed

void mouseButtonCallback(GLFWwindow* a_window, int a_button, int a_action, int a_mods); // Callback when mouse is click
void mouseMotionCallback(GLFWwindow* a_window, double a_posX, double a_posY);
void mouseScrollCallback(GLFWwindow* a_window, double a_offsetX, double a_offsetY);

void updateGraphics(void); // This function renders the scene
void updateHaptics(void); // This function contains the main haptics simulation loop
void close(void); // This function closes the application

// Initialises Scene 1
void initScene1();
// Initialises Scene 2
void initScene2();
// Initialises Scene 3
void initScene3();
// Initialises Debug Scene
void initDebugScene();
// Obtains Sensor data from script
void getSensorData();

// Files for storing data

std::ofstream ballfile;
std::ofstream conductancefile;
std::ofstream HIPfile;
std::ofstream CIPfile;
std::ofstream NIPfile;
std::ofstream HIPforcefile;
std::ofstream CIPforcefile;
std::ofstream alphafile;

// Script and thread for sensor data collecting

SensorData* s;
cThread* sensorThread;

float previousConductance = 0;

fl::Engine* fuzzy;

std::vector<float> forceLastSec;
std::vector<float> conductanceLastSec;