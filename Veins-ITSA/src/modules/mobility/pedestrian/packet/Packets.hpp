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
 *  Created on: Nov 20, 2014
 *      @author: Alisson Oliveira
 *
 *  Updated on: Jan 02, 2015
 */

#ifndef PACKETS_HPP_
#define PACKETS_HPP_

#include <BluetoothConnection.hpp>
#include <WaveShortMessage_m.h>

// ========================== WRITEBLE PACKETS ===============================

class W_InitPacket: public WritablePacket {
    static const uint16_t OPCODE = 0x00;
public:
    virtual void write(BluetoothConnectionClient*, ByteBuffer*) {/* just a packet flag: Connection was accepted*/};
    virtual short getOpcode() {return OPCODE;}
};

class W_ClosePacket: public WritablePacket {
    static const uint16_t OPCODE = 0xFF;
public:
    virtual void write(BluetoothConnectionClient*, ByteBuffer*){/* just a packet flag: Connection will be closed */}
    virtual short getOpcode() {return OPCODE;}
};

class W_WSMPacket : public WritablePacket {
    static const uint16_t OPCODE = 0x01;
    WaveShortMessage* wsm;
public:
    W_WSMPacket(WaveShortMessage*);
    virtual void write(BluetoothConnectionClient*, ByteBuffer*);
    virtual short getOpcode() { return OPCODE; }
};


// =========================== READABLE PACKETS =============================

class R_InitPacket: public ReadablePacket {
    double longitude, latitude, altitude;
    std::string address;
public:
    static const uint16_t OPCODE = 0x00;
    virtual void read(ByteBuffer*);
    virtual void process(Manager*);
};

class R_ClosePacket: public ReadablePacket {
public:
    static const uint16_t OPCODE = 0xFF;
    virtual void read(ByteBuffer*) {/* just a packet flag: Connection was closed */}
    virtual void process(Manager*);
};



class R_PositionUpdate: public ReadablePacket {
    double longitude, latitude, altitude;
public:
    static const uint16_t OPCODE = 0x02;
    virtual void read(ByteBuffer*);
    virtual void process(Manager*);
};

#endif /* PACKETS_HPP_ */