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

/*
 *  Created on: Mar 09, 2015.
 *
 *  @author: Alisson Oliveira
 *
 */
#include "TLScenarioApp.h"
#include <sstream>

using Veins::AnnotationManagerAccess;
using Veins::PedestrianScenarioManagerAccess;

Define_Module(TLScenarioApp);

void TLScenarioApp::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        mobi = dynamic_cast<BaseMobility*> (getParentModule()->getSubmodule("mobility"));
        ASSERT(mobi);
        annotations = AnnotationManagerAccess().getIfExists();
        ASSERT(annotations);
        externalId = "";
        sendNotificationEvt = new cMessage("notifEvt");
        service = WARNING;
        scheduleAt(simTime() + 10, new cMessage("init"));
        scheduleAt(simTime() + 15, sendNotificationEvt);
    }
}

void TLScenarioApp::onData(WaveShortMessage* wsm) {
    findHost()->getDisplayString().updateWith("r=16,green");
    annotations->scheduleErase(1, annotations->drawLine(wsm->getSenderPos(), mobi->getCurrentPosition(), "blue"));
}

void TLScenarioApp::handleSelfMsg(cMessage* msg) {
    if (std::string(msg->getName()) == "init") {
        traci = PedestrianScenarioManagerAccess().get()->getTraCIInterface();
        std::list<std::string> trafficlights = traci->getTLIds();
        for(std::list<std::string>::iterator i = trafficlights.begin(); i != trafficlights.end(); i++) {
            if((*i)[0] != 'c') {
                externalId = *i;
                break;
            }
        }
        states = traci->getTLState(externalId);
    }
    else if (msg == sendNotificationEvt) {
        currentState = traci->getCurrentTlState(externalId);
        int32_t nextSwitch =  traci->getTimeToNextTlState(externalId) - round(simTime().dbl() * 1000);
        std::stringstream ss;
        ss << states[currentState]  << nextSwitch;
        t_channel channel = dataOnSch ? type_SCH : type_CCH;
        WaveShortMessage* wsm = prepareWSM("tlstate", dataLengthBits, channel, dataPriority, -1, 2);
        wsm->setPsid(service);
        wsm->setPsc("Traffic Light State");
        wsm->setWsmData(ss.str().c_str());
        sendWSM(wsm);
        scheduleAt(simTime() + 15, sendNotificationEvt);
    } else {
        BaseWaveApplLayer::handleSelfMsg(msg);
    }
}
