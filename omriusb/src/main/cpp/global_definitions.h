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

#ifndef GLOBAL_DEFINITIONS
#define GLOBAL_DEFINITIONS

//NS: 21, 26, 31
constexpr int NUM_DAB_ENSEMBLES = 41;
constexpr int DAB_FREQ_TABLE_MHZ[NUM_DAB_ENSEMBLES] = {
        174928, //05A
        176640, //05B
        178352, //05C
        180064, //05D
        181936, //06A
        183648, //06B
        185360, //06C
        187072, //06D
        188928, //07A
        190640, //07B
        192352, //07C
        194064, //07D
        195936, //08A
        197648, //08B
        199360, //08C
        201072, //08D
        202928, //09A
        204640, //09B
        206352, //09C
        208064, //09D
        209936, //10A
        //intermediate frequency
        210096, //10N
        211648, //10B
        213360, //10C
        215072, //10D
        216928, //11A
        //intermediate frequency
        217088, //11N
        218640, //11B
        220352, //11C
        222064, //11D
        223936, //12A
        //intermediate frequency
        224096, //12N
        225648, //12B
        227360, //12C
        229072, //12D
        //Channel 13
        230784, //13A
        232496, //13B
        234208, //13C
        235776, //13D
        237488, //13E
        239200  //13F
};

constexpr int NUM_DAB_N_FREQUENCIES = 3;
constexpr int DAB_N_FREQUENCIES_IDX[NUM_DAB_N_FREQUENCIES] {
    21, 26, 31
};

enum PAD_APPLICATION_TYPE {
    END_MARKER,
    DATA_GROUP_LENGTH_INDICATOR,
    DLS_DATAGROUP_START,
    DLS_DATAGROUP_CONTINUATION,
    //4-11 user defined
    MOT_DATAGROUP_START = 12,
    MOT_DATAGROUP_CONTINUATION,
    MOT_CA_MESSAGE_START,
    MOT_CA_MESSAGE_CONTINUATION,
    //16 - 31 user defined
    NOT_USED = 31
};

enum MOT_DATAGROUP_TYPE {
    MOT_DATAGROUP_GENERAL_DATA,
    MOT_CA_MESSAGE                  = 1,
    MOT_HEADER                  = 3,
    MOT_BODY_UNSCRAMBLED        = 4,
    MOT_BODY_SCRAMBLED          = 5,
    MOT_DIRECTORY_UNCOMPRESSED  = 6,
    MOT_DIRECTORY_COMPRESSED    = 7
};

enum MOT_CONTENT_TYPE {
    MOT_GENERAL_DATA,
    TEXT,
    IMAGE,
    AUDIO,
    VIDEO,
    MOT_TRANSPORT,
    SYSTEM,
    APPLICATION,
    PROPRIETARY
};

enum MOT_CONTENT_SUBTYPE_IMAGE {
    GIF,
    JPEG,
    BMP,
    PNG
};

enum MOT_CONTENT_SUBTYPE_GENERAL_DATA {
    OBJECT_TRANSFER,
    MIME_HTTP
};

enum MOT_CONTENT_SUBTYPE_MOT_TRANSPORT {
    HEADER_UPDATE,
    HEADER_ONLY
};

