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

#ifndef FICPARSER_H
#define FICPARSER_H

#include <atomic>
#include <list>

#include "callback.h"
#include "concurrent_queue.h"

#include "fig_00_ext_00.h"
#include "fig_00_ext_01.h"
#include "fig_00_ext_02.h"
#include "fig_00_ext_03.h"
#include "fig_00_ext_04.h"
#include "fig_00_ext_05.h"
#include "fig_00_ext_06.h"
#include "fig_00_ext_07.h"
#include "fig_00_ext_08.h"
#include "fig_00_ext_09.h"
#include "fig_00_ext_10.h"
#include "fig_00_ext_13.h"
#include "fig_00_ext_14.h"
#include "fig_00_ext_17.h"
#include "fig_00_ext_18.h"
#include "fig_00_ext_19.h"
#include "fig_00_ext_20.h"
#include "fig_00_ext_21.h"
#include "fig_00_ext_24.h"
#include "fig_00_ext_25.h"
#include "fig_00_ext_26.h"

#include "fig_01_ext_00.h"
#include "fig_01_ext_01.h"
#include "fig_01_ext_04.h"
#include "fig_01_ext_05.h"
#include "fig_01_ext_06.h"

#include "callbackhandle.h"

class DabEnsemble;
class FicParser : public Callback {

public:
    using Fig_00_00_Callback = std::function<void(const Fig_00_Ext_00&)>;
    using Fig_00_01_Callback = std::function<void(const Fig_00_Ext_01&)>;
    using Fig_00_02_Callback = std::function<void(const Fig_00_Ext_02&)>;
    using Fig_00_03_Callback = std::function<void(const Fig_00_Ext_03&)>;
    using Fig_00_04_Callback = std::function<void(const Fig_00_Ext_04&)>;
    using Fig_00_05_Callback = std::function<void(const Fig_00_Ext_05&)>;
    using Fig_00_06_Callback = std::function<void(const Fig_00_Ext_06&)>;
    using Fig_00_07_Callback = std::function<void(const Fig_00_Ext_07&)>;
    using Fig_00_08_Callback = std::function<void(const Fig_00_Ext_08&)>;
    using Fig_00_09_Callback = std::function<void(const Fig_00_Ext_09&)>;
    using Fig_00_10_Callback = std::function<void(const Fig_00_Ext_10&)>;

    using Fig_00_13_Callback = std::function<void(const Fig_00_Ext_13&)>;
    using Fig_00_14_Callback = std::function<void(const Fig_00_Ext_14&)>;
    using Fig_00_17_Callback = std::function<void(const Fig_00_Ext_17&)>;
    using Fig_00_18_Callback = std::function<void(const Fig_00_Ext_18&)>;
    using Fig_00_19_Callback = std::function<void(const Fig_00_Ext_19&)>;
    using Fig_00_20_Callback = std::function<void(const Fig_00_Ext_20&)>;
    using Fig_00_21_Callback = std::function<void(const Fig_00_Ext_21&)>;
    using Fig_00_24_Callback = std::function<void(const Fig_00_Ext_24&)>;
    using Fig_00_25_Callback = std::function<void(const Fig_00_Ext_25&)>;
    using Fig_00_26_Callback = std::function<void(const Fig_00_Ext_26&)>;

    using Fig_01_00_Callback = std::function<void(const Fig_01_Ext_00&)>;
    using Fig_01_01_Callback = std::function<void(const Fig_01_Ext_01&)>;
    using Fig_01_04_Callback = std::function<void(const Fig_01_Ext_04&)>;
    using Fig_01_05_Callback = std::function<void(const Fig_01_Ext_05&)>;
    using Fig_01_06_Callback = std::function<void(const Fig_01_Ext_06&)>;

    using Fig_00_Done_Callback = std::function<void(Fig::FIG_00_TYPE)>;
    using Fig_01_Done_Callback = std::function<void(Fig::FIG_01_TYPE)>;

public:
    explicit FicParser();
    virtual ~FicParser();

    virtual void call(const std::vector<uint8_t>& data) override;

