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

/**
 *
 *  @author Alisson Oliveira
 *
 *  Updated on: Jan 03, 2015
 */

#ifndef Pedestrian11p_H
#define Pedestrian11p_H

#include <BaseWaveApplLayer.h>
#include <PedestrianMobility.h>
#include <BluetoothConnection.hpp>

using Veins::PedestrianMobility;
using Veins::AnnotationManager;

/**
 * Small IVC Demo using 11p
 */
class Pedestrian11p : public BaseWaveApplLayer {
	public:
		virtual void initialize(int stage);
		virtual void preInitialize(BluetoothConnectionClient* connection) {
		    this->connection = connection;
		}
	protected:
		BluetoothConnectionClient* connection;
		PedestrianMobility* mobility;
		AnnotationManager* annotations;
		simtime_t lastDroveAt;
		bool sentMessage;
		long int lastPacketReceived;
		virtual void onBeacon(WaveShortMessage* wsm);
		virtual void onData(WaveShortMessage* wsm);
		void sendMessage(std::string blockedRoadId);
		virtual void handlePositionUpdate(cObject* obj);
		virtual void sendWSM(WaveShortMessage* wsm);
		virtual void handleWSM(WaveShortMessage* wsm);
		virtual void handleLowerMsg(cMessage* msg);
		WaveShortMessage* transformPosition(WaveShortMessage* wsm);
};


class Pedestrian11pAccess {
    public:
        Pedestrian11p* get(cModule* host) {
            Pedestrian11p* traci = FindModule<Pedestrian11p*>::findSubModule(host);
            ASSERT(traci);
            return traci;
        };
};
#endif
