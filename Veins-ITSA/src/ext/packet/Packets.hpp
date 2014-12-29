/*
 * packets.hpp
 *
 *  Created on: 20/11/2014
 *      Author: alisson
 */

#ifndef PACKETS_HPP_
#define PACKETS_HPP_

#include <BluetoothConnection.hpp>
#include <WaveShortMessage_m.h>
#include <InfoBusScenarioManager.h>

namespace Veins {
    class InfoBusScenarioManager;
}  // namespace infoBus

using Veins::InfoBusScenarioManager;

// ========================== WRITEBLE PACKETS ===============================

class W_WSMPacket : public WriteblePacket {
    static const short OPCODE = 0x01;
    WaveShortMessage* wsm;
public:
    W_WSMPacket(WaveShortMessage*);
    void write(BluetoothConnection*, ByteBuffer*);
    virtual short getOpcode() { return OPCODE; }
};



// =========================== READABLE PACKETS =============================


class R_InitPacket: public ReadablePacket {
    static const short OPCODE = 0x00;
    double longitude, latitude, altitude;
    InfoBusScenarioManager* manager;
public:
    R_InitPacket(InfoBusScenarioManager* );
    virtual void read(ByteBuffer*);
    virtual void process(BluetoothConnection*);
};


#endif /* PACKETS_HPP_ */
