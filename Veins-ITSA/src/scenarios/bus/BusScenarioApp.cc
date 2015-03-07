//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "BusScenarioApp.h"

using Veins::TraCIMobilityAccess;
using Veins::AnnotationManagerAccess;
using Veins::TraCICoord;

const simsignalwrap_t BusScenarioApp::parkingStateChangedSignal = simsignalwrap_t(
TRACI_SIGNAL_PARKING_CHANGE_NAME);

Define_Module(BusScenarioApp);

void BusScenarioApp::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        mobility = TraCIMobilityAccess().get(getParentModule());
        annotations = AnnotationManagerAccess().getIfExists();
        ASSERT(annotations);

        sentMessage = false;
        lastDroveAt = simTime();
        findHost()->subscribe(parkingStateChangedSignal, this);
        isParking = false;
        sendWhileParking = par("sendWhileParking").boolValue();
    } else if (stage == 1) {
        routeId = getTraciInterface()->getRouteId(mobility->getExternalId());
        typeId = getTraciInterface()->getTypeId(mobility->getExternalId());
        sendNotificationEvt = new cMessage("routeEvt");
        if (typeId == "bus" && RoutesAccess().get(routeId) != "") {
            service = PUBLIC_TRANSPORT;
            scheduleAt(simTime() + 10, sendNotificationEvt);
        }

    }
}

void BusScenarioApp::onBeacon(WaveShortMessage* wsm) {
}

void BusScenarioApp::onData(WaveShortMessage* wsm) {
    findHost()->getDisplayString().updateWith("r=16,green");
    annotations->scheduleErase(1,
            annotations->drawLine(wsm->getSenderPos(),
                    mobility->getPositionAt(simTime()), "blue"));

    if (mobility->getRoadId()[0] != ':')
        mobility->commandChangeRoute(wsm->getWsmData(), 9999);
    if (!sentMessage)
        sendMessage(wsm->getWsmData());
}

void BusScenarioApp::sendMessage(std::string blockedRoadId) {
    sentMessage = true;
    t_channel channel = dataOnSch ? type_SCH : type_CCH;
    WaveShortMessage* wsm = prepareWSM("data", dataLengthBits, channel, dataPriority, -1, 2);
    wsm->setWsmData(blockedRoadId.c_str());
    sendWSM(wsm);
}

void BusScenarioApp::receiveSignal(cComponent* source, simsignal_t signalID, cObject* obj) {
    Enter_Method_Silent();
    if (signalID == mobilityStateChangedSignal) {
        handlePositionUpdate(obj);
    } else if (signalID == parkingStateChangedSignal) {
        handleParkingUpdate(obj);
    }
}

void BusScenarioApp::handleParkingUpdate(cObject* obj) {
    isParking = mobility->getParkingState();
    if (sendWhileParking == false) {
        if (isParking == true) {
            (FindModule<BaseConnectionManager*>::findGlobalModule())->unregisterNic(
                    this->getParentModule()->getSubmodule("nic"));
        } else {
            Coord pos = mobility->getCurrentPosition();
            (FindModule<BaseConnectionManager*>::findGlobalModule())->registerNic(
                    this->getParentModule()->getSubmodule("nic"),
                    (ChannelAccess*) this->getParentModule()->getSubmodule(
                            "nic")->getSubmodule("phy80211p"), &pos);
        }
    }
}


void BusScenarioApp::handlePositionUpdate(cObject* obj) {
    BaseWaveApplLayer::handlePositionUpdate(obj);

    // stopped for for at least 10s?
    // send traffic jam message.
    if (mobility->getSpeed() < 1) {
        if (simTime() - lastDroveAt >= 10) {
            findHost()->getDisplayString().updateWith("r=16,red");
            if (!sentMessage) sendMessage(mobility->getRoadId());
        }
    } else {
        lastDroveAt = simTime();
    }
}

void BusScenarioApp::sendWSM(WaveShortMessage* wsm) {
    if (isParking && !sendWhileParking)
        return;
    sendDelayedDown(wsm, individualOffset);
}

void BusScenarioApp::handleSelfMsg(cMessage* msg) {
    if (msg == sendNotificationEvt) {
        t_channel channel = dataOnSch ? type_SCH : type_CCH;
        WaveShortMessage* wsm = prepareWSM("route", dataLengthBits, channel, dataPriority, -1, 2);
        wsm->setPsid(service);
        wsm->setPsc("bus");
        wsm->setWsmData(RoutesAccess().get(routeId).c_str());
        sendWSM(wsm);
        scheduleAt(simTime() + 10, sendNotificationEvt);
    } else {
        BaseWaveApplLayer::handleSelfMsg(msg);
    }
}
