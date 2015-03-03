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

#ifndef BusScenarioApp_H
#define BusScenarioApp_H

#include <BaseWaveApplLayer.h>
#include <TraCIMobility.h>
#include <Routes.h>
#include <Services.h>
#include <PedestrianScenarioManager.h>

using Veins::TraCIMobility;
using Veins::PedestrianScenarioManagerAccess;
using Veins::AnnotationManager;


class BusScenarioApp : public BaseWaveApplLayer {
	public:
		virtual void initialize(int stage);
		virtual void receiveSignal(cComponent* source, simsignal_t signalID, cObject* obj);
	protected:
		TraCIMobility* mobility;
		AnnotationManager* annotations;
		simtime_t lastDroveAt;
		bool sentMessage;
		bool isParking;
		bool sendWhileParking;
		static const simsignalwrap_t parkingStateChangedSignal;
		cMessage* sendRouteEvt;
		std::string routeId;
		std::string typeId;
		int service;
		virtual void onBeacon(WaveShortMessage* wsm);
		virtual void onData(WaveShortMessage* wsm);
		void sendMessage(std::string blockedRoadId);
		virtual void handlePositionUpdate(cObject* obj);
		virtual void handleParkingUpdate(cObject* obj);
		virtual void sendWSM(WaveShortMessage* wsm);
		virtual void handleSelfMsg(cMessage* msg);
		TraCIInterface* getTraciInterface() {
		    return PedestrianScenarioManagerAccess().get()->getTraCIInterface();
		}
};

#endif
