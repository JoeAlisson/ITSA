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
 *  @author: Alisson Oliveira
 *
 *  Updated on: Jan 28, 2015
 */
#include <Pedestrian11p.h>

using Veins::PedestrianMobilityAccess;
using Veins::AnnotationManagerAccess;
using Veins::TraCICoord;


Define_Module(Pedestrian11p);

void Pedestrian11p::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);

    if (stage == 0) {
        mobility = PedestrianMobilityAccess().get(getParentModule());
        annotations = AnnotationManagerAccess().getIfExists();
        ASSERT(annotations);
        sentMessage = false;
        lastPacketReceived = -1;
        lastDroveAt = simTime();
    }
}

void Pedestrian11p::onBeacon(WaveShortMessage* wsm) {
    //send data through Connection
    handleWSM(wsm);
}

void Pedestrian11p::onData(WaveShortMessage* wsm) {
    findHost()->getDisplayString().updateWith("r=16,green");
    annotations->scheduleErase(1,
            annotations->drawLine(wsm->getSenderPos(), mobility->getPositionAt(simTime()), "blue"));

    //send data through Bluetooth
    handleWSM(wsm);

    if (!sentMessage)
        sendMessage(wsm->getWsmData());
}

void Pedestrian11p::sendMessage(std::string blockedRoadId) {
    sentMessage = true;
    t_channel channel = dataOnSch ? type_SCH : type_CCH;
    WaveShortMessage* wsm = prepareWSM("data", dataLengthBits, channel, dataPriority, -1, 2);
    wsm->setWsmData(blockedRoadId.c_str());
    sendWSM(wsm);
}

void Pedestrian11p::handlePositionUpdate(cObject* obj) {
    BaseWaveApplLayer::handlePositionUpdate(obj);
    lastDroveAt = simTime();
}

void Pedestrian11p::sendWSM(WaveShortMessage* wsm) {
    sendDelayedDown(wsm, individualOffset);
}

void Pedestrian11p::handleWSM(WaveShortMessage* wsm) {
   if(wsm->getTreeId() == lastPacketReceived) {
       return;
   }
   lastPacketReceived = wsm->getTreeId();
   WaveShortMessage* cop = transformPosition(wsm);
   Coord pos = cop->getSenderPos();
   if(std::string(cop->getName()) == "data_route"){
       connection->sendPacket(new W_RoutePacket(cop->getSenderAddress(), cop->getPsc(), cop->getPsid(), cop->getWsmData(), pos.x, pos.y, pos.z));
   } else {
       connection->sendPacket(new W_VehiclePacket(cop->getSenderAddress(),cop->getPsc(),cop->getPsid(), pos.x, pos.y, pos.z));
   }
}

WaveShortMessage* Pedestrian11p::transformPosition(WaveShortMessage* wsm) {
    WaveShortMessage* cop = wsm->dup();
    std::pair<double, double> lonlat = mobility->getCommandInterface()->positionConversionLonLat(mobility->getManager()->omnet2traci(cop->getSenderPos()));
    Coord pos(lonlat.first, lonlat.second);
    cop->setSenderPos(pos);
    return cop;
}

void Pedestrian11p::handleLowerMsg(cMessage* msg) {
    if(std::string(msg->getName()) == "data_route"){
        WaveShortMessage* wsm = dynamic_cast<WaveShortMessage*>(msg);
        ASSERT(wsm);
        handleWSM(wsm);
        delete(msg);
    } else {
        BaseWaveApplLayer::handleLowerMsg(msg);
    }
}
