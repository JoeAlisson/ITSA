/*
 * packets.hpp
 *
 *  Created on: 20/11/2014
 *      Author: Alisson Oliveira
 */

#ifndef PACKETS_HPP_
#define PACKETS_HPP_

#include <BluetoothConnection.hpp>
#include <WaveShortMessage_m.h>

// ========================== WRITEBLE PACKETS ===============================

class W_InitPacket: public WritablePacket {
    static const short OPCODE = 0x00;
public:
    virtual void write(BluetoothConnectionClient*, ByteBuffer*) {/* just a packet flag: Connection was accepted*/};
    virtual short getOpcode() {return OPCODE;}
};

class W_WSMPacket : public WritablePacket {
    static const short OPCODE = 0x01;
    WaveShortMessage* wsm;
public:
    W_WSMPacket(WaveShortMessage*);
    virtual void write(BluetoothConnectionClient*, ByteBuffer*);
    virtual short getOpcode() { return OPCODE; }
};



// =========================== READABLE PACKETS =============================

class R_InitPacket: public ReadablePacket {
    static const short OPCODE = 0x00;
    double longitude, latitude, altitude;
    std::string address;
public:
    virtual void read(ByteBuffer*);
    virtual void process(Manager*);
};



class R_PositionUpdate: public ReadablePacket {
    static const short OPCODE = 0x02;
    double longitude, latitude, altitude;
public:
    virtual void read(ByteBuffer*);
    virtual void process(Manager*);
};

#endif /* PACKETS_HPP_ */
