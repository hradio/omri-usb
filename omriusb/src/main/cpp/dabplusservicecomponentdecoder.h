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

#ifndef DABPLUSSERVICECOMPONENTDECODER_H
#define DABPLUSSERVICECOMPONENTDECODER_H

#include <vector>
#include <string>
#include <thread>
#include <atomic>

#include <iostream>

#include "dabservicecomponentdecoder.h"
#include "concurrent_queue.h"

#include "registered_tables.h"
#include "global_definitions.h"

class DabPlusServiceComponentDecoder : public DabServiceComponentDecoder {

public:
    DabPlusServiceComponentDecoder();
    virtual ~DabPlusServiceComponentDecoder();

    virtual void setSubchannelBitrate(uint16_t bitrate) override;
    virtual void componentDataInput(const std::vector<uint8_t>& frameData, bool synchronized) override;
    virtual void flushBufferedData() override;

    //special case for MscStreamAudio
    using PAD_DATA_CALLBACK = std::function<void (const std::vector<uint8_t>&)>;
    virtual std::shared_ptr<PAD_DATA_CALLBACK> registerPadDataCallback(PAD_DATA_CALLBACK cb);

    using AUDIO_COMPONENT_DATA_CALLBACK = std::function<void(const std::vector<uint8_t>&, int, int, int, bool, bool)>;
    virtual std::shared_ptr<AUDIO_COMPONENT_DATA_CALLBACK> registerAudioDataCallback(AUDIO_COMPONENT_DATA_CALLBACK cb);

private:
    struct DabSuperFrame {
        std::vector<uint8_t> superFrameData;
        uint8_t numAUs{0};
        std::vector<uint16_t> auLengths;
        std::vector<uint16_t> auStarts;
        bool sbrUsed{false};
        bool psUsed{false};
        int samplingRate{-1};
        int channels{-1};
        void clear() {superFrameData.clear(); numAUs = 0; auLengths.clear(); auStarts.clear(); sbrUsed = false; psUsed = false; samplingRate = -1; channels = -1;}
    };

private:
    class RSDecoder {
    private:
        void *rs_handle;
        uint8_t rs_packet[120];
        int corr_pos[10];
    public:
        RSDecoder();
        ~RSDecoder();

        void DecodeSuperframe(uint8_t *sf, size_t sf_len);
    };

    RSDecoder m_rsDec;

private:
    const std::string m_logTag = "[DabPlusServiceComponentDecoder]";

    DabSuperFrame m_currentSuperFrame;
    bool m_isSync{false};
    int m_dabSuperFrameCount{0};

    uint16_t m_superFrameSize{0};

    ConcurrentQueue <std::vector<uint8_t>> m_conQueue;
    CallbackDispatcher<std::function<void (const std::vector<uint8_t>&)>> m_padDataDispatcher;
    CallbackDispatcher<AUDIO_COMPONENT_DATA_CALLBACK> m_audioDataDispatcher;

    std::atomic<bool> m_processThreadRunning{false};
    std::thread m_processThread;

    std::vector<uint8_t> m_unsyncDataBuffer;
    bool m_unsyncSync{false};
    int m_unsyncFrameCount{0};

    inline bool CRC_CCITT_CHECK_LOCAL(const std::vector<uint8_t>& data, uint16_t dataStart, uint16_t dataLen) {
        //initial register all 1
        if(dataLen < 2) {
            return false;
        }

        uint16_t crc{0xffff};
        uint16_t crc2{0xffff};

        uint16_t crcVal{0};
        uint8_t crcCalData{0};

        try {
            auto dataIter = data.begin() + dataStart;
            while(dataIter < (data.begin() + dataStart + dataLen - 2)) {
                crcCalData = static_cast<uint8_t>(*dataIter++);
                crc = (crc << 8)^CRC_CCITT_TABLE[(crc >> 8)^(crcCalData)++];
            }

            crcVal = (*dataIter++) << 8;
            crcVal = static_cast<uint16_t>(crcVal | (*dataIter));
        } catch(std::out_of_range oor) {
            std::cout << m_logTag << " CRC iterator out of range catched..." << std::endl;
            return false;
        }

        crc2 = (crcVal^crc2);

        return crc == crc2;
    }

private:
    void processData();

    void synchronizeData(const std::vector<uint8_t>& unsyncData);

private:
    static const uint16_t FIRECODE_TABLE[256];
    static inline bool CHECK_FIRECODE(const uint8_t* frameData) {
        uint16_t firstState = (frameData[2] << 8) | frameData[3];
        uint16_t secState;

        for(int16_t i = 4; i < 11; i++) {
            secState = FIRECODE_TABLE[firstState >> 8];
            firstState = (uint16_t)(((secState & 0x00ff) ^ frameData[i]) | ((secState ^ firstState << 8) & 0xff00));
        }

        for(int16_t i = 0; i < 2; i++) {
            secState = FIRECODE_TABLE[firstState >> 8];
            firstState = (uint16_t)(((secState & 0x00ff) ^ frameData[i]) | ((secState ^ firstState << 8) & 0xff00));
        }

        return firstState == 0;
    }

};

#endif // DABPLUSSERVICECOMPONENTDECODER_H
