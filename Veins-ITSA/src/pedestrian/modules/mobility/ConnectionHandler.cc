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
 *  Created on: Dec 31, 2014
 *      @author: Alisson Oliveira
 *
 *  Updated on: Jan 03, 2015
 */

#include <ConnectionHandler.h>


ConnectionHandler::ConnectionHandler(Server* server, Manager* manager, int maxConnections) :
        PacketReader(server, manager) {
    server->setPacketReader(this);
    setPacketListener(this);
    this->maxConnections = maxConnections;
    activeConnections = 0;
}


ConnectionHandler::~ConnectionHandler() {
    delete server;
    server = NULL;
}

void ConnectionHandler::listen() {
    if (activeConnections >= maxConnections) return;
    server->acceptConnection();
}

ReadablePacket* ConnectionHandler::createPacket(uint16_t opcode, Client* con) {
    ReadablePacket* packet = NULL;
    switch (opcode) {
    case R_InitPacket::OPCODE:
        packet = new R_InitPacket();
        break;
    case R_PositionUpdate::OPCODE:
        packet = new R_PositionUpdate();
        break;
    default:
        std::cout << "Received Unknown Packet Opcode " << opcode << std::endl;
    }
    if(packet != NULL) {
        packet->setConnection(con);
    }
    return packet;
}

void ConnectionHandler::processPacket(ReadablePacket* packet, Manager* manager) {
    packet->process(manager);
}

void ConnectionHandler::onConnected() {
    ++activeConnections;
    listen();
}

void ConnectionHandler::onDisconnected() {
    --activeConnections;
    listen();
}
