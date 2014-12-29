//
// Copyright (C) 2006-2012 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
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
#include "modules/mobility/traci/TraCICommandInterface.h"
#include <InfoBusScenarioManager.h>

/**
 * @brief
 * Used in modules created by the TraCIScenarioManager.
 *
 * This module relies on the TraCIScenarioManager for state updates
 * and can not be used on its own.
 *
 * See the Veins website <a href="http://veins.car2x.org/"> for a tutorial, documentation, and publications </a>.
 *
 * @author Christoph Sommer, David Eckhoff, Luca Bedogni, Bastian Halmos, Stefan Joerer
 *
 * @see TraCIScenarioManager
 * @see TraCIScenarioManagerLaunchd
 *
 * @ingroup mobility
 */

namespace Veins {

class PedestrianMobility : public BaseMobility
{
	public:
		PedestrianMobility() : BaseMobility(), isPreInitialized(false) {}
		virtual void initialize(int);
		virtual void finish();
		virtual void handleSelfMsg(cMessage *msg);
		virtual void preInitialize(int external_id, const Coord& position, std::string road_id = "", double speed = -1, double angle = -1);
		virtual void nextPosition(const Coord& position, std::string road_id = "", double speed = -1, double angle = -1);
		virtual void changePosition();
		virtual void updateDisplayString();
		virtual void setExternalId(int external_id) {
			this->external_id = external_id;
		}
		virtual int getExternalId() const {
			return external_id;
		}
		virtual double getAntennaPositionOffset() const {
			return antennaPositionOffset;
		}
		virtual Coord getPositionAt(const simtime_t& t) const {
			return move.getPositionAt(t) ;
		}

		virtual TraCICommandInterface* getCommandInterface() const {
			return getManager()->getCommandInterface();
		}

		virtual InfoBusScenarioManager* getManager() const {
		    if(!manager) manager = InfoBusScenarioManagerAccess().get();
		    return manager;
		}

	protected:
		bool debug; /**< whether to emit debug messages */

		cOutVector currentPosXVec; /**< vector plotting posx */
		cOutVector currentPosYVec; /**< vector plotting posy */
		cOutVector currentSpeedVec; /**< vector plotting speed */

		bool isPreInitialized; /**< true if preInitialize() has been called immediately before initialize() */

		int external_id; /**< updated by setExternalId() */
		double antennaPositionOffset; /**< front offset for the antenna on this car */

		simtime_t lastUpdate; /**< updated by nextPosition() */
		Coord roadPosition; /**< position of front bumper, updated by nextPosition() */

		mutable InfoBusScenarioManager* manager;

		double last_speed;


		virtual void fixIfHostGetsOutside(); /**< called after each read to check for (and handle) invalid positions */

		/**
		 * Calculates where the antenna of this car is, given its front bumper position
		 */
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

