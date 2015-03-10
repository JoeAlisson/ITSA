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

/*
 *  Created on: Mar 09, 2015.
 *
 *  @author: Alisson Oliveira
 *
 */
#include <PedestrianTLApp.h>
#include <string>

using Veins::PedestrianMobilityAccess;
using Veins::AnnotationManagerAccess;
using Veins::TraCICoord;


Define_Module(PedestrianTLApp);

void PedestrianTLApp::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        mobility = PedestrianMobilityAccess().get(getParentModule());
        annotations = AnnotationManagerAccess().getIfExists();
        ASSERT(annotations);
        sentMessage = false;
        lastDroveAt = simTime();
    }
}

void PedestrianTLApp::onBeacon(WaveShortMessage* wsm) {
    WaveShortMessage* cop = convertCoordToLonLat(wsm);
    Coord pos = cop->getSenderPos();
    connection->sendPacket(new W_VehiclePacket(cop->getSenderAddress(),cop->getPsc(),cop->getPsid(), pos.x, pos.y, pos.z));
}

void PedestrianTLApp::onData(WaveShortMessage* wsm) {
    findHost()->getDisplayString().updateWith("r=16,green");
    annotations->scheduleErase(1,
            annotations->drawLine(wsm->getSenderPos(), mobility->getPositionAt(simTime()), "blue"));

    WaveShortMessage* cop = convertCoordToLonLat(wsm);
    Coord pos = cop->getSenderPos();
    connection->sendPacket(new W_VehiclePacket(cop->getSenderAddress(),cop->getPsc(),cop->getPsid(), pos.x, pos.y, pos.z));
}

void PedestrianTLApp::handleLowerMsg(cMessage* msg) {
    if(std::string(msg->getName()) == "tlstate"){
        WaveShortMessage* wsm = dynamic_cast<WaveShortMessage*>(msg);
        ASSERT(wsm);
        WaveShortMessage* cop = convertCoordToLonLat(wsm);
        Coord pos = cop->getSenderPos();
        std::string data(cop->getWsmData());
        int time = std::stoi(data.substr(1));
        connection->sendPacket(new W_TLStatePacket(cop->getSenderAddress(),cop->getPsc(),cop->getPsid(), data[0], time, pos.x, pos.y, pos.z));
        delete(msg);
    } else {
        BaseWaveApplLayer::handleLowerMsg(msg);
    }
}
