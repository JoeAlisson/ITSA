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
#include <Connection.hpp>

using Veins::PedestrianMobility;
using Veins::AnnotationManager;

class Pedestrian11p : public BaseWaveApplLayer {
	public:
		virtual void initialize(int stage);
		virtual void preInitialize(Client* connection) {
		    this->connection = connection;
		}
	protected:
		Client* connection;
		PedestrianMobility* mobility;
		AnnotationManager* annotations;
		simtime_t lastDroveAt;
		bool sentMessage;
		virtual void handlePositionUpdate(cObject* obj);
		WaveShortMessage* convertCoordToLonLat(WaveShortMessage* wsm);
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
