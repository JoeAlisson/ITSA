#include "Pedestrian11p.h"

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
    mobility->handleReceiveWSM(wsm);

}

void Pedestrian11p::onData(WaveShortMessage* wsm) {
    mobility->handleReceiveWSM(wsm);
    findHost()->getDisplayString().updateWith("r=16,green");
    annotations->scheduleErase(1,
            annotations->drawLine(wsm->getSenderPos(), mobility->getPositionAt(simTime()), "blue"));

    //send data through Bluetooth
    mobility->handleReceiveWSM(wsm);

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
