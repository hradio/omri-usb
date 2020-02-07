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

#include "dabplusservicecomponentdecoder.h"

extern "C" {
    #include "thirdparty/fec/fec.h"
}

#include <iostream>
#include <iomanip>

DabPlusServiceComponentDecoder::DabPlusServiceComponentDecoder() {
    m_processThreadRunning = true;
    m_processThread = std::thread(&DabPlusServiceComponentDecoder::processData, this);
}

DabPlusServiceComponentDecoder::~DabPlusServiceComponentDecoder() {
    std::cout << m_logTag << " Deconstructing" << std::endl;

    m_processThreadRunning = false;
    if(m_processThread.joinable()) {
        m_processThread.join();
    }
    //m_conQueue.clear();
}

void DabPlusServiceComponentDecoder::setSubchannelBitrate(uint16_t bitrate) {
    DabServiceComponentDecoder::setSubchannelBitrate(bitrate);

    //Superframe size = bitrate/8*110
    //subchannel_index = MSC sub-channel size (kbps) ÷ 8
    //audio_super_frame_size (bytes) = subchannel_index × 110
    m_superFrameSize = static_cast<uint16_t>((m_subChanBitrate / 8) * 110);

    std::cout << m_logTag << " SuperFrameSize: " << +m_superFrameSize << " SubchanBitrate: " << +m_subChanBitrate << std::endl;
}

void DabPlusServiceComponentDecoder::componentDataInput(const std::vector<uint8_t> &frameData, bool synchronized) {
    //std::cout << m_logTag << " DataInput Size: " << +frameData.size() << std::endl;
    if(!m_processThreadRunning) {
        return;
    }

    if(synchronized) {
        m_conQueue.push(frameData);
    } else {
        if(m_frameSize > 0) {
            synchronizeData(frameData);
        }
    }
}

void DabPlusServiceComponentDecoder::flushBufferedData() {
    m_conQueue.clear();
}

void DabPlusServiceComponentDecoder::synchronizeData(const std::vector<uint8_t>& unsyncData) {
    //TODO poor and inefficient
    std::vector<uint8_t> data;
    if(!m_unsyncDataBuffer.empty()) {
        data.insert(data.begin(), m_unsyncDataBuffer.begin(), m_unsyncDataBuffer.end());
        m_unsyncDataBuffer.clear();
    }

    data.insert(data.end(), unsyncData.begin(), unsyncData.end());

    auto unsyncIter = data.begin();
    while(unsyncIter+m_frameSize < data.end()) {
        if (m_unsyncSync) {
            while (unsyncIter + m_frameSize < data.end()) {
                m_conQueue.push(std::vector<uint8_t>(unsyncIter, unsyncIter + m_frameSize));
                unsyncIter += m_frameSize;
                ++m_unsyncFrameCount;
                if (m_unsyncFrameCount == 5) {
                    m_unsyncSync = false;
                    m_unsyncFrameCount = 0;
                    break;
                }
            }
            continue;
        }

        std::vector<uint8_t> checkData(unsyncIter, unsyncIter + m_frameSize);

        if(!(checkData[0] == 0x00 && checkData[1] == 0x00)) {
            if (CHECK_FIRECODE(checkData.data())) {
                //std::cout << m_logTag << " Found FireCode: " << +checkData.size() << std::hex << +checkData[0] << " : " << +checkData[1] << std::dec << std::endl;
                m_conQueue.push(checkData);
                ++m_unsyncFrameCount;
                m_unsyncSync = true;
                unsyncIter += m_frameSize;
                continue;
            }
        } else {
            std::cout << m_logTag << " FireCode: 0x" << std::hex << +checkData[0] << " 0x" << +checkData[1] << " 0x" << +checkData[2] << " 0x" << +checkData[3] << " 0x" << +checkData[4] << std::dec << std::endl;
        }

        unsyncIter++;
    }

    m_unsyncDataBuffer.insert(m_unsyncDataBuffer.begin(), unsyncIter, data.end());
}

