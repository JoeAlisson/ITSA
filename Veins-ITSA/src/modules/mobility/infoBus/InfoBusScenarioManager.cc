//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "InfoBusScenarioManager.h"
#include "modules/mobility/infoBus/PedestrianMobility.h"

using Veins::InfoBusScenarioManager;

Define_Module(InfoBusScenarioManager);

InfoBusScenarioManager::~InfoBusScenarioManager() {
}

void InfoBusScenarioManager::initialize(int stage) {
    if (stage != 1) {
        TraCIScenarioManagerLaunchd::initialize(stage);
        return;
    }

    pedestrianNameCounter = 0;
    nextNodePedestrianIndex = 0;

    pedestrianModType = par("pedestrianModType").stdstringValue();
    pedestrianModName = par("pedestrianModName").stdstringValue();

    connectionHandler = new ConnectionHandler(par("bluetoothAddress").stringValue(), this);
    connectionHandler->listen();
    TraCIScenarioManagerLaunchd::initialize(stage);

}

void InfoBusScenarioManager::finish() {
    TraCIScenarioManagerLaunchd::finish();
}

void InfoBusScenarioManager::newPedestrian(double latitude, double longitude, double altitude) {
    pedestrianInsertQueue[++pedestrianNameCounter] = traci2omnet(getCommandInterface()->positionConversionCoord(longitude, latitude, altitude));
    std::cout << "new pedestrian add into queue" << std::endl;
}

void InfoBusScenarioManager::executeOneTimestep() {
    TraCIScenarioManagerLaunchd::executeOneTimestep();

    if (!pedestrianInsertQueue.empty()) {
        std::cout << "queue is not empty " << std::endl;
        for (std::map<int, Coord>::iterator i = pedestrianInsertQueue.begin();
                i != pedestrianInsertQueue.end();) {
            addPedestrianModule(i->first, i->second);
            std::map<int, Coord>::iterator tmp = i;
            ++tmp;
            pedestrianInsertQueue.erase(i);
            i = tmp;
        }
    }
}

void InfoBusScenarioManager::addPedestrianModule(int pedestrianId,  Coord position) {

    std::cout << "Add new Module " << std::endl;
    int32_t nodeVectorIndex = nextNodePedestrianIndex++;

    cModule* parentmod = getParentModule();

    if (!parentmod)
        error("Parent Module not found");

    cModuleType* nodeType = cModuleType::get(pedestrianModType.c_str());

    if (!nodeType)
        error("Module Type \"%s\" not found", pedestrianModType.c_str());

    cModule* mod = nodeType->create(pedestrianModName.c_str(), parentmod,
            nodeVectorIndex, nodeVectorIndex);
    mod->finalizeParameters();
    mod->getDisplayString().parse("");
    mod->buildInside();
    mod->scheduleStart(simTime() + updateInterval);

    // pre-initialize TraCIMobility
    for (cModule::SubmoduleIterator iter(mod); !iter.end(); iter++) {
        cModule* submod = iter();
        PedestrianMobility* mm = dynamic_cast<PedestrianMobility*>(submod);
        if (!mm)
            continue;
        mm->preInitialize(pedestrianId, position);
    }

    mod->callInitialize();
    pedestrians[pedestrianId] = mod;

    // post-initialize TraCIMobility
    for (cModule::SubmoduleIterator iter(mod); !iter.end(); iter++) {
        cModule* submod = iter();
        PedestrianMobility* mm = dynamic_cast<PedestrianMobility*>(submod);
        if (!mm)
            continue;
        mm->changePosition();
    }
}

