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

package org.car2x.veins.pedestrian.modules;

//
// Extends the TraCIScenarioManagerLaunchd for use devices with Bluetooth to handle pedestrian simulition in real devices.
//
// Connects to real devices and send data to them. So we can test the feasibility of them in vanets
//
// All other functionality is provided by the TraCIScenarioManagerLaunchd.
//
// See the Veins website <a href="http://veins.car2x.org/"> for a tutorial, documentation, and publications </a>.
//
// @author Alisson Oliveira
//
// @see TraCIMobility
// @see TraCIScenarioManagerLaunchd
//

import org.car2x.veins.modules.mobility.traci.TraCIScenarioManagerLaunchd;


simple PedestrianScenarioManager extends TraCIScenarioManagerLaunchd {
    
    parameters:
        @class(Veins::PedestrianScenarioManager);
        string pedestrianModType = default("org.car2x.veins.pedestrian.nodes.Pedestrian");  // module type to be used in the simulation for each pedestrian
        string pedestrianModName = default("pedestrian");  // module name to be used in the simulation for each pedestrian
        string trafficLightModType = default("org.car2x.veins.pedestrian.nodes.TrafficLight");  
        string trafficLightModName = default("tl");  // module name to be used in the simulation for traffic Light
        bool useBluetooth = default(true);
        string bluetoothAddress = default("14:2D:27:CD:A5:68");
        string IPAddress = default("127.0.0.1");
        int socketPort = default(3637);
        int maxConnections = default(7);
    
}
