#include <thread>
#include <chrono>
#include <algorithm>
#include <random>
#include "GenericScene.h"
#include "CBullet.h"



using namespace std;

int NUMBER_OF_RUNS     = 5;

const double SPHERE_MASS        = 0.04;
const double K_DAMPING          = 0.9999999999999999;
const double HIP_STIFFNESS      = 15;
const double CIP_STIFFNESS      = 15;
const double TARGET_STIFFNESS   = 10;
const double BALL_STIFFNESS     = 60;
const double TARGET_DAMPING     = 0.1;
const double BALL_DAMPING       = 0.1;
const double WALL_GROUND        = -0.2;

double MAX_HAPTIC_FORCE = 4;

bool flagHapticsEnabled = false;
double hapticDeviceMaxStiffness;





void GenericScene::updateTarget(){
    destination_index++;
    if (fuzzyControl) {
        time_t timetaken = difftime(time(0), timeSinceTarget);
        cout << "Time: " << timetaken << " Collisions: " << (collisionNum-collsSinceTarget) << endl;
        double result = getFuzzyOutput(timetaken, (collisionNum - collsSinceTarget));
        cout << "Change in alpha: " << result << endl;
        ALPHA_CONTROL += result;
        ALPHA_CONTROL = min(1.0, ALPHA_CONTROL);
        ALPHA_CONTROL = max(0.0, ALPHA_CONTROL);
        currentAlpha = ALPHA_CONTROL;
        timeSinceTarget = time(0);
        collsSinceTarget = collisionNum;
    }
    cout<< "updated target" << endl;
    if( destination_index == destinations.size() ){
        NUMBER_OF_RUNS--;
        if (NUMBER_OF_RUNS > 0) {
            performanceReview();
            timeLastRun = time(0);
            destination_index = 0;
            target->setLocalPos(destinations[shuffled_order[destination_index]]);
            generateWaypoints(mainSphere->getLocalPos(), target->getLocalPos());
            initWaypoints();
        }
        cout << "finished" << endl;
    }
    target->setLocalPos(destinations[shuffled_order[destination_index]]);
}

void GenericScene::performanceReview() {
    
    double finalColls = collisionNum - collsLastRun;
    double finalTime = difftime(time(0), timeLastRun);
    string feedback = "";
    if (finalColls > 2000) {
        feedback += "You did terribly";
    }
    else if (finalColls > 1000) {
        feedback += "You need some practice";
    }
    else if (finalColls > 500) {
        feedback += "You did OK";
    }
    else if (finalColls > 100) {
        feedback += "You did very well!";
    }
    else if (finalColls > 0) {
        feedback += "You did excellent!";
    }
    else if (finalColls == 0) {
        feedback += "You did perfect!";
    }

    if (finalTime > 120) {
        feedback += "\nYou really need to hurry up!";
    }
    else if (finalTime > 100) {
        feedback += "\nYou could go a bit faster!";
    }
    else if (finalTime > 80) {
        feedback += "\nYou made decent time!";
    }
    else if (finalTime > 60) {
        feedback += "\nYou had time to spare!";
    }
    else if (finalTime > 40) {
        feedback += "\nYou were lightning fast!";
    }
    labelReview->setText("Runs remaining: " + to_string(NUMBER_OF_RUNS) +"\n" + feedback);
    collsLastRun = collisionNum;
}

