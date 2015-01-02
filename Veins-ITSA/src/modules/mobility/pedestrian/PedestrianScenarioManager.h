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

#ifndef __VEINS_INFOBUSSCENARIOMANAGER_H_
#define __VEINS_INFOBUSSCENARIOMANAGER_H_

#include <omnetpp.h>
#include <TraCIScenarioManagerLaunchd.h>
#include <ConnectionHandler.h>

/**
 *
 * @author Alisson Oliveira
 */
namespace Veins {

class PedestrianScenarioManager : public TraCIScenarioManagerLaunchd, public Manager
{
  public:
    virtual ~PedestrianScenarioManager();
    virtual void initialize(int stage);
    virtual void finish();
    virtual void newPedestrian(BluetoothConnectionClient*, double, double, double);
  protected:
    std::map<BluetoothConnectionClient*, cModule*> pedestrians;
    std::string pedestrianModType;
    std::string pedestrianModName;
    uint32_t pedestrianNameCounter;
    size_t nextNodePedestrianIndex;
    std::map<BluetoothConnectionClient*, Coord > pedestrianInsertQueue;
    ConnectionHandler* connectionHandler;
    virtual void addPedestrianModule(BluetoothConnectionClient*, Coord);
    virtual void onForcedDisconnection(BluetoothConnectionClient* connection);
    virtual void handleConnection(BluetoothConnectionClient* connection);
    virtual void executeOneTimestep();

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
