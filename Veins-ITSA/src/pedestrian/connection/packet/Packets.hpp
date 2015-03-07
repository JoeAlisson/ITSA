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
 *  Updated on: Jan 26, 2015
 */

#ifndef PACKETS_HPP_
#define PACKETS_HPP_

#include <Connection.hpp>
#include <WaveShortMessage_m.h>

// ========================== WRITEBLE PACKETS ===============================

class W_InitPacket: public WritablePacket {
    static const uint16_t OPCODE = 0x00;
public:
    virtual void write(Client*, ByteBuffer*) {/* just a packet flag: Connection was accepted*/};
    virtual short getOpcode() {return OPCODE;}
};

class W_WSMPacket : public WritablePacket {
    static const uint16_t OPCODE = 0x01;
    WaveShortMessage* wsm;
public:
    W_WSMPacket(WaveShortMessage*);
    ~W_WSMPacket() {
        delete wsm;
    }
    virtual void write(Client*, ByteBuffer*);
    virtual short getOpcode() { return OPCODE; }
};

class W_VehiclePacket : public WritablePacket {
    static const uint16_t OPCODE = 0x03;
protected:
    int sender;
    std::string serviceContext;
    int service;
    double longitude;
    double latitude;
    double altitude;

public:
    W_VehiclePacket(int sender, std::string serviceContext, int service, double longitude, double latitude, double altitude);
    virtual void write(Client*, ByteBuffer*);
    virtual short getOpcode() { return OPCODE; }
};

class W_RoutePacket : public W_VehiclePacket {
    static const uint16_t OPCODE = 0x04;
    std::string route;
public:
    W_RoutePacket(int sender, std::string serviceContext, int service, std::string route, double longitude, double latitude, double altitude);
    virtual void write(Client*, ByteBuffer*);
    virtual short getOpcode() { return OPCODE; }
};


class W_NotificationPacket : public WritablePacket {
    static const uint16_t OPCODE = 0xFF;
protected:
    int sender;
    std::string serviceContext;
    int service;
    std::string data;
public:
    W_NotificationPacket(int sender, std::string serviceContext, int service, std::string data);
    virtual void write(Client*, ByteBuffer*);
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

class R_PositionUpdate: public ReadablePacket {
    double longitude, latitude, altitude;
public:
    static const uint16_t OPCODE = 0x02;
    virtual void read(ByteBuffer*);
    virtual void process(Manager*);
};

#endif /* PACKETS_HPP_ */
