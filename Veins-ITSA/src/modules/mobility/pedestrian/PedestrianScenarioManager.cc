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

/**
 *
 *  @author Alisson Oliveira
 *
 *  Updated on: Jan 03, 2015
 */

#include <PedestrianScenarioManager.h>
#include <PedestrianMobility.h>
#include <Pedestrian11p.h>

using Veins::PedestrianScenarioManager;

Define_Module(PedestrianScenarioManager);

PedestrianScenarioManager::~PedestrianScenarioManager() {
}

void PedestrianScenarioManager::handleSelfMsg(cMessage *msg) {
    TraCIScenarioManager::handleSelfMsg(msg);
    if (msg == connectAndStartTrigger) {
        connectionHandler->listen();
    }
}

void PedestrianScenarioManager::initialize(int stage) {
    if (stage != 1) {
        TraCIScenarioManagerLaunchd::initialize(stage);
        return;
    }

    pedestrianNameCounter = 0;
    nextNodePedestrianIndex = 0;

    pedestrianModType = par("pedestrianModType").stdstringValue();
    pedestrianModName = par("pedestrianModName").stdstringValue();

    connectionHandler = new ConnectionHandler(
            par("bluetoothAddress").stringValue(), this);
    TraCIScenarioManagerLaunchd::initialize(stage);

}

void PedestrianScenarioManager::finish() {
    TraCIScenarioManagerLaunchd::finish();
}

void PedestrianScenarioManager::newPedestrian(
        BluetoothConnectionClient* connection, double latitude,
        double longitude, double altitude) {
    pedestrianToUpdate[connection] = new PedestrianPosition(latitude, longitude,
            altitude);
}

void PedestrianScenarioManager::updatePedestrian(BluetoothConnectionClient* connection, double latitude, double longitude, double altitude) {

    PedestrianPosition* pos = pedestrianToUpdate[connection];
    if (pos) {
        pos->latitude = latitude;
        pos->altitude = altitude;
        pos->longitude = longitude;
    } else {
        // pedestrian already inserted into simulation.
        pedestrianToUpdate[connection] = new PedestrianPosition(latitude, longitude, altitude);
    }
}

Coord PedestrianScenarioManager::pedestrianPositionToCoord(PedestrianPosition* pos) {
    return traci2omnet(getCommandInterface()->positionConversionCoord(pos->longitude, pos->latitude, pos->altitude));
}

void PedestrianScenarioManager::executeOneTimestep() {
    TraCIScenarioManagerLaunchd::executeOneTimestep();

    for (std::map<BluetoothConnectionClient*, PedestrianPosition*>::iterator i = pedestrianToUpdate.begin(); i != pedestrianToUpdate.end();) {
        std::cout << "Executing timestep " << std::endl;
        std::cout << i->first->getAddress() << std::endl;
        cModule* mod = pedestrians[i->first];
        if (!mod) {
            addPedestrianModule(i->first, pedestrianPositionToCoord(i->second));
        } else {
            PedestrianMobility* mobility = PedestrianMobilityAccess().get(mod);
            mobility->nextPosition(pedestrianPositionToCoord(i->second));
        }
        delete i->second;
        std::map<BluetoothConnectionClient*, PedestrianPosition*>::iterator tmp = i;
        ++tmp;
        pedestrianToUpdate.erase(i);
        i = tmp;
    }
}

void PedestrianScenarioManager::addPedestrianModule(BluetoothConnectionClient* connection, Coord position) {
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

    // pre-initialize PedestrianMobility
    PedestrianMobility* mobility = PedestrianMobilityAccess().get(mod);

    Pedestrian11p* appl = Pedestrian11pAccess().get(mod);

    appl->preInitialize(connection);
    mobility->preInitialize(connection, position);
    mod->callInitialize();
    pedestrians[connection] = mod;
    // post-initialize PedestrianMobility
    mobility->changePosition();
}

void PedestrianScenarioManager::handleConnection(BluetoothConnectionClient* connection) {
    connection->sendPacket(new W_InitPacket());
    connectionHandler->onConnected();
}

void PedestrianScenarioManager::onDisconnection(BluetoothConnectionClient* connection) {
    std::cout << "handling disconnection " << std::endl;
    pedestrianToUpdate.erase(connection);
    cModule* mod = pedestrians[connection];

    if (mod) {
        std::cout << "Unregister " << std::endl;
        cc->unregisterNic(mod->getSubmodule("nic"));
        mod->callFinish();
        mod->deleteModule();
        pedestrians.erase(connection);
    }
    std::cout << "Closing connection " << std::endl;
    connection->closeConnection();
    std::cout << "on Disconnect " << std::endl;
    connectionHandler->onDisconnected();
    std::cout << "Disconnection handled " << std::endl;
}
