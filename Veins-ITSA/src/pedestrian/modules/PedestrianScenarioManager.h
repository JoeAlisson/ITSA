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
 *  @author Alisson Oliveira
 *
 *  Updated on: Mar 08, 2015
 *
 */
#ifndef __VEINS_INFOBUSSCENARIOMANAGER_H_
#define __VEINS_INFOBUSSCENARIOMANAGER_H_

#include <omnetpp.h>
#include <TraCIScenarioManagerLaunchd.h>
#include <ConnectionHandler.h>
#include <TraCIInterface.h>

namespace Veins {

class PedestrianScenarioManager : public TraCIScenarioManagerLaunchd, public Manager
{
  public:
    virtual ~PedestrianScenarioManager() {}
    virtual void initialize(int stage);
    virtual void finish();
    virtual void newPedestrian(Client*, double, double, double);
    virtual void updatePedestrian(Client*, double, double, double);
    virtual void handleSelfMsg(cMessage *msg);
    TraCIInterface* getTraCIInterface() { return traci;}
  protected:
    class PedestrianPosition {
    public:
        double latitude;
        double longitude;
        double altitude;
        PedestrianPosition(double latitude, double longitude, double altitude) {
            this->latitude = latitude;
            this->longitude = longitude;
            this->altitude = altitude;
        }
    };

    std::map<Client*, cModule*> pedestrians;
    std::string pedestrianModType;
    std::string pedestrianModName;
    size_t nextNodePedestrianIndex;
    std::map<Client*, PedestrianPosition*> pedestrianToUpdate;

    std::map<std::string, cModule*> trafficLights;
    std::string TLModType;
    std::string TLModName;
    size_t nextTLIndex;

    ConnectionHandler* connectionHandler;
    TraCIInterface* traci;
    virtual void addPedestrianModule(Client*, Coord);
    virtual void handleConnection(Client* connection);
    virtual void onDisconnection(Client* connection);
    virtual void executeOneTimestep();
    virtual Coord pedestrianPositionToCoord(PedestrianPosition*);
};
}

namespace Veins {

class PedestrianScenarioManagerAccess {
    public:
        PedestrianScenarioManager* get() {
            return FindModule<PedestrianScenarioManager*>::findGlobalModule();
        };
};
 }

#endif