std::shared_ptr<DabPlusServiceComponentDecoder::PAD_DATA_CALLBACK> DabPlusServiceComponentDecoder::registerPadDataCallback(DabPlusServiceComponentDecoder::PAD_DATA_CALLBACK cb) {
    return m_padDataDispatcher.add(cb);
}

std::shared_ptr<DabPlusServiceComponentDecoder::AUDIO_COMPONENT_DATA_CALLBACK> DabPlusServiceComponentDecoder::registerAudioDataCallback(DabPlusServiceComponentDecoder::AUDIO_COMPONENT_DATA_CALLBACK cb) {
    return m_audioDataDispatcher.add(cb);
}

void DabPlusServiceComponentDecoder::processData() {
    while(m_processThreadRunning) {
        std::vector<uint8_t> frameData;
        //For assembling all access units per superframe for always having 120 ms audio for feeding into decoder
        std::vector<uint8_t> auBuff;

        if(m_superFrameSize == 0) {
            continue;
        }

        if(m_conQueue.tryPop(frameData, std::chrono::milliseconds(50))) {
            if(m_dabSuperFrameCount == 0) {

                if (CHECK_FIRECODE(frameData.data())) {
                    m_currentSuperFrame.clear();

                    //AAC CoreSamplingRate
                    //16 kHz AAC core sampling rate with SBR enabled
                    //24 kHz AAC core sampling rate with SBR enabled
                    //32 kHz AAC core sampling rate
                    //48 kHz AAC core sampling rate

                    //2 byte firecode preceeding
                    //bool rfa = (frameData[2] >> 7) & 0xFF;
                    bool dacRate = static_cast<bool>((frameData[2] & 0x40u) >> 6u);      // dacRate ? samplingRate(48000) : samplingRate(32000)
                    bool sbr = static_cast<bool>((frameData[2] & 0x20u) >> 5u);          // sbr ? sbrUsed : sbrNotUsed
                    bool chanMode = static_cast<bool>((frameData[2] & 0x10u) >> 4u);     // chanMode ? aacStereo : aacMono
                    bool ps = static_cast<bool>((frameData[2] & 0x08u) >> 3u);           // ps ? psUsed : psNotUsed
                    auto mpgSurCfg = static_cast<uint8_t >(frameData[2] & 0x07u);

                    m_currentSuperFrame.sbrUsed = sbr;
                    m_currentSuperFrame.psUsed = ps;
                    m_currentSuperFrame.channels = chanMode ? 2 : 1;
                    m_currentSuperFrame.samplingRate = dacRate ? 48000 : 32000;

                    if(!dacRate && sbr) m_currentSuperFrame.numAUs = 2;
                    if(dacRate && sbr) m_currentSuperFrame.numAUs = 3;
                    if(!dacRate && !sbr) m_currentSuperFrame.numAUs = 4;
                    if(dacRate && !sbr) m_currentSuperFrame.numAUs = 6;

                    //std::cout << m_logTag << " NumAUs: " << +m_currentSuperFrame.numAUs << " DAC: " << +dacRate << " : " << +m_currentSuperFrame.samplingRate << " SBR: " << +sbr << " CHAN: " << +chanMode << " : " << +m_currentSuperFrame.channels << " PS: " << +ps << " MPEG: " << +mpgSurCfg << std::endl;

                    bool alignmentPresent = (!(dacRate && sbr));

                    //First AU start position
                    switch (m_currentSuperFrame.numAUs) {
                        case 2: {
                            //AAC CoreSamplingRate
                            //16 kHz AAC core sampling rate with SBR enabled
                            //AU contains audio samples for 60 ms
                            m_currentSuperFrame.auStarts.push_back(5);
                            break;
                        }
                        case 3: {
                            //AAC CoreSamplingRate
                            //24 kHz AAC core sampling rate with SBR enabled
                            //AU contains audio samples for 40 ms
                            m_currentSuperFrame.auStarts.push_back(6);
                            break;
                        }
                        case 4: {
                            //AAC CoreSamplingRate
                            //32 kHz AAC core sampling rate
                            //AU contains audio samples for 30 ms
                            m_currentSuperFrame.auStarts.push_back(8);
                            break;
                        }
                        case 6: {
                            //AAC CoreSamplingRate
                            //48 kHz AAC core sampling rate
                            //AU contains audio samples for 20 ms
                            m_currentSuperFrame.auStarts.push_back(11);
                            break;
                        }
                        default:
                            //Though shall not pass
                            std::cout << m_logTag << " Bad NumAUs: " << +m_currentSuperFrame.numAUs << std::endl;
                            break;
                    }

                    //other AU start positions
                    auto frameIter = frameData.cbegin() + 3;
                    //start at index 1. first Au start is already known
                    bool badAuStart{false};
                    for (uint8_t i = 1; i < m_currentSuperFrame.numAUs; i++) {
                        if(!m_processThreadRunning) {
                            std::cout << m_logTag << " Breaking for thread stop at getting AU lengths" << std::endl;
                            return;
                        }

                        uint16_t auStart{0xFFFF};
                        if (i % 2) {
                            auStart = static_cast<uint16_t>((*frameIter++ & 0xFFu) << 4u | (*frameIter & 0xF0u) >> 4u);
                        } else {
                            auStart = static_cast<uint16_t>((*frameIter++ & 0x0Fu) << 8u | (*frameIter++ & 0xFFu));
                        }

                        //AU start sanity checks
                        if(auStart > m_superFrameSize || auStart <= m_currentSuperFrame.auStarts[i-1]) {
                            std::cout << m_logTag << " Bad AU-Start NumAUs " << +m_currentSuperFrame.numAUs << " : " << +i << std::endl;
                            std::cout << m_logTag << " Bad AU-Start: " << +auStart << " : " << +m_superFrameSize << std::endl;
                            std::cout << m_logTag << " Bad AU-Start: " << +auStart << " : " << +m_currentSuperFrame.auStarts[i-1] << std::endl;
                            badAuStart = true;
                            break;
                        }

                        m_currentSuperFrame.auStarts.push_back(auStart);
                    }

                    if(badAuStart) {
                        //std::cout << m_logTag << " Bad AU-Start" << std::endl;
                        continue;
                    }

                    //last auNum+1
                    m_currentSuperFrame.auStarts.push_back(m_superFrameSize);

                    for (int i = 0; i < m_currentSuperFrame.numAUs; i++) {
                        m_currentSuperFrame.auLengths.push_back(m_currentSuperFrame.auStarts[i + 1] - m_currentSuperFrame.auStarts[i]);
                    }

                    m_currentSuperFrame.superFrameData.insert(m_currentSuperFrame.superFrameData.end(), frameData.begin(), frameData.end());

                    ++m_dabSuperFrameCount;

                    m_isSync = true;
                    continue;
                }
            }

            if(m_dabSuperFrameCount > 5) {
                std::cout << m_logTag << " SuperFrame damaged #########################" << std::endl;
                m_isSync = false;
                m_dabSuperFrameCount = 0;
            }

            if(m_isSync) {
                m_currentSuperFrame.superFrameData.insert(m_currentSuperFrame.superFrameData.end(), frameData.begin(), frameData.end());
                ++m_dabSuperFrameCount;

                if(m_dabSuperFrameCount == 5) {

                    m_rsDec.DecodeSuperframe(m_currentSuperFrame.superFrameData.data(), m_currentSuperFrame.superFrameData.size());

                    if(m_currentSuperFrame.superFrameData.size() >= m_superFrameSize) {
                        auBuff.clear();
                        for(int i = 0; i < m_currentSuperFrame.numAUs; i++) {

                            if(!m_processThreadRunning) {
                                return;
                            }

                            if(CRC_CCITT_CHECK(m_currentSuperFrame.superFrameData.data()+m_currentSuperFrame.auStarts[i], m_currentSuperFrame.auLengths[i])) {
                                if(((static_cast<unsigned>(m_currentSuperFrame.superFrameData[m_currentSuperFrame.auStarts[i]]) >> 5u) & 0x07u) == 0x04u) {
                                    uint8_t padDataStart = 2;
                                    uint8_t padDataLen = m_currentSuperFrame.superFrameData[m_currentSuperFrame.auStarts[i] + 1];
                                    if(padDataLen == 0xFF) {
                                        padDataLen += m_currentSuperFrame.superFrameData[m_currentSuperFrame.auStarts[i] + 2];
                                        ++padDataStart;
                                    }

                                    std::vector<uint8_t> padData(m_currentSuperFrame.superFrameData.begin()+m_currentSuperFrame.auStarts[i]+padDataStart, m_currentSuperFrame.superFrameData.begin()+m_currentSuperFrame.auStarts[i]+padDataStart+padDataLen);

                                    m_padDataDispatcher.invoke(padData);

                                    //change the beginnings and lengths of the AU
                                    m_currentSuperFrame.auStarts[i] += (padDataStart + padDataLen);
                                    m_currentSuperFrame.auLengths[i] -= (padDataStart + padDataLen);
                                }

                                auBuff.insert(auBuff.end(), m_currentSuperFrame.superFrameData.begin()+m_currentSuperFrame.auStarts[i], m_currentSuperFrame.superFrameData.begin() + m_currentSuperFrame.auStarts[i] + m_currentSuperFrame.auLengths[i] - 2); // -2 of length to cut off CRC
                            } else {
                                std::cout << m_logTag << " SuperFrame AU[" << +i << "] CRC failed, Bitrate: " << +m_subChanBitrate << std::endl;
                            }
                        }

                        m_audioDataDispatcher.invoke(auBuff, 63, m_currentSuperFrame.channels, m_currentSuperFrame.samplingRate, m_currentSuperFrame.sbrUsed, m_currentSuperFrame.psUsed);
                    }

                    m_dabSuperFrameCount = 0;
                    m_isSync = false;
                }
            }
        }
    }

    std::cout << m_logTag << " ProcessData thread stopped" << std::endl;
}