GenericScene::GenericScene(shared_ptr<cGenericHapticDevice> a_hapticDevice)
{   
    
    //-----------------------------------------------------------------------
    // WORLD - CAMERA - LIGHTING
    //-----------------------------------------------------------------------
    
    // Create the world.
    bulletWorld = new cBulletWorld();
    bulletWorld->m_backgroundColor.setBlack();

    // Create the camera.
    camera = new cCamera(bulletWorld);
    bulletWorld->addChild(camera);
    camera->setSphericalReferences(cVector3d(0.00, 0.00, -0.20),cVector3d(0.00, 0.00, 1.00), cVector3d(1.00, 0.00, 0.00));
    camera->set(cVector3d(0.50, 0.00, 0.40),    // Camera position (eye)
                cVector3d(0.00, 0.00, -0.20),    // Lookat position (target)
                cVector3d(0.00, 0.00, 1.00));    // Direction of the (up) vector
    camera->setClippingPlanes(0.01, 10.0); // Set near and far clipping planes of the camera
    camera->setStereoEyeSeparation(0.01);
    camera->setStereoFocalLength(0.5);

    
    //Create a spot light.
    spotLight = new cSpotLight(bulletWorld);
    bulletWorld->addChild(spotLight);
    spotLight->setEnabled(true); // Enable light source
    spotLight->setLocalPos(0.4, 0.4, 0.3); // Posision the light source
    spotLight->setDir(-0.4, -0.4, -0.3); // Define the direction of the light beam
    spotLight->setSpotExponent(0.0); // Set uniform concentration of light
    spotLight->m_ambient.set(0.6, 0.6, 0.6);
    spotLight->m_diffuse.set(0.8, 0.8, 0.8);
    spotLight->m_specular.set(0.8, 0.8, 0.8);
    spotLight->setCutOffAngleDeg(45); // Set the light cone half angle

    // Create a directional light.
    directionalLight = new cDirectionalLight(bulletWorld);
    bulletWorld->addChild(directionalLight);                   // attach light to camera
    directionalLight->setEnabled(true);                    // enable light source
    directionalLight->setDir(0.0, 0.2, -1.0);             // define the direction of the light beam
    directionalLight->m_ambient.set(0.3, 0.3, 0.3);
    directionalLight->m_diffuse.set(0.6, 0.6, 0.6);
    directionalLight->m_specular.set(0.0, 0.0, 0.0);

    // Create the ground.
    double planeWidth = 0.2;
    bulletGround = new cBulletStaticPlane(bulletWorld,  cVector3d(0.0, 0.0, 1.0),-planeWidth);
    bulletWorld->addChild(bulletGround);
    cCreatePlane(bulletGround, 0.6, 0.6, bulletGround->getPlaneConstant() * bulletGround->getPlaneNormal());

    bulletCeilling = new cBulletStaticPlane(bulletWorld,  cVector3d(0.0, 0.0, -1.0),-planeWidth);
    bulletWorld->addChild(bulletCeilling);
    cCreatePlane(bulletCeilling, 0.6, 0.6, bulletCeilling->getPlaneConstant() * bulletCeilling->getPlaneNormal()+cVector3d(0.0,0.0, 0.01));
    bulletCeilling->createAABBCollisionDetector(toolRadius);
    bulletCeilling->setEnabled(false);

    //-----------------------------------------------------------------------
    // Widget
    //-----------------------------------------------------------------------    
    cFontPtr font = NEW_CFONTCALIBRI40();

    labelHapticDeviceModel = new cLabel(font);
    cHapticDeviceInfo hapticDeviceInfo = a_hapticDevice->getSpecifications();
    labelHapticDeviceModel->setText(hapticDeviceInfo.m_modelName);
    if(hapticDeviceInfo.m_modelName == "Touch X"){
        MAX_HAPTIC_FORCE = 4;
    }else{
        MAX_HAPTIC_FORCE = 2;
    }
    // Create a label to display the position of haptic device
    labelHapticDevicePosition = new cLabel(font);


    // Create a label to display the haptic and graphic rate of the simulation
    labelRates = new cLabel(font);
    labelRates->m_fontColor.setWhite();
    camera->m_frontLayer->addChild(labelRates);

    // Create bar to display control level
    controlLevel = new cLevel();
    controlLevel->setLocalPos(20, 60);
    controlLevel->setRange(0.0, 1.0);
    controlLevel->setWidth(40);
    controlLevel->setNumIncrements(46);
    controlLevel->setSingleIncrementDisplay(false);
    controlLevel->setTransparencyLevel(0.5);

    // Create a label to display the time taken to complete task
    labelTime = new cLabel(font);
    labelTime->m_fontColor.setWhite();
    camera->m_frontLayer->addChild(labelTime);

    //Create a label to display the number of collisions
    labelCollisions = new cLabel(font);
    labelCollisions->m_fontColor.setWhite();
    camera->m_frontLayer->addChild(labelCollisions);

    labelReview = new cLabel(NEW_CFONTCALIBRI72());
    labelReview->m_fontColor.setWhite();
    camera->m_frontLayer->addChild(labelReview);


    hapticDevice = a_hapticDevice;
    mirroredDisplay = false;
    toolRadius = 0.0025;

    //-----------------------------------------------------------------------
    // Border Materials
    //-----------------------------------------------------------------------
    cMaterial matBase;
    matBase.setGrayLevel(0.3);
    matBase.setStiffness(10);
    matBase.setDynamicFriction(0.5);
    matBase.setStaticFriction(0.9);

    //-----------------------------------------------------------------------
    // SETUP GROUND AND TOOL
    //-----------------------------------------------------------------------
    // Read the scale factor between the physical workspace of the haptic
    // device and the virtual workspace defined for the tool
    // Setting the ground
    cMaterial matGround;
    matGround.setStiffness(1);
    matGround.setDynamicFriction(0.10);
    matGround.setStaticFriction(0.20);
    matGround.setBlueRoyal();
    matGround.m_emission.setGrayLevel(0.3);
    bulletGround->setMaterial(matGround);
    bulletGround->createAABBCollisionDetector(toolRadius);

    //Create HIP sphere
    controlSphere = new cShapeSphere(toolRadius);
    bulletWorld->addChild(controlSphere);
    controlSphere->setLocalPos(0.0,0.0, -0.20);
    controlSphere->m_material->setWhite();
    controlSphere->setEnabled(false);

    cMaterial mat;
    mat.setRedIndian();
    mat.m_specular.set(0.0, 0.0, 0.0);
    mat.setStiffness(1);
    mat.setDynamicFriction(0.9);
    mat.setStaticFriction(0.9);

    //Create CIP sphere
    guidanceSphere = new cBulletSphere(bulletWorld, toolRadius);
    bulletWorld->addChild(guidanceSphere);
    guidanceSphere->createAABBCollisionDetector(toolRadius);
    guidanceSphere->setMaterial(mat);
    guidanceSphere->setMass(SPHERE_MASS);
    guidanceSphere->buildDynamicModel();
    guidanceSphere->setLocalPos(0.01,0.01,-0.2+toolRadius);
    guidanceSphere->setDamping(K_DAMPING, K_DAMPING);
    guidanceSphere->m_material->setBlue();
    guidanceSphere->setEnabled(false);
    guidanceSphere->m_bulletRigidBody->setUserPointer(guidanceSphere);
    
    //Create NIP sphere
    negotiatedSphere = new cBulletSphere(bulletWorld, toolRadius);
    bulletWorld->addChild(negotiatedSphere);
    negotiatedSphere->createAABBCollisionDetector(toolRadius);
    negotiatedSphere->setMaterial(mat);
    negotiatedSphere->setMass(SPHERE_MASS);
    negotiatedSphere->buildDynamicModel();
    negotiatedSphere->setLocalPos(0.0,0.0,-0.2+toolRadius);
    negotiatedSphere->setDamping(K_DAMPING, K_DAMPING);
    negotiatedSphere->setEnabled(false);
    negotiatedSphere->m_bulletRigidBody->setUserPointer(negotiatedSphere);
    negotiatedSphere->m_bulletRigidBody->setIgnoreCollisionCheck(guidanceSphere->m_bulletRigidBody, true);
    negotiatedSphere->m_material->setYellow();
    //guidanceSphere->m_bulletRigidBody->setCollisionFlags(guidanceSphere->m_bulletRigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

    //Create main sphere
    mainSphere = new cBulletSphere(bulletWorld, toolRadius+0.002);
    bulletWorld->addChild(mainSphere);
    mainSphere->createAABBCollisionDetector(toolRadius);
    mainSphere->setMaterial(mat);
    mainSphere->setMass(SPHERE_MASS);
    mainSphere->buildDynamicModel();
    mainSphere->setLocalPos(0.01,0.02,-0.2+toolRadius);
    mainSphere->setDamping(K_DAMPING, K_DAMPING);
    mainSphere->m_bulletRigidBody->setUserPointer(mainSphere);
    mainSphere->m_bulletRigidBody->setIgnoreCollisionCheck(guidanceSphere->m_bulletRigidBody, true);
    mainSphere->m_bulletRigidBody->setIgnoreCollisionCheck(negotiatedSphere->m_bulletRigidBody, true);
    mainSphere->setInertia(cVector3d(0,0,0));

    borderSetup({0.005, 0.6, 0.05}, {0.3, 0.0, -0.2}, { 0,0,0 }, matBase);
    borderSetup({0.005, 0.6, 0.05}, {-0.3, 0.0, -0.2 }, { 0,0,0 }, matBase);
    borderSetup({0.6, 0.005, 0.05}, {0.0, 0.3, -0.2}, { 0,0,0 }, matBase);
    borderSetup({0.6, 0.005, 0.05}, {0.0, -0.3, -0.2 }, { 0,0,0 }, matBase);

    // Set gravity.
    bulletWorld->setGravity(cVector3d(0.0, 0.0, -9.0));

    // retrieve the highest stiffness this device can render
    hapticDeviceMaxStiffness = hapticDeviceInfo.m_maxLinearStiffness;

    //initial the starting point for robot guidance.
    waypoints.push_back(cVector3d(0, 0.0,-0.2));
    waypointsRange.push_back(0.01);
    positionWaypoint = waypoints[0];

    //Initialise the engine for fuzzy logic
    engine = new fl::Engine();
    engine->setName("Fuzzy Control Engine");
    engine->setDescription("");

    startTime = time(0);
    timeLastRun = time(0);
    timeSinceTarget = time(0);
}

