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
 *  Created on: Mar 06, 2015.
 *
 *  @author: Alisson Oliveira
 *
 */
#include "RSUNotificationScenarioApp.h"

using Veins::AnnotationManagerAccess;

Define_Module(RSUNotificationScenarioApp);

void RSUNotificationScenarioApp::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        mobi = dynamic_cast<BaseMobility*> (getParentModule()->getSubmodule("mobility"));
        ASSERT(mobi);
        annotations = AnnotationManagerAccess().getIfExists();
        ASSERT(annotations);
        sendNotificationEvt = new cMessage("notifEvt");
        service = NOTIFICATION;
        scheduleAt(simTime() + 50, sendNotificationEvt);
    }
}

void RSUNotificationScenarioApp::onData(WaveShortMessage* wsm) {
    findHost()->getDisplayString().updateWith("r=16,green");
    annotations->scheduleErase(1, annotations->drawLine(wsm->getSenderPos(), mobi->getCurrentPosition(), "blue"));
    //sendWSM(wsm);
}

void RSUNotificationScenarioApp::handleSelfMsg(cMessage* msg) {
    if (msg == sendNotificationEvt) {
        t_channel channel = dataOnSch ? type_SCH : type_CCH;
        WaveShortMessage* wsm = prepareWSM("notification", dataLengthBits, channel, dataPriority, -1, 2);
        wsm->setPsid(service);
        wsm->setPsc("Anuncio da RSU");
        wsm->setWsmData("Anúncios, Promoções localizações");
        sendWSM(wsm);
        scheduleAt(simTime() + 100, sendNotificationEvt);
    } else {
        BaseWaveApplLayer::handleSelfMsg(msg);
    }
}