const uint16_t DabPlusServiceComponentDecoder::FIRECODE_TABLE[256] = {
    0x0000, 0x782f, 0xf05e, 0x8871, 0x9893, 0xe0bc, 0x68cd, 0x10e2,
    0x4909, 0x3126, 0xb957, 0xc178, 0xd19a, 0xa9b5, 0x21c4, 0x59eb,
    0x9212, 0xea3d, 0x624c, 0x1a63, 0x0a81, 0x72ae, 0xfadf, 0x82f0,
    0xdb1b, 0xa334, 0x2b45, 0x536a, 0x4388, 0x3ba7, 0xb3d6, 0xcbf9,
    0x5c0b, 0x2424, 0xac55, 0xd47a, 0xc498, 0xbcb7, 0x34c6, 0x4ce9,
    0x1502, 0x6d2d, 0xe55c, 0x9d73, 0x8d91, 0xf5be, 0x7dcf, 0x05e0,
    0xce19, 0xb636, 0x3e47, 0x4668, 0x568a, 0x2ea5, 0xa6d4, 0xdefb,
    0x8710, 0xff3f, 0x774e, 0x0f61, 0x1f83, 0x67ac, 0xefdd, 0x97f2,
    0xb816, 0xc039, 0x4848, 0x3067, 0x2085, 0x58aa, 0xd0db, 0xa8f4,
    0xf11f, 0x8930, 0x0141, 0x796e, 0x698c, 0x11a3, 0x99d2, 0xe1fd,
    0x2a04, 0x522b, 0xda5a, 0xa275, 0xb297, 0xcab8, 0x42c9, 0x3ae6,
    0x630d, 0x1b22, 0x9353, 0xeb7c, 0xfb9e, 0x83b1, 0x0bc0, 0x73ef,
    0xe41d, 0x9c32, 0x1443, 0x6c6c, 0x7c8e, 0x04a1, 0x8cd0, 0xf4ff,
    0xad14, 0xd53b, 0x5d4a, 0x2565, 0x3587, 0x4da8, 0xc5d9, 0xbdf6,
    0x760f, 0x0e20, 0x8651, 0xfe7e, 0xee9c, 0x96b3, 0x1ec2, 0x66ed,
    0x3f06, 0x4729, 0xcf58, 0xb777, 0xa795, 0xdfba, 0x57cb, 0x2fe4,
    0x0803, 0x702c, 0xf85d, 0x8072, 0x9090, 0xe8bf, 0x60ce, 0x18e1,
    0x410a, 0x3925, 0xb154, 0xc97b, 0xd999, 0xa1b6, 0x29c7, 0x51e8,
    0x9a11, 0xe23e, 0x6a4f, 0x1260, 0x0282, 0x7aad, 0xf2dc, 0x8af3,
    0xd318, 0xab37, 0x2346, 0x5b69, 0x4b8b, 0x33a4, 0xbbd5, 0xc3fa,
    0x5408, 0x2c27, 0xa456, 0xdc79, 0xcc9b, 0xb4b4, 0x3cc5, 0x44ea,
    0x1d01, 0x652e, 0xed5f, 0x9570, 0x8592, 0xfdbd, 0x75cc, 0x0de3,
    0xc61a, 0xbe35, 0x3644, 0x4e6b, 0x5e89, 0x26a6, 0xaed7, 0xd6f8,
    0x8f13, 0xf73c, 0x7f4d, 0x0762, 0x1780, 0x6faf, 0xe7de, 0x9ff1,
    0xb015, 0xc83a, 0x404b, 0x3864, 0x2886, 0x50a9, 0xd8d8, 0xa0f7,
    0xf91c, 0x8133, 0x0942, 0x716d, 0x618f, 0x19a0, 0x91d1, 0xe9fe,
    0x2207, 0x5a28, 0xd259, 0xaa76, 0xba94, 0xc2bb, 0x4aca, 0x32e5,
    0x6b0e, 0x1321, 0x9b50, 0xe37f, 0xf39d, 0x8bb2, 0x03c3, 0x7bec,
    0xec1e, 0x9431, 0x1c40, 0x646f, 0x748d, 0x0ca2, 0x84d3, 0xfcfc,
    0xa517, 0xdd38, 0x5549, 0x2d66, 0x3d84, 0x45ab, 0xcdda, 0xb5f5,
    0x7e0c, 0x0623, 0x8e52, 0xf67d, 0xe69f, 0x9eb0, 0x16c1, 0x6eee,
    0x3705, 0x4f2a, 0xc75b, 0xbf74, 0xaf96, 0xd7b9, 0x5fc8, 0x27e7
};

