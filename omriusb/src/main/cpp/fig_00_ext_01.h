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

#ifndef FIG_00_EXT_01_H
#define FIG_00_EXT_01_H

#include <vector>

#include "fig_00.h"

/*
 * ETS 300 401 clause 6.2.1 Basic sub-channel organization
 *
 * The Extension 1 of FIG type 0 (FIG 0/1) defines the basic sub-channel organization. Each sub-channel is described
 * explicitly by its start address (in the range 0 to 863 CUs) and (either explicitly or implicitly) by the size of
 * the sub-channel and the error coding protection mechanism employed.
 */
class Fig_00_Ext_01 : public Fig_00 {

public:
    /*
     * -----------
     * | Table 7 |
     * --------------------------------------------------------------
     * | ProtectionLevel            |  1-A  |  2-A  |  3-A  |  4-A  |
     * |------------------------------------------------------------|
     * | ConvolutionalCodingRate    |  1/4  |  3/8  |  1/2  |  3/4  |
     * |------------------------------------------------------------|
     * | Sub-Channle size (CUs)     |  12 n |  8 n  |  6 n  |  4 n  |
     * --------------------------------------------------------------
     */
    enum PROTECTIONLEVEL_OPTION_0 {
        OPTION_0_1A = 0,
        OPTION_0_2A = 1,
        OPTION_0_3A = 2,
        OPTION_0_4A = 3,
        OPTION_0_UNKNOWN = 0xFF,
    };

    /*
     * -----------
     * | Table 8 |
     * --------------------------------------------------------------
     * | ProtectionLevel            |  1-B  |  2-B  |  3-B  |  4-B  |
     * |------------------------------------------------------------|
     * | ConvolutionalCodingRate    |  4/9  |  4/7  |  4/6  |  4/5  |
     * |------------------------------------------------------------|
     * | Sub-Channle size (CUs)     |  27 n |  21 n |  18 n |  15 n |
     * --------------------------------------------------------------
     */
    enum PROTECTIONLEVEL_OPTION_1 {
        OPTION_1_1B = 0,
        OPTION_1_2B = 1,
        OPTION_1_3B = 2,
        OPTION_1_4B = 3,
        OPTION_1_UNKNOWN = 0xFF
    };

    enum PROTECTIONLEVEL {
        LEVEL_1A,
        LEVEL_2A,
        LEVEL_3A,
        LEVEL_4A,
        LEVEL_1B,
        LEVEL_2B,
        LEVEL_3B,
        LEVEL_4B
    };

    const uint8_t PROTECTIONLEVEL_SUBCHAN_SIZE[8] {
        12,
         8,
         6,
         4,
        27,
        21,
        18,
        15
    };

    const std::string PROTECTIONLEVEL_STRING[8] = {
        "1A",
        "2A",
        "3A",
        "4A",
        "1B",
        "2B",
        "3B",
        "4B"
    };

    enum PROTECTIONTYPE {
        PROTECTION_TYPE_UEP,
        PROTECTION_TYPE_EEPA_8N,
        PROTECTION_TYPE_EEPB_32N
    };

    /*
     * SubChId (Sub-channel Identifier): this 6-bit field, coded as an unsigned binary number, shall identify a sub-channel.
     *
     * Start Address: this 10-bit field, coded as an unsigned binary number (in the range 0 to 863), shall address the first Capacity Unit (CU) of the sub-channel.
     *
     * Sub-channel size: this 10-bit field, coded as an unsigned binary number (in the range 1 to 864),
     * shall define the number of Capacity Units occupied by the sub-channel. Table 7 shows the number
     * of CUs required for all permissible data rates, in multiples of 8 kbit/s, for the four protection levels defined.
     */
    struct BasicSubchannelOrganization {
        inline bool operator==(const BasicSubchannelOrganization& other) const {
            return other.isProgrammeService == isProgrammeService && \
                   other.subChannelId == subChannelId && \
                   other.startAddress == startAddress && \
                   other.subChannelSize == subChannelSize && \
                   other.subChannelBitrate == subChannelBitrate && \
                   other.protectionLevelType == protectionLevelType && \
                   other.protectionLevel == protectionLevel && \
                   other.protectionType == protectionType && \
                   other.uepTableIndex == uepTableIndex && \
                   other.convolutionalCodingRate == convolutionalCodingRate;
        }
        inline bool operator!=(const BasicSubchannelOrganization& other) const { return !operator==(other); }

