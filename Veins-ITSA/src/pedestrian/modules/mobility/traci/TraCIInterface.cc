/*
 * TraCIInterface.cpp
 *
 *  Created on: Feb 16, 2015
 *      Author: Alisson Oliveira
 */

#include <traci/TraCIInterface.h>
#include <TraCIConstants.h>
#include <TraCIBuffer.h>

using Veins::TraCIBuffer;

TraCIInterface::TraCIInterface(TraCIConnection* connection) : TraCICommandInterface(*connection) {
    con = connection;
}

std::string TraCIInterface::getTypeId(std::string nodeId) {
   return genericString(CMD_GET_VEHICLE_VARIABLE, nodeId, VAR_TYPE, RESPONSE_GET_VEHICLE_VARIABLE);
}

std::string TraCIInterface::getTLState(std::string tlId) {
    return genericString(CMD_GET_TL_VARIABLE, tlId, TL_RED_YELLOW_GREEN_STATE, RESPONSE_GET_TL_VARIABLE);
}

std::list<std::string> TraCIInterface::getTLIds() {
    return genericStringList(CMD_GET_TL_VARIABLE, "", ID_LIST, RESPONSE_GET_TL_VARIABLE);
}

int32_t TraCIInterface::getTimeToNextTlState(std::string tlId) {
    return genericInt(CMD_GET_TL_VARIABLE, tlId, TL_NEXT_SWITCH, RESPONSE_GET_TL_VARIABLE);
}

int32_t TraCIInterface::getCurrentTlState(std::string tlId) {
    return genericInt(CMD_GET_TL_VARIABLE, tlId, TL_CURRENT_PHASE, RESPONSE_GET_TL_VARIABLE);
}

int32_t TraCIInterface::genericInt(uint8_t commandId, std::string objectId, uint8_t variableId, uint8_t responseId) {

    uint8_t resultTypeId = TYPE_INTEGER;
    int32_t res;

    TraCIBuffer buf = con->query(commandId, TraCIBuffer() << variableId << objectId);

    uint8_t cmdLength; buf >> cmdLength;
    if (cmdLength == 0) {
        uint32_t cmdLengthX;
        buf >> cmdLengthX;
    }
    uint8_t commandId_r; buf >> commandId_r;
    ASSERT(commandId_r == responseId);
    uint8_t varId; buf >> varId;
    ASSERT(varId == variableId);
    std::string objectId_r; buf >> objectId_r;
    ASSERT(objectId_r == objectId);
    uint8_t resType_r; buf >> resType_r;
    ASSERT(resType_r == resultTypeId);
    buf >> res;

    ASSERT(buf.eof());

    return res;
}


TraCICoord TraCIInterface::convertLonLatToTraCICoord(double longitude, double latitude, double altitude) {
    TraCIBuffer request;
    request << static_cast<uint8_t>(POSITION_CONVERSION) << std::string("sim0")
            << static_cast<uint8_t>(TYPE_COMPOUND) << static_cast<int32_t>(2)
            << static_cast<uint8_t>(POSITION_LON_LAT_ALT) << longitude << latitude << altitude
            << static_cast<uint8_t>(TYPE_UBYTE) << static_cast<uint8_t>(POSITION_2D);
    TraCIBuffer response = con->query(CMD_GET_SIM_VARIABLE, request);

    uint8_t cmdLength; response >> cmdLength;
    if (cmdLength == 0) {
        uint32_t cmdLengthX;
        response >> cmdLengthX;
    }
    uint8_t responseId; response >> responseId;
    ASSERT(responseId == RESPONSE_GET_SIM_VARIABLE);
    uint8_t variable; response >> variable;
    ASSERT(variable == POSITION_CONVERSION);
    std::string id; response >> id;
    uint8_t convPosType; response >> convPosType;
    ASSERT(convPosType == POSITION_2D);
    double x; response >> x;
    double y; response >> y;
    return TraCICoord(x, y);
}

std::string TraCIInterface::genericString(uint8_t commandId, std::string objectId, uint8_t variableId, uint8_t responseId) {
    uint8_t resultTypeId = TYPE_STRING;
    std::string res;

    TraCIBuffer buf = con->query(commandId, TraCIBuffer() << variableId << objectId);

    uint8_t cmdLength; buf >> cmdLength;
    if (cmdLength == 0) {
        uint32_t cmdLengthX;
        buf >> cmdLengthX;
    }
    uint8_t commandId_r; buf >> commandId_r;
    ASSERT(commandId_r == responseId);
    uint8_t varId; buf >> varId;
    ASSERT(varId == variableId);
    std::string objectId_r; buf >> objectId_r;
    ASSERT(objectId_r == objectId);
    uint8_t resType_r; buf >> resType_r;
    ASSERT(resType_r == resultTypeId);
    buf >> res;

    ASSERT(buf.eof());

    return res;
}

std::list<std::string> TraCIInterface::genericStringList(uint8_t commandId, std::string objectId, uint8_t variableId, uint8_t responseId) {

    uint8_t resultTypeId = TYPE_STRINGLIST;
    std::list<std::string> res;

    TraCIBuffer buf = con->query(commandId, TraCIBuffer() << variableId << objectId);

    uint8_t cmdLength; buf >> cmdLength;
    if (cmdLength == 0) {
        uint32_t cmdLengthX;
        buf >> cmdLengthX;
    }
    uint8_t commandId_r; buf >> commandId_r;
    ASSERT(commandId_r == responseId);
    uint8_t varId; buf >> varId;
    ASSERT(varId == variableId);
    std::string objectId_r; buf >> objectId_r;
    ASSERT(objectId_r == objectId);
    uint8_t resType_r; buf >> resType_r;
    ASSERT(resType_r == resultTypeId);
    uint32_t count; buf >> count;
    for (uint32_t i = 0; i < count; i++) {
        std::string id; buf >> id;
        res.push_back(id);
    }

    ASSERT(buf.eof());

    return res;
}