// --- RSDecoder -----------------------------------------------------------------
DabPlusServiceComponentDecoder::RSDecoder::RSDecoder() {
    rs_handle = init_rs_char(8, 0x11D, 0, 1, 10, 135);
    if(!rs_handle)
        throw std::runtime_error("RSDecoder: error while init_rs_char");
}

DabPlusServiceComponentDecoder::RSDecoder::~RSDecoder() {
    free_rs_char(rs_handle);
}

void DabPlusServiceComponentDecoder::RSDecoder::DecodeSuperframe(uint8_t* sf, size_t sf_len) {
	// insert errors for test
	//sf[0] ^= 0xFF;
	//sf[10] ^= 0xFF;
	//sf[20] ^= 0xFF;

    int subch_index = sf_len / 120;
    int total_corr_count = 0;
    bool uncorr_errors = false;

    // process all RS packets
    for(int i = 0; i < subch_index; i++) {
        for(int pos = 0; pos < 120; pos++)
            rs_packet[pos] = sf[pos * subch_index + i];

        // detect errors
        int corr_count = decode_rs_char(rs_handle, rs_packet, corr_pos, 0);
        if(corr_count == -1)
            uncorr_errors = true;
        else
            total_corr_count += corr_count;

        // correct errors
        for(int j = 0; j < corr_count; j++) {

            int pos = corr_pos[j] - 135;
            if(pos < 0)
                continue;

            sf[pos * subch_index + i] = rs_packet[pos];
        }
    }

    if(total_corr_count || uncorr_errors) {
        std::cout << "[RSDecoder] TotalCorrCount: " << +total_corr_count << " UncorrErr: " << +uncorr_errors << std::endl;
    }
}