        bool isProgrammeService;
        uint8_t subChannelId;
        uint16_t startAddress;
        uint16_t subChannelSize;
        uint16_t subChannelBitrate;
        std::string convolutionalCodingRate;
        Fig_00_Ext_01::PROTECTIONLEVEL protectionLevelType;
        uint8_t protectionLevel;
        Fig_00_Ext_01::PROTECTIONTYPE protectionType;
        uint8_t uepTableIndex;
    };

public:
    explicit Fig_00_Ext_01(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_01();

    virtual const std::vector<Fig_00_Ext_01::BasicSubchannelOrganization>& getSubchannelOrganizations() const;

public:
    inline bool operator==(const Fig_00_Ext_01& other) const {
        if(m_subChanOrgas.size() != other.m_subChanOrgas.size()) {
            return false;
        }
        for(int i = 0; i < m_subChanOrgas.size(); i++) {
            if(m_subChanOrgas[i] != other.m_subChanOrgas[i]) {
                return false;
            }
        }

        return true;
    }
    inline bool operator!=(const Fig_00_Ext_01& other) const { return !operator==(other); }

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_01]"};

    std::vector<Fig_00_Ext_01::BasicSubchannelOrganization> m_subChanOrgas;

    //subchansize, protectionlvl, bitrate
    static constexpr uint16_t DAB_SUBCHANNEL_SHORTFORM_PROTECTION_TABLE[64][3] {
            /* 0*/      {16,         5,          32},
            /* 1*/      {21,         4,          32},
            /* 2*/      {24,         3,          32},
            /* 3*/      {29,         2,          32},
            /* 4*/      {35,         1,          32},
            /* 5*/      {24,         5,          48},
            /* 6*/      {29,         4,          48},
            /* 7*/      {35,         3,          48},
            /* 8*/      {42,         2,          48},
            /* 9*/      {52,         1,          48},
            /*10*/      {29,         5,          56},
            /*11*/      {35,         4,          56},
            /*12*/      {42,         3,          56},
            /*13*/      {52,         2,          56},
            /*14*/      {32,         5,          64},
            /*15*/      {42,         4,          64},
            /*16*/      {48,         3,          64},
            /*17*/      {58,         2,          64},
            /*18*/      {70,         1,          64},
            /*19*/      {40,         5,          80},
            /*20*/      {52,         4,          80},
            /*21*/      {58,         3,          80},
            /*22*/      {70,         2,          80},
            /*23*/      {84,         1,          80},
            /*24*/      {48,         5,          96},
            /*25*/      {58,         4,          96},
            /*26*/      {70,         3,          96},
            /*27*/      {84,         2,          96},
            /*28*/      {104,        1,          96},
            /*29*/      {58,         5,          112},
            /*30*/      {70,         4,          112},
            /*31*/      {84,         3,          112},
            /*32*/      {104,        2,          112},
            /*33*/      {64,         5,          128},
            /*34*/      {84,         4,          128},
            /*35*/      {96,         3,          128},
            /*36*/      {116,        2,          128},
            /*37*/      {140,        1,          128},
            /*38*/      {80,         5,          160},
            /*39*/      {104,        4,          160},
            /*40*/      {116,        3,          160},
            /*41*/      {140,        2,          160},
            /*42*/      {168,        1,          160},
            /*43*/      {96,         5,          192},
            /*44*/      {116,        4,          192},
            /*45*/      {140,        3,          192},
            /*46*/      {168,        2,          192},
            /*47*/      {208,        1,          192},
            /*48*/      {116,        5,          224},
            /*49*/      {140,        4,          224},
            /*50*/      {168,        3,          224},
            /*51*/      {208,        2,          224},
            /*52*/      {232,        1,          224},
            /*53*/      {128,        5,          256},
            /*54*/      {168,        4,          256},
            /*55*/      {192,        3,          256},
            /*56*/      {232,        2,          256},
            /*57*/      {280,        1,          256},
            /*58*/      {160,        5,          320},
            /*59*/      {208,        4,          320},
            /*60*/      {280,        2,          320},
            /*61*/      {192,        5,          384},
            /*62*/      {280,        3,          384},
            /*63*/      {416,        1,          384}
    };

private:

    const std::string OPTION_0_CONVOLUTIONAL_CODING_RATE[4] = {
        "1/4",
        "3/8",
        "1/2",
        "3/4"
    };

    const std::string OPTION_1_CONVOLUTIONAL_CODING_RATE[4] = {
        "4/9",
        "4/7",
        "4/6",
        "4/5"
    };

    const std::string CONVOLUTIONAL_CODING_RATE[8] = {
        "1/4",
        "3/8",
        "1/2",
        "3/4",
        "4/9",
        "4/7",
        "4/6",
        "4/5"
    };

    /*
     * Sub-channel size for data at different coding rates, as a function of the data rate 8 n kbit/s (where n is an integer ≥ 1)
     */
    const int OPTION_0_SUBCHAN_SIZE_CU[4] = {
        12,
         8,
         6,
         4
    };

    /*
     * Sub-channel size for data at different coding rates, as a function of the data rate 32 n kbit/s (where n is an integer ≥ 1)
     */
    const int OPTION_1_SUBCHAN_SIZE_CU[4] = {
        27,
        21,
        18,
        15
    };

    const int SUBCHANNEL_SIZE_CU[8] = {
        12,
         8,
         6,
         4,
        27,
        21,
        18,
        15
    };
};

#endif // FIG_00_EXT_01_H
