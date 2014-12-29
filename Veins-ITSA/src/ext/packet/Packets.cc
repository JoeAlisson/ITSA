/*
 * packets.cc
 *
 *  Created on: 20/11/2014
 *      Author: alisson
*/

#include <packet/Packets.hpp>

// =================================== WRITABLE PACKETS =============================

W_WSMPacket::W_WSMPacket(WaveShortMessage* wsm) {
    this->wsm = wsm;
}

void W_WSMPacket::write(BluetoothConnection* con, ByteBuffer* buf) {
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
    buf->putDouble(wsm->getSenderPos().x); // latitude
    buf->putDouble(wsm->getSenderPos().y); // longitude
    buf->putDouble(wsm->getSenderPos().z); //
}

// ============================ READABLE PACKETS ====================================


R_InitPacket::R_InitPacket(InfoBusScenarioManager* manager) {
    this->manager = manager;
}


void R_InitPacket::read(ByteBuffer* buf) {
    latitude = buf->getDouble();
    longitude = buf->getDouble();
    altitude = buf->getDouble();
}

void R_InitPacket::process(BluetoothConnection* con) {
    manager->newPedestrian(latitude, longitude, altitude);
}
