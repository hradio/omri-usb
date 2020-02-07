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

#ifndef DABMPEGSERVICECOMPONENTDECODER_H
#define DABMPEGSERVICECOMPONENTDECODER_H

#include <thread>
#include <atomic>

#include "dabservicecomponentdecoder.h"

#include "concurrent_queue.h"

class DabMpegServiceComponentDecoder : public DabServiceComponentDecoder {

public:
    DabMpegServiceComponentDecoder();
    virtual ~DabMpegServiceComponentDecoder();

    virtual void componentDataInput(const std::vector<uint8_t>& frameData, bool synchronized) override;
    virtual void flushBufferedData() override;

    using PAD_DATA_CALLBACK = std::function<void (const std::vector<uint8_t>&)>;
    virtual std::shared_ptr<PAD_DATA_CALLBACK> registerPadDataCallback(PAD_DATA_CALLBACK cb);

    using AUDIO_COMPONENT_DATA_CALLBACK = std::function<void(const std::vector<uint8_t>&, int, int, int, bool, bool)>;
    virtual std::shared_ptr<AUDIO_COMPONENT_DATA_CALLBACK> registerAudioDataCallback(AUDIO_COMPONENT_DATA_CALLBACK cb);

private:
    const std::string m_logTag{"[DabMpegServiceComponentDecoder] "};

    ConcurrentQueue <std::vector<uint8_t>> m_conQueue;
    std::atomic<bool> m_processThreadRunning{false};
    std::thread m_processThread;

    CallbackDispatcher<std::function<void (const std::vector<uint8_t>&)>> m_padDataDispatcher;
    CallbackDispatcher<AUDIO_COMPONENT_DATA_CALLBACK> m_audioDataDispatcher;

    bool m_frameSizeAdjusted{false};

private:
    void processData();
    void synchronizeData(const std::vector<uint8_t>& unsyncData);

private:
    uint8_t m_noCiLastLength{0};

    std::vector<uint8_t> m_unsyncDataBuffer;

    static constexpr uint8_t XPAD_SIZE[8] {
        4, 6, 8, 12, 16, 24, 32, 48
    };

    static constexpr uint8_t  M1L2_BITRATE_IDX_MAX{0x0E};
    static constexpr uint16_t M1L2_BITRATE_IDX[] {
        0,
        32,
        48,
        56,
        64,
        80,
        96,
        112,
        128,
        160,
        192,
        224,
        256,
        320,
        384
    };

    static constexpr uint8_t  M1_SAMPLINGFREQUENCY_IDX_MAX{0x02};
    static constexpr uint16_t M1_SAMPLINGFREQUENCY_IDX[] {
        44100,
        48000,
        32000
    };

    static constexpr uint8_t CHANNELMODE_IDX_MAX{0x03};
    static constexpr uint8_t CHANNELMODE_IDX[] {
        2,
        2,
        2,
        1
    };
};

#endif // DABMPEGSERVICECOMPONENTDECODER_H
