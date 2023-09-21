#include "Scene1.h"
#include "CBullet.h"
#include <fstream>
#include <iostream>
using namespace std;

/**
* @file Scene1.cpp
* 
* Scene 1 ("Easy" scene) implementation
*/
Scene1::Scene1(shared_ptr<cGenericHapticDevice> a_hapticDevice):GenericScene(a_hapticDevice){    

    hapticDevice = a_hapticDevice;
   
    // Describe the material to be used for obstacles
    cMaterial matBase;
    matBase.setGrayLevel(0.3);
    matBase.setStiffness(100);
    matBase.setDynamicFriction(0.5);
    matBase.setStaticFriction(0.9);
    
    // Set up static obstacles
    borderSetup({ 0.005,0.5,0.02 }, { -0.20, -0.05, -0.2 + toolRadius }, {0,0,0}, matBase);
    borderSetup({ 0.005, 0.5, 0.02 }, { -0.10, 0.05, -0.2 }, { 0,0,0 }, matBase);
    borderSetup({ 0.005, 0.5, 0.02 }, { 0.015, -0.05, -0.2 }, { 0,0,0 }, matBase);
    borderSetup({ 0.005, 0.5, 0.02 }, { 0.05, 0.05, -0.2 }, { 0,0,0 }, matBase);
    borderSetup({ 0.005, 0.5, 0.02 }, { 0.15, -0.05, -0.2 }, { 0,0,0 }, matBase);
    borderSetup({ 0.005, 0.5, 0.02 }, { 0.25, 0.05, -0.2 }, { 0,0,0 }, matBase);

    // Set up list of destinations
    destinations.push_back(cVector3d(0.28,0.28,-0.2+0.00025));
    destinations.push_back(cVector3d(-0.28,-0.28,-0.2+0.00025));

    // Set up list of checkpoints
    checkpoints.push_back(cVector3d(-0.26, 0.23, -0.20025));
    checkpointsRange.push_back(0.04);
    checkpoints.push_back(cVector3d(-0.16, 0.23, -0.20025));
    checkpointsRange.push_back(0.04);
    checkpoints.push_back(cVector3d(-0.16, -0.23, -0.20025));
    checkpointsRange.push_back(0.04);
    checkpoints.push_back(cVector3d(-0.06, -0.22, -0.20025));
    checkpointsRange.push_back(0.04);
    checkpoints.push_back(cVector3d(-0.06, 0.22, -0.20025));
    checkpointsRange.push_back(0.04);
    checkpoints.push_back(cVector3d(0.03, 0.22, -0.20025));
    checkpointsRange.push_back(0.04);
    checkpoints.push_back(cVector3d(0.03, -0.22, -0.20025));
    checkpointsRange.push_back(0.04);
    checkpoints.push_back(cVector3d(0.10, -0.22, -0.20025));
    checkpointsRange.push_back(0.04);
    checkpoints.push_back(cVector3d(0.1, 0.22, -0.20025));
    checkpointsRange.push_back(0.04);
    checkpoints.push_back(cVector3d(0.2, 0.22, -0.20025));
    checkpointsRange.push_back(0.04);
    checkpoints.push_back(cVector3d(0.2, -0.23, -0.20025));
    checkpointsRange.push_back(0.04);
    checkpoints.push_back(cVector3d(0.28, -0.23, -0.20025));
    checkpointsRange.push_back(0.04);

    // Set up target 3D element
    target = new cBulletCylinder(bulletWorld, 0.0005, toolRadius*1.5);
    bulletWorld->addChild(target);
    target->createAABBCollisionDetector(toolRadius);
    target->setMaterial(matBase);
    target->buildDynamicModel();
    target->m_material->setPurpleIndigo();
    target->m_bulletRigidBody->setCollisionFlags(target->m_bulletRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    target->m_bulletRigidBody->setUserPointer(target);

    /** Set up fuzzy engine */
    engineSetup();

}

void Scene1::initWaypoints(){
    if (target->getLocalPos().equals(destinations[1]) && last_waypoint_index == 12) {
        last_waypoint_index = 13;
        waypoint_index = 12;
    }
    else if((target->getLocalPos()).equals(destinations[0])){
        last_waypoint_index = 3;
        waypoint_index = 4;
    }
    else if((target->getLocalPos()).equals(destinations[1])){
        last_waypoint_index = 7; 
        waypoint_index = 8;
    }
}

void Scene1::generateWaypoints(cVector3d positionSphere, cVector3d positionTarget){
    if (positionTarget.equals(destinations[0])) {
        for (int i = 0; i < checkpoints.size(); i++) {
            waypoints.push_back(checkpoints[i]);
            waypointsRange.push_back(checkpointsRange[i]);
        }

        waypoints.push_back(positionTarget);
        waypointsRange.push_back(0.005);
        waypoints.push_back(checkpoints.back());
        waypointsRange.push_back(checkpointsRange.back());
    }
    else if(positionTarget.equals(destinations[1])){
        for (int i = checkpoints.size() - 1; i >= 0; i--) {
            waypoints.push_back(checkpoints[i]);
            waypointsRange.push_back(checkpointsRange[i]);
        }
        
        waypoints.push_back(positionTarget);
        waypointsRange.push_back(0.01);
        waypoints.push_back(checkpoints.back());
        waypointsRange.push_back(checkpointsRange.back());
    }
}

void Scene1::engineSetup() {
    using namespace fl;

    InputVariable* time = new InputVariable;
    time->setName("time");
    time->setDescription("");
    time->setEnabled(true);
    time->setRange(0.000, 120.000);
    time->setLockValueInRange(false);
    time->addTerm(new Bell("acceptable", 0.000, 20.000, 5.000));
    time->addTerm(new Gaussian("slow", 50.000, 10.000));
    time->addTerm(new Bell("veryslow", 120.000, 40.000, 5.000));
    engine->addInputVariable(time);

    InputVariable* collisions = new InputVariable;
    collisions->setName("collisions");
    collisions->setDescription("");
    collisions->setEnabled(true);
    collisions->setRange(0.000, 500.000);
    collisions->setLockValueInRange(false);
    collisions->addTerm(new Trapezoid("flawless", 0.000, 0.000, 75.000, 100.000));
    collisions->addTerm(new Trapezoid("good", 0.000, 100.000, 150.000, 200.000));
    collisions->addTerm(new Trapezoid("OK", 100.000, 200.000, 250.000, 300.000));
    collisions->addTerm(new Trapezoid("bad", 200.000, 300.000, 350.000, 400.000));
    collisions->addTerm(new Trapezoid("terrible", 350.000, 400.000, 500.000, 99999.000));
    engine->addInputVariable(collisions);

    OutputVariable* alphachange = new OutputVariable;
    alphachange->setName("alphachange");
    alphachange->setDescription("");
    alphachange->setEnabled(true);
    alphachange->setRange(-1.000, 1.000);
    alphachange->setLockValueInRange(false);
    alphachange->setAggregation(new Maximum);
    alphachange->setDefuzzifier(new Centroid(100));
    alphachange->setDefaultValue(fl::nan);
    alphachange->setLockPreviousValue(false);
    alphachange->addTerm(new Trapezoid("greatdeduction", -1.000, -1.000, -0.800, -0.600));
    alphachange->addTerm(new Trapezoid("smalldeduction", -0.800, -0.600, -0.400, -0.200));
    alphachange->addTerm(new Gaussian("menialchange", 0.000, 0.100));
    alphachange->addTerm(new Trapezoid("smallincrease", 0.200, 0.400, 0.600, 0.800));
    alphachange->addTerm(new Trapezoid("greatincrease", 0.600, 0.800, 1.000, 1.000));
    engine->addOutputVariable(alphachange);

    RuleBlock* ruleBlock = new RuleBlock;
    ruleBlock->setName("");
    ruleBlock->setDescription("");
    ruleBlock->setEnabled(true);
    ruleBlock->setConjunction(new Minimum);
    ruleBlock->setDisjunction(new Maximum);
    ruleBlock->setImplication(new Minimum);
    ruleBlock->setActivation(new General);
    ruleBlock->addRule(Rule::parse("if time is acceptable and collisions is flawless then alphachange is greatincrease", engine));
    ruleBlock->addRule(Rule::parse("if time is acceptable and collisions is good then alphachange is smallincrease", engine));
    ruleBlock->addRule(Rule::parse("if time is acceptable and collisions is OK then alphachange is menialchange", engine));
    ruleBlock->addRule(Rule::parse("if time is acceptable and collisions is bad then alphachange is smalldeduction", engine));
    ruleBlock->addRule(Rule::parse("if time is acceptable and collisions is terrible then alphachange is smalldeduction", engine));
    ruleBlock->addRule(Rule::parse("if time is slow and collisions is flawless then alphachange is smallincrease", engine));
    ruleBlock->addRule(Rule::parse("if time is slow and collisions is good then alphachange is menialchange", engine));
    ruleBlock->addRule(Rule::parse("if time is slow and collisions is OK then alphachange is smalldeduction", engine));
    ruleBlock->addRule(Rule::parse("if time is slow and collisions is bad then alphachange is smalldeduction", engine));
    ruleBlock->addRule(Rule::parse("if time is slow and collisions is terrible then alphachange is smalldeduction", engine));
    ruleBlock->addRule(Rule::parse("if time is veryslow and collisions is flawless then alphachange is menialchange", engine));
    ruleBlock->addRule(Rule::parse("if time is veryslow and collisions is good then alphachange is smalldeduction", engine));
    ruleBlock->addRule(Rule::parse("if time is veryslow and collisions is OK then alphachange is smalldeduction", engine));
    ruleBlock->addRule(Rule::parse("if time is veryslow and collisions is bad then alphachange is smalldeduction", engine));
    ruleBlock->addRule(Rule::parse("if time is veryslow and collisions is terrible then alphachange is greatdeduction", engine));
    engine->addRuleBlock(ruleBlock);
}

