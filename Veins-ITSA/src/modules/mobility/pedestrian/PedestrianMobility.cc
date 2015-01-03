//
//
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

#include <PedestrianMobility.h>

using Veins::PedestrianMobility;

Define_Module(Veins::PedestrianMobility);

namespace {
const double MY_INFINITY = (
        std::numeric_limits<double>::has_infinity ?
                std::numeric_limits<double>::infinity() :
                std::numeric_limits<double>::max());

double roadIdAsDouble(std::string road_id) {
    std::istringstream iss(road_id);
    double d;
    if (!(iss >> d))
        return MY_INFINITY;
    return d;
}
}

void PedestrianMobility::initialize(int stage) {
    if (stage == 0) {
        BaseMobility::initialize(stage);

        debug = par("debug");
        antennaPositionOffset = par("antennaPositionOffset");

        //ASSERT(isPreInitialized);
        isPreInitialized = false;

        Coord nextPos = calculateAntennaPosition(roadPosition);
        nextPos.z = move.getCurrentPosition().z;

        move.setStart(nextPos);

        manager = 0;
        last_speed = -1;
    } else if (stage == 1) {
        // don't call BaseMobility::initialize(stage) -- our parent will take care to call changePosition later
    } else {
        BaseMobility::initialize(stage);
    }

}

void PedestrianMobility::finish() {
    isPreInitialized = false;
}

void PedestrianMobility::handleSelfMsg(cMessage *msg) {

}


void PedestrianMobility::handleReceiveWSM(WaveShortMessage* wsm) {

}

void PedestrianMobility::preInitialize(BluetoothConnectionClient* con, const Coord& position, std::string road_id, double speed, double angle) {
    this->con  = con;
    this->lastUpdate = 0;
    this->roadPosition = position;
    this->antennaPositionOffset = par("antennaPositionOffset");

    Coord nextPos = calculateAntennaPosition(roadPosition);
    nextPos.z = move.getCurrentPosition().z;

    move.setStart(nextPos);
    move.setDirectionByVector(Coord(cos(angle), -sin(angle)));
    move.setSpeed(speed);

    isPreInitialized = true;
}

void PedestrianMobility::nextPosition(const Coord& position, std::string road_id, double speed, double angle) {
    if (debug)
        EV << "Pedestrian nextPosition " << position.x << " " << position.y << " " << road_id << " " << speed << " " << angle << std::endl;
    isPreInitialized = false;
    this->roadPosition = position;
    changePosition();
}

void PedestrianMobility::changePosition() {
    Coord nextPos = calculateAntennaPosition(roadPosition);
    nextPos.z = move.getCurrentPosition().z;

    this->lastUpdate = simTime();

    move.setStart(Coord(nextPos.x, nextPos.y, move.getCurrentPosition().z)); // keep z position
    if (ev.isGUI())
        updateDisplayString();
    fixIfHostGetsOutside();
    updatePosition();
}

void PedestrianMobility::updateDisplayString() {

}

void PedestrianMobility::fixIfHostGetsOutside() {
    Coord pos = move.getStartPos();
    Coord dummy = Coord::ZERO;
    double dum;

    bool outsideX = (pos.x < 0) || (pos.x >= playgroundSizeX());
    bool outsideY = (pos.y < 0) || (pos.y >= playgroundSizeY());
    bool outsideZ = (!world->use2D())
            && ((pos.z < 0) || (pos.z >= playgroundSizeZ()));
    if (outsideX || outsideY || outsideZ) {
        error("Tried moving host to (%f, %f) which is outside the playground",
                pos.x, pos.y);
    }

    handleIfOutside(RAISEERROR, pos, dummy, dummy, dum);
}

Coord PedestrianMobility::calculateAntennaPosition(const Coord& vehiclePos) const {
    Coord corPos;
    if (antennaPositionOffset >= 0.001) {
        //calculate antenna position of vehicle according to antenna offset
        corPos = Coord(vehiclePos.x - antennaPositionOffset * cos(-1), vehiclePos.y + antennaPositionOffset * sin(-1), vehiclePos.z);
    } else {
        corPos = Coord(vehiclePos.x, vehiclePos.y, vehiclePos.z);
    }
    return corPos;
}