void GenericScene::borderSetup(std::vector<double> size, std::vector<double> pos, std::vector<double> rot,cMaterial matBase) {
    cBulletBox* obstacle = new cBulletBox(bulletWorld, size.at(0), size.at(1), size.at(2));
    obstacles.push_back(obstacle);
    bulletWorld->addChild(obstacle);
    obstacle->createAABBCollisionDetector(toolRadius);
    obstacle->setMaterial(matBase);
    obstacle->buildDynamicModel();
    obstacle->setLocalPos(pos.at(0), pos.at(1), pos.at(2));
    obstacle->setLocalRot(cMatrix3d(cDegToRad(rot.at(0)), cDegToRad(rot.at(1)), cDegToRad(rot.at(2)), C_EULER_ORDER_XYZ));
    obstacle->m_bulletRigidBody->setCollisionFlags(obstacle->m_bulletRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    obstacle->m_bulletRigidBody->setUserPointer(obstacle);
    negotiatedSphere->m_bulletRigidBody->setIgnoreCollisionCheck(obstacle->m_bulletRigidBody, true);
    guidanceSphere->m_bulletRigidBody->setIgnoreCollisionCheck(obstacle->m_bulletRigidBody, true);
}

void GenericScene::updateWaypoint(cVector3d positionSphere, cVector3d positionTarget) {
    //If target is reached, clear the current list of waypoints and update the target
    if (cDistance(positionSphere, positionTarget) < waypointsRange[waypointsRange.size() - 2]) {
        updateTarget();
        waypoints.clear();
        generateWaypoints(positionSphere, target->getLocalPos());
        waypoint_index = 0;
        last_waypoint_index = -1;
        positionWaypoint = waypoints[0];

    }

    //If waypoint is reached, go to next waypoint
    if(waypoint_index == last_waypoint_index)
    {
        waypoint_index = last_waypoint_index +1;
        return;
    }

    //If main sphere is closer to next waypoint than current one, advance to next waypoint
    if ((cDistance(positionSphere, waypoints[waypoint_index+1]) < cDistance(waypoints[waypoint_index], waypoints[waypoint_index+1])*0.7) &&
        waypoint_index != waypoints.size()-2) {
        cout << "Advancing to waypoint " << waypoint_index + 1 << endl;
        waypoint_index += 1;
        last_waypoint_index += 1;
    }

    if(waypoint_index == waypoints.size()-1){
        updateTarget();
        generateWaypoints(positionSphere, target->getLocalPos());
    }

    waypoint_index = waypoint_index % waypoints.size();
    positionWaypoint = waypoints[waypoint_index];
}

void GenericScene::updateGraphics(int a_width, int a_height){

    //Update cameras and control level label
    bulletWorld->updateShadowMaps(false, mirroredDisplay);
    camera->renderView(a_width, a_height);

    controlLevel->setValue(1-ALPHA_CONTROL);

}

void GenericScene::updateHaptics(double timeInterval){
    bulletWorld->computeGlobalPositions(true);
     
    //Retrieve positions for all spheres ans the haptic device

    cVector3d hapticDevicePosition;
    cVector3d positionTarget = target->getLocalPos();
    positionNegotiatedSphere = negotiatedSphere->getLocalPos();
    positionMainSphere = mainSphere->getLocalPos();
    positionGuidanceSphere = guidanceSphere->getLocalPos();
    
    hapticDevice->getPosition(hapticDevicePosition); 
    hapticDevicePosition = hapticDevicePosition *10;

    //Additional force required to keep the device still in the z axis
    cVector3d wallForce;
    wallForce.add(cVector3d(0.0, 0.0, 50 * (WALL_GROUND + toolRadius- hapticDevicePosition.z())));
    hapticDevicePosition.z(WALL_GROUND+toolRadius);
    
    // Compute the various forces.
    // Force at HIP towards NIP and haptic force for the device
    cVector3d disp_HIP_NIP = hapticDevicePosition - positionNegotiatedSphere;
    HIPForce = (HIP_STIFFNESS * disp_HIP_NIP);
    cVector3d hapticForce = -HIPForce + wallForce;

    //Force exerted by the user
    userForce = HIPForce.length();
    if (userForce < 1.0) {
        if (!userInactive) {
            using namespace std::chrono;
            inactiveTime = high_resolution_clock::now();
        }
        userInactive = true;
    }
    else {
        userInactive = false;
    }

    // Force at CIP towards NIP. 
    cVector3d disp_CIP_NIP = positionGuidanceSphere - positionNegotiatedSphere;
    CIPForce = (CIP_STIFFNESS * disp_CIP_NIP);

    // The force of CIP towards current waypoint.
    cVector3d disp_TARGET_CIP = positionWaypoint - positionGuidanceSphere;
    cVector3d velocity_CIP = (Eigen::Vector3d)guidanceSphere->m_bulletRigidBody->getLinearVelocity();
    cVector3d waypointForce = (TARGET_STIFFNESS * disp_TARGET_CIP) + TARGET_DAMPING * velocity_CIP;

    // Force of the ball towards NIP.
    cVector3d disp_NIP_BALL = positionNegotiatedSphere - positionMainSphere;
    cVector3d velocity_BALL = (Eigen::Vector3d)mainSphere->m_bulletRigidBody->getLinearVelocity();
    cVector3d ballForce = (BALL_STIFFNESS * disp_NIP_BALL) + BALL_DAMPING * velocity_BALL;

    cVector3d NIPForce = CIPForce * (1-ALPHA_CONTROL) + HIPForce * ALPHA_CONTROL;
 
    if(cDistance(positionMainSphere, waypoints[waypoint_index]) < waypointsRange[waypoint_index]){
        last_waypoint_index = waypoint_index;
    }
   
    updateWaypoint(positionMainSphere, target->getLocalPos());

    HIPForce.z(0);
    ballForce.z(0);
    
    negotiatedSphere->addExternalForce(NIPForce);
    guidanceSphere->addExternalForce(waypointForce);
    mainSphere->addExternalForce(ballForce);
    controlSphere->setLocalPos(hapticDevicePosition); 

    double time = simClock.getCurrentTimeSeconds();
    double nextSimInterval = cClamp(time, 0.0001, 0.001);

    /////////////////////////////////////////////////////////////////////////
    // APPLY FORCES
    /////////////////////////////////////////////////////////////////////////
    
    // Safety fuse.
    if(hapticForce.length()> MAX_HAPTIC_FORCE){
        hapticForce = cNormalize(hapticForce) * MAX_HAPTIC_FORCE;
    }
    
    hapticDevice->setForce(hapticForce);

    // Reset Clock
    simClock.reset();
    simClock.start();
    bulletWorld->updateDynamics(nextSimInterval);

    
}

void GenericScene::init(){
    // Reset the camera, destination,
    camera->set(cVector3d(0.50, 0.00, 0.40),    
                cVector3d(0.00, 0.00, -0.20),   
                cVector3d(0.00, 0.00, 1.00));
    negotiatedSphere->setLocalPos(0.0,0.0,-0.2+toolRadius);
    destination_index = 0;
    

    // randomise the sequence of target position.
    auto rng = std::default_random_engine {};
    for(int i = 0; i < destinations.size(); i++){
        shuffled_order.push_back(i);
    }
    std::shuffle(std::begin(shuffled_order), std::end(shuffled_order), rng);
    target->setLocalPos(destinations[shuffled_order[destination_index]]);

    // Reset the position of spheres.
    negotiatedSphere->setLocalPos(0);
    controlSphere->setLocalPos(0.0,0.0, -0.20);
    negotiatedSphere->setLocalPos(0.0,0.0,-0.2+toolRadius);
    guidanceSphere->setLocalPos(0.02,0.02,-0.2+toolRadius);
    mainSphere->setLocalPos(0.01, 0.01, -0.2+toolRadius);
    waypoints.clear();
    waypointsRange.clear();
    

    // Generate the sequenece of waypooints and set the waypoint index based on the target position.
    generateWaypoints(mainSphere->getLocalPos(), target->getLocalPos());
    initWaypoints();
    
}

double GenericScene::getFuzzyOutput(int timein, int collisionsin) {
    //Input the time taken to reach the waypoint and the collisions, output the change in control level
    engine->inputVariables()[0]->setValue(timein);
    engine->inputVariables()[1]->setValue(collisionsin);
    engine->process();
    double output = stod(fl::Op::str(engine->outputVariables()[0]->getValue()));
    return output;
}

