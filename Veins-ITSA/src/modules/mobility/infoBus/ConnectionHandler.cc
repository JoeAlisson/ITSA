/*
 * ConnectionHandler.cpp
 *
 *  Created on: Dec 31, 2014
 *      Author: Alisson Oliveira
 */

#include <ConnectionHandler.h>

ConnectionHandler::ConnectionHandler(const char* address, Manager* manager)
    : PacketReader(new BluetoothConnection(address), manager) {
    con->setPacketReader(this);
    setPacketListener(this);
}

ConnectionHandler::~ConnectionHandler() {
    if(con) {
        delete con;
        con = NULL;
    }
}

ReadablePacket* ConnectionHandler::createPacket(short opcode) {
    ReadablePacket* packet = NULL;
    std::cout << "receiving opcode " << opcode << std::endl;
    switch (opcode) {
    case 0x00:
        packet = new R_InitPacket();
        break;
    default:
        std::cout << "Received Unknown Packet Opcode " << opcode << std::endl;
    }
    return packet;
}

void ConnectionHandler::processPacket(ReadablePacket* packet, Manager* manager) {
    packet->process(con, manager);
    delete packet;
}
