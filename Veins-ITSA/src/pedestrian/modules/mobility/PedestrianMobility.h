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

#ifndef VEINS_MOBILITY_TRACI_PedestrianMobility_H
#define VEINS_MOBILITY_TRACI_PedestrianMobility_H

#include <limits>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <list>
#include <stdexcept>
#include <BaseMobility.h>
#include "FindModule.h"
#include <PedestrianScenarioManager.h>

/**
 * Based on @see TraciMobility
 *
 * @author Alisson Oliveira
 *
 * Updated on: Jan 03, 2015
 */

namespace Veins {

class PedestrianMobility : public BaseMobility {
	public:
		PedestrianMobility() : BaseMobility(), isPreInitialized(false) {}
		virtual void initialize(int);
		virtual void finish();
		virtual void handleSelfMsg(cMessage *msg);
		virtual void handleReceiveWSM(WaveShortMessage* wsm);
		virtual void preInitialize(Client* con, const Coord& position, std::string road_id = "", double speed = -1, double angle = -1);
		virtual void nextPosition(const Coord& position, std::string road_id = "", double speed = -1, double angle = -1);
		virtual void changePosition();
		virtual void updateDisplayString();
		virtual double getAntennaPositionOffset() const {
			return antennaPositionOffset;
		}
		virtual Coord getPositionAt(const simtime_t& t) const {
			return move.getPositionAt(t) ;
		}

		virtual TraCICommandInterface* getCommandInterface() const {
			return getManager()->getCommandInterface();
		}

		virtual PedestrianScenarioManager* getManager() const {
		    if(!manager) manager = PedestrianScenarioManagerAccess().get();
		    return manager;
		}

	protected:
		bool debug;


		bool isPreInitialized;

		double antennaPositionOffset;

		simtime_t lastUpdate;
		Coord roadPosition;

		mutable PedestrianScenarioManager* manager;
		Client* con;

		double last_speed;


		virtual void fixIfHostGetsOutside();

		Coord calculateAntennaPosition(const Coord& vehiclePos) const;
};
}

namespace Veins {
class PedestrianMobilityAccess
{
	public:
		PedestrianMobility* get(cModule* host) {
			PedestrianMobility* traci = FindModule<PedestrianMobility*>::findSubModule(host);
			ASSERT(traci);
			return traci;
		};
};
}

#endif

