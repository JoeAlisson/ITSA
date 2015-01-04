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
 *  Updated on: Jan 03, 2015
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
        lastDroveAt = simTime();
    }
}

void Pedestrian11p::onBeacon(WaveShortMessage* wsm) {
    //myId
    DBG << "Eu sou  " << myId << " recebendo msg de " << wsm->getSenderAddress() << " com dados = " << wsm->getWsmData() << std::endl;

    //send data through Bluetooth
    handlerWSM(wsm);
}

void Pedestrian11p::onData(WaveShortMessage* wsm) {
    findHost()->getDisplayString().updateWith("r=16,green");
    annotations->scheduleErase(1,
            annotations->drawLine(wsm->getSenderPos(), mobility->getPositionAt(simTime()), "blue"));

    //send data through Bluetooth
    handlerWSM(wsm);

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

void Pedestrian11p::handlerWSM(WaveShortMessage* wsm) {
   WaveShortMessage* cop = wsm->dup();
    std::pair<double, double> lonlat = mobility->getCommandInterface()->positionConversionLonLat(mobility->getManager()->omnet2traci(cop->getSenderPos()));
    Coord pos(lonlat.first, lonlat.second);
    cop->setSenderPos(pos);
    connection->sendPacket(new W_WSMPacket(cop));

}
