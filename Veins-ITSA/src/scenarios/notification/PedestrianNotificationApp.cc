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
 *  Created on: Mar 06, 2015.
 *
 *  @author: Alisson Oliveira
 *
 */
#include <PedestrianNotificationApp.h>

using Veins::PedestrianMobilityAccess;
using Veins::AnnotationManagerAccess;
using Veins::TraCICoord;


Define_Module(PedestrianNotificationApp);

void PedestrianNotificationApp::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        mobility = PedestrianMobilityAccess().get(getParentModule());
        annotations = AnnotationManagerAccess().getIfExists();
        ASSERT(annotations);
        sentMessage = false;
        lastDroveAt = simTime();
    }
}

void PedestrianNotificationApp::onBeacon(WaveShortMessage* wsm) {
    WaveShortMessage* cop = convertCoordToLonLat(wsm);
    Coord pos = cop->getSenderPos();
    connection->sendPacket(new W_VehiclePacket(cop->getSenderAddress(),cop->getPsc(),cop->getPsid(), pos.x, pos.y, pos.z));
}

void PedestrianNotificationApp::onData(WaveShortMessage* wsm) {
    findHost()->getDisplayString().updateWith("r=16,green");
    annotations->scheduleErase(1,
            annotations->drawLine(wsm->getSenderPos(), mobility->getPositionAt(simTime()), "blue"));

    WaveShortMessage* cop = convertCoordToLonLat(wsm);
    Coord pos = cop->getSenderPos();
    connection->sendPacket(new W_VehiclePacket(cop->getSenderAddress(),cop->getPsc(),cop->getPsid(), pos.x, pos.y, pos.z));
}

void PedestrianNotificationApp::handleLowerMsg(cMessage* msg) {
    if(std::string(msg->getName()) == "notification"){
        WaveShortMessage* wsm = dynamic_cast<WaveShortMessage*>(msg);
        ASSERT(wsm);
        connection->sendPacket(new W_NotificationPacket(wsm->getSenderAddress(), wsm->getPsc(), wsm->getPsid(), wsm->getWsmData()));
        std::cout << "Handled notification" << std::endl;
        delete(msg);
        std::cout << "deleted" << std::endl;
    } else {
        BaseWaveApplLayer::handleLowerMsg(msg);
    }
}
