/*
 * Copyright (C) 2018 IRT GmbH
 *
 * Author:
 *  Fabian Sattler
 *
 * This file is a part of IRT DAB library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 */

#ifndef DABSERVICECOMPONENTMSCPACKETDATA_H
#define DABSERVICECOMPONENTMSCPACKETDATA_H

#include "dabservicecomponent.h"

class DabServiceComponentMscPacketData : public DabServiceComponent {

public:
    explicit DabServiceComponentMscPacketData();
    virtual ~DabServiceComponentMscPacketData();

    virtual uint16_t getDataServiceComponentId() const;
    virtual uint8_t  getDataServiceComponentType() const;
    virtual uint16_t getPacketAddress() const;
    virtual uint16_t getCaOrganization() const;
    virtual bool isDataGroupTransportUsed() const;

    virtual void setDataServiceComponentId(uint16_t scid);
    virtual void setPacketAddress(uint16_t packAddr);
    virtual void setCaOrganization(uint16_t caOrg);
    virtual void setIsDataGroupTransportUsed(bool dataGroupsUsed);
    virtual void setDataServiceComponentType(uint8_t dscty);

    virtual void componentMscDataInput(const std::vector<uint8_t>& mscData, bool synchronized) override;
    virtual void flushBufferedData() override;

private:
    static constexpr uint8_t PACKETLENGTH[4][2] {
        //PacketLength / Packet datafield (useful) length
        {24, 19},
        {48, 43},
        {72, 67},
        {96, 91}
    };

    enum FIRST_LAST_PACKET {
        INTERMEDIATE,
        LAST,
        FIRST,
        ONE_AND_ONLY
    };

    struct MscPacketData {
        uint8_t continuityIndex;
        DabServiceComponentMscPacketData::FIRST_LAST_PACKET firstLast;
        uint16_t packetAddress;
        bool commandFlag;
        std::vector<uint8_t> packetData;
    };

private:
    std::string m_logTag = "[DabServiceComponentMscPacketData]";

    uint16_t m_serviceComponentId{0xFFFF};
    uint8_t m_dscty{0xFF};
    uint16_t m_packetAddress{0xFFFF};
    uint16_t m_caOrg{0x0000};
    bool m_dataGroupsUsed{false};

    MscPacketData m_mscPacket;

    std::vector<uint8_t> m_unsyncDataBuffer;
};

#endif // DABSERVICECOMPONENTMSCPACKETDATA_H