    std::shared_ptr<Fig_00_Done_Callback> registerFig_00_Done_Callback(Fig_00_Done_Callback cb);
    std::shared_ptr<Fig_01_Done_Callback> registerFig_01_Done_Callback(Fig_01_Done_Callback cb);
    std::shared_ptr<Fig_00_00_Callback> registerFig_00_00_Callback(Fig_00_00_Callback cb);
    std::shared_ptr<Fig_00_01_Callback> registerFig_00_01_Callback(Fig_00_01_Callback cb);
    std::shared_ptr<Fig_00_02_Callback> registerFig_00_02_Callback(Fig_00_02_Callback cb);
    std::shared_ptr<Fig_00_03_Callback> registerFig_00_03_Callback(Fig_00_03_Callback cb);
    std::shared_ptr<Fig_00_04_Callback> registerFig_00_04_Callback(Fig_00_04_Callback cb);
    std::shared_ptr<Fig_00_05_Callback> registerFig_00_05_Callback(Fig_00_05_Callback cb);
    std::shared_ptr<Fig_00_06_Callback> registerFig_00_06_Callback(Fig_00_06_Callback cb);
    std::shared_ptr<Fig_00_07_Callback> registerFig_00_07_Callback(Fig_00_07_Callback cb);
    std::shared_ptr<Fig_00_08_Callback> registerFig_00_08_Callback(Fig_00_08_Callback cb);
    std::shared_ptr<Fig_00_09_Callback> registerFig_00_09_Callback(Fig_00_09_Callback cb);
    std::shared_ptr<Fig_00_10_Callback> registerFig_00_10_Callback(Fig_00_10_Callback cb);
    std::shared_ptr<Fig_00_13_Callback> registerFig_00_13_Callback(Fig_00_13_Callback cb);
    std::shared_ptr<Fig_00_14_Callback> registerFig_00_14_Callback(Fig_00_14_Callback cb);
    std::shared_ptr<Fig_00_17_Callback> registerFig_00_17_Callback(Fig_00_17_Callback cb);
    std::shared_ptr<Fig_00_18_Callback> registerFig_00_18_Callback(Fig_00_18_Callback cb);
    std::shared_ptr<Fig_00_19_Callback> registerFig_00_19_Callback(Fig_00_19_Callback cb);
    std::shared_ptr<Fig_00_20_Callback> registerFig_00_20_Callback(Fig_00_20_Callback cb);
    std::shared_ptr<Fig_00_21_Callback> registerFig_00_21_Callback(Fig_00_21_Callback cb);
    std::shared_ptr<Fig_00_24_Callback> registerFig_00_24_Callback(Fig_00_24_Callback cb);
    std::shared_ptr<Fig_00_25_Callback> registerFig_00_25_Callback(Fig_00_25_Callback cb);
    std::shared_ptr<Fig_00_26_Callback> registerFig_00_26_Callback(Fig_00_26_Callback cb);

    std::shared_ptr<Fig_01_00_Callback> registerFig_01_00_Callback(Fig_01_00_Callback cb);
    std::shared_ptr<Fig_01_01_Callback> registerFig_01_01_Callback(Fig_01_01_Callback cb);
    std::shared_ptr<Fig_01_04_Callback> registerFig_01_04_Callback(Fig_01_04_Callback cb);
    std::shared_ptr<Fig_01_05_Callback> registerFig_01_05_Callback(Fig_01_05_Callback cb);
    std::shared_ptr<Fig_01_06_Callback> registerFig_01_06_Callback(Fig_01_06_Callback cb);

    void reset();

private:
    const std::string M_LOG_TAG = "[FicParser] ";

