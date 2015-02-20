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
 *
 *  Created on: Nov 20, 2014
 *      @author: Alisson Oliveira
 *
 *  Updated on: Jan 03, 2015
 */

#include <packet/Packets.hpp>

#include <PedestrianScenarioManager.h>

// =================================== WRITABLE PACKETS =============================

using Veins::PedestrianScenarioManager;


W_WSMPacket::W_WSMPacket(WaveShortMessage* wsm) {
    this->wsm = wsm;
}

void W_WSMPacket::write(Client* con, ByteBuffer* buf) {
    writeString(wsm->getName(), buf);
    buf->putShort(wsm->getKind());
    buf->putInt(wsm->getWsmVersion());
    buf->putInt(wsm->getSecurityType());
    buf->putInt(wsm->getChannelNumber());
    buf->putInt(wsm->getDataRate());
    buf->putInt(wsm->getPriority());
    buf->putInt(wsm->getPsid());
    writeString(wsm->getPsc(), buf);
    buf->putInt(wsm->getWsmLength());
    writeString(wsm->getWsmData(), buf);
    buf->putInt(wsm->getSenderAddress());
    buf->putInt(wsm->getRecipientAddress());
    buf->putInt(wsm->getSerial());
    // Sender Pos
    buf->putDouble(wsm->getSenderPos().x); // longitude
    buf->putDouble(wsm->getSenderPos().y); // latitude
    buf->putDouble(wsm->getSenderPos().z); // altitude
}


W_VehiclePacket::W_VehiclePacket(int sender, std::string serviceContext, int service, double longitude, double latitude, double altitude) {
    this->sender = sender;
    this->serviceContext = serviceContext;
    this->service = service;
    this->longitude = longitude;
    this->latitude = latitude;
    this->altitude = altitude;
}

void W_VehiclePacket::write(Client* con, ByteBuffer* buf) {
    buf->putInt(sender);
    buf->putInt(service);
    writeString(serviceContext,buf);
    buf->putDouble(longitude);
    buf->putDouble(latitude);
    buf->putDouble(altitude);
}
W_RoutePacket::W_RoutePacket(int sender, std::string serviceContext, int service, std::string route, double longitude, double latitude, double altitude) :
    W_VehiclePacket(sender, serviceContext, service, longitude, latitude, altitude){
    this->route = route;
}

void W_RoutePacket::write(Client* con, ByteBuffer* buf) {
    W_VehiclePacket::write(con,buf);
    writeString(route,buf);
}

// ============================ READABLE PACKETS ====================================

void R_InitPacket::read(ByteBuffer* buf) {
    address = readString(buf);
    uint8_t flag = buf->get();
    if (flag) {
        latitude = buf->getDouble();
        longitude = buf->getDouble();
        altitude = buf->getDouble();
    } else {
        // the device's location isn't ready yet wait for the update Packet.
        latitude = 0;
        longitude = 0;
        altitude = 0;
    }
}

void R_InitPacket::process(Manager* manager) {
    ((PedestrianScenarioManager*)manager)->newPedestrian(connection,latitude, longitude,altitude);
}

void R_PositionUpdate::read(ByteBuffer* buf) {
    latitude = buf->getDouble();
    longitude = buf->getDouble();
    altitude = buf->getDouble();
}

void R_PositionUpdate::process(Manager* manager) {
    ((PedestrianScenarioManager*)manager)->updatePedestrian(connection,latitude, longitude,altitude);
}