static const uint16_t CRC_CCITT_TABLE[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

static inline bool CRC_CCITT_CHECK(const uint8_t* data, uint16_t dataLen) {
    //initial register all 1
    if(dataLen < 2) {
        return false;
    }

    uint16_t crc = 0xffff;
    uint16_t crc2 = 0xffff;

    uint16_t crcVal, i;
    uint8_t  crcCalData;

    for (i = 0; i < (dataLen - 2); i++) {
        crcCalData = *(data+i);
        crc = (crc << 8)^CRC_CCITT_TABLE[(crc >> 8)^(crcCalData)++];
    }

    crcVal = *(data+i) << 8;
    crcVal = crcVal | *(data+i+1);

    crc2 = (crcVal^crc2);

    return crc == crc2;
}

static inline void REMOVE_PADDING(std::vector<uint8_t>& data) {
    auto delPaddingIter = data.rbegin();
    while(delPaddingIter != data.rend()) {
        if(*delPaddingIter == 0x00) {
            data.erase(--delPaddingIter.base());
            ++delPaddingIter;
            continue;
        }
        break;
    }
}

static constexpr char const * EBU_SET[16][16] {
        //           -0        -1        -2        -3        -4        -5        -6        -7        -8        -9        -A        -B        -C        -D        -E        -F
        /* 0- */ {"\u0000", "\u0118", "\u012E", "\u0172", "\u0102", "\u0116", "\u010E", "\u0218", "\u021A", "\u010A", "\u0000", "\u0000", "\u0120", "\u0139", "\u017B", "\u0143"},
        /* 1- */ {"\u0105", "\u0119", "\u012F", "\u0173", "\u0103", "\u0117", "\u010F", "\u0219", "\u021B", "\u010B", "\u0147", "\u011A", "\u0121", "\u013A", "\u017C", "\u0000"},
        /* 2- */ {"\u0020", "\u0021", "\u0022", "\u0023", "\u0142", "\u0025", "\u0026", "\u0027", "\u0028", "\u0029", "\u002A", "\u002B", "\u002C", "\u002D", "\u002E", "\u002F"},
        /* 3- */ {"\u0030", "\u0031", "\u0032", "\u0033", "\u0034", "\u0035", "\u0036", "\u0037", "\u0038", "\u0039", "\u003A", "\u003B", "\u003C", "\u003D", "\u003E", "\u003F"},
        /* 4- */ {"\u0040", "\u0041", "\u0042", "\u0043", "\u0044", "\u0045", "\u0046", "\u0047", "\u0048", "\u0049", "\u004A", "\u004B", "\u004C", "\u004D", "\u004E", "\u004F"},
        /* 5- */ {"\u0050", "\u0051", "\u0052", "\u0053", "\u0054", "\u0055", "\u0056", "\u0057", "\u0058", "\u0059", "\u005A", "\u005B", "\u016E", "\u005D", "\u0141", "\u005F"},
        /* 6- */ {"\u0104", "\u0061", "\u0062", "\u0063", "\u0064", "\u0065", "\u0066", "\u0067", "\u0068", "\u0069", "\u006A", "\u006B", "\u006C", "\u006D", "\u006E", "\u006F"},
        /* 7- */ {"\u0070", "\u0071", "\u0072", "\u0073", "\u0074", "\u0075", "\u0076", "\u0077", "\u0078", "\u0079", "\u007A", "\u00AB", "\u016F", "\u00BB", "\u013D", "\u0126"},
        /* 8- */ {"\u00E1", "\u00E0", "\u00E9", "\u00E8", "\u00ED", "\u00EC", "\u00F3", "\u00F2", "\u00FA", "\u00F9", "\u00D1", "\u00C7", "\u015E", "\u00DF", "\u00A1", "\u0178"},
        /* 9- */ {"\u00E2", "\u00E4", "\u00EA", "\u00EB", "\u00EE", "\u00EF", "\u00F4", "\u00F6", "\u00FB", "\u00FC", "\u00F1", "\u00E7", "\u015F", "\u011F", "\u0131", "\u00FF"},
        /* A- */ {"\u0136", "\u0145", "\u00A9", "\u0122", "\u011E", "\u011B", "\u0148", "\u0151", "\u0150", "\u20AC", "\u00A3", "\u0024", "\u0100", "\u0112", "\u012A", "\u016A"},
        /* B- */ {"\u0137", "\u0146", "\u013B", "\u0123", "\u013C", "\u0130", "\u0144", "\u0171", "\u0170", "\u00BF", "\u013E", "\u00B0", "\u0101", "\u0113", "\u012B", "\u016B"},
        /* C- */ {"\u00C1", "\u00C0", "\u00C9", "\u00C8", "\u00CD", "\u00CC", "\u00D3", "\u00D2", "\u00DA", "\u00D9", "\u0158", "\u010C", "\u0160", "\u017D", "\u00D0", "\u013F"},
        /* D- */ {"\u00C2", "\u00C4", "\u00CA", "\u00CB", "\u00CE", "\u00CF", "\u00D4", "\u00D6", "\u00DB", "\u00DC", "\u0159", "\u010D", "\u0161", "\u017E", "\u0111", "\u0140"},
        /* E- */ {"\u00C3", "\u00C5", "\u00C6", "\u0152", "\u0177", "\u00DD", "\u00D5", "\u00D8", "\u00DE", "\u014A", "\u0154", "\u0106", "\u015A", "\u0179", "\u0164", "\u00F0"},
        /* F- */ {"\u00E3", "\u00E5", "\u00E6", "\u0153", "\u0175", "\u00FD", "\u00F5", "\u00F8", "\u00FE", "\u014B", "\u0155", "\u0107", "\u015B", "\u017A", "\u0165", "\u0127"},
};

static inline std::string convertEbuToUtf(const std::string& ebuString) {
    std::string utfString;
    for(const auto& temp : ebuString) {
        utfString.append(EBU_SET[(temp >> 4) & 0x0F][temp & 0x0F]);
    }

    return utfString;
}

#endif // GLOBAL_DEFINITIONS