    CallbackDispatcher<Fig_00_00_Callback> m_fig00_00dispatcher;
    CallbackDispatcher<Fig_00_01_Callback> m_fig00_01dispatcher;
    CallbackDispatcher<Fig_00_02_Callback> m_fig00_02dispatcher;
    CallbackDispatcher<Fig_00_03_Callback> m_fig00_03dispatcher;
    CallbackDispatcher<Fig_00_04_Callback> m_fig00_04dispatcher;
    CallbackDispatcher<Fig_00_05_Callback> m_fig00_05dispatcher;
    CallbackDispatcher<Fig_00_06_Callback> m_fig00_06dispatcher;
    CallbackDispatcher<Fig_00_07_Callback> m_fig00_07dispatcher;
    CallbackDispatcher<Fig_00_08_Callback> m_fig00_08dispatcher;
    CallbackDispatcher<Fig_00_09_Callback> m_fig00_09dispatcher;
    CallbackDispatcher<Fig_00_10_Callback> m_fig00_10dispatcher;
    CallbackDispatcher<Fig_00_13_Callback> m_fig00_13dispatcher;
    CallbackDispatcher<Fig_00_14_Callback> m_fig00_14dispatcher;
    CallbackDispatcher<Fig_00_17_Callback> m_fig00_17dispatcher;
    CallbackDispatcher<Fig_00_18_Callback> m_fig00_18dispatcher;
    CallbackDispatcher<Fig_00_19_Callback> m_fig00_19dispatcher;
    CallbackDispatcher<Fig_00_20_Callback> m_fig00_20dispatcher;
    CallbackDispatcher<Fig_00_21_Callback> m_fig00_21dispatcher;
    CallbackDispatcher<Fig_00_24_Callback> m_fig00_24dispatcher;
    CallbackDispatcher<Fig_00_25_Callback> m_fig00_25dispatcher;
    CallbackDispatcher<Fig_00_26_Callback> m_fig00_26dispatcher;

    CallbackDispatcher<Fig_01_00_Callback> m_fig01_00dispatcher;
    CallbackDispatcher<Fig_01_01_Callback> m_fig01_01dispatcher;
    CallbackDispatcher<Fig_01_04_Callback> m_fig01_04dispatcher;
    CallbackDispatcher<Fig_01_05_Callback> m_fig01_05dispatcher;
    CallbackDispatcher<Fig_01_06_Callback> m_fig01_06dispatcher;

    CallbackDispatcher<Fig_00_Done_Callback> m_fig00DoneDispatcher;
    CallbackDispatcher<Fig_01_Done_Callback> m_fig01DoneDispatcher;

    ConcurrentQueue<std::vector<uint8_t> > m_fibDataQueue;
    std::atomic<bool> m_fibProcessThreadRunning;
    std::thread m_fibProcessorThread;

    std::vector<std::vector<uint8_t> > ficBuffer;

private:
    void processFib();
    void parseFig_00(const std::vector<uint8_t>& ficData);
    void parseFig_01(const std::vector<uint8_t>& ficData);

private:
    static constexpr uint16_t CRC_CCITT_TABLE[] = {
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

    static inline bool FIB_CRC_CHECK(const uint8_t* data) {

        //fixed fib size
        uint16_t dataLen = 32;

        //initial register
        uint16_t crc = 0xffff;
        uint16_t crc2 = 0xffff;

        uint16_t crcVal, i;
        uint8_t  crcCalData;

        for (i = 0; i < (dataLen - 2); i++) {
            crcCalData = *(data+i);
            crc = (crc << 8)^FicParser::CRC_CCITT_TABLE[(crc >> 8)^(crcCalData)++];
        }

        crcVal = *(data+i) << 8;
        crcVal = crcVal | *(data+i+1);

        crc2 = (crcVal^crc2);

        if(crc == crc2) {
            return true;
        }

        return false;
    }

private:
    std::vector<Fig_00_Ext_01> m_parsedFig0001;
    std::vector<Fig_00_Ext_02> m_parsedFig0002;
    std::vector<Fig_00_Ext_03> m_parsedFig0003;
    std::vector<Fig_00_Ext_08> m_parsedFig0008;
    std::vector<Fig_00_Ext_13> m_parsedFig0013;
    std::vector<Fig_00_Ext_14> m_parsedFig0014;
    std::vector<Fig_00_Ext_17> m_parsedFig0017;

    std::vector<Fig_01_Ext_00> m_parsedFig0100;
    std::vector<Fig_01_Ext_01> m_parsedFig0101;
    std::vector<Fig_01_Ext_04> m_parsedFig0104;
    std::vector<Fig_01_Ext_05> m_parsedFig0105;
    std::vector<Fig_01_Ext_06> m_parsedFig0106;

    bool m_1wasDone{false};
    bool m_2wasDone{false};
    bool m_3wasDone{false};
    bool m_8wasDone{false};

private:
    template<class T>
    bool contains(std::vector<T> a, T b) {
        for(const auto& fig : a) {
            if(fig == b) {
                return true;
            }
        }
        return false;
    }
};

#endif // FICPARSER_H
