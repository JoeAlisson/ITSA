/*
 * handler.cc
 *
 *  Created on: 20/11/2014
 *      Author: alisson
 */

#include <Handler.hpp>

PacketHandler::PacketHandler(BluetoothConnection* con) : PacketReader(con) {
    setPacketListener(this);
}

ReadablePacket* PacketHandler::createPacket(short opcode) {
    ReadablePacket* packet = NULL;
    std::cout << "receiving opcode " << opcode << std::endl;
    switch (opcode) {
    case 0x00:
        break;
    default:
        std::cout << "Received Unknown Packet Opcode " << opcode << std::endl;
    }
    return packet;
}

void PacketHandler::processPacket(ReadablePacket* packet) {
    packet->process(con);
    delete packet;
}
