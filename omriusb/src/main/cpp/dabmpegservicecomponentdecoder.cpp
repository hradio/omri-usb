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

#include "dabmpegservicecomponentdecoder.h"
#include "global_definitions.h"

#include <iomanip>

constexpr uint8_t DabMpegServiceComponentDecoder::XPAD_SIZE[8];
constexpr uint16_t DabMpegServiceComponentDecoder::M1L2_BITRATE_IDX[];
constexpr uint16_t DabMpegServiceComponentDecoder::M1_SAMPLINGFREQUENCY_IDX[];
constexpr uint8_t DabMpegServiceComponentDecoder::CHANNELMODE_IDX[];

DabMpegServiceComponentDecoder::DabMpegServiceComponentDecoder() {
    //std::cout << m_logTag << "Creating" << std::endl;

    m_processThreadRunning = true;
    m_processThread = std::thread(&DabMpegServiceComponentDecoder::processData, this);
}

DabMpegServiceComponentDecoder::~DabMpegServiceComponentDecoder() {
    //std::cout << m_logTag << "Destroying" << std::endl;

    m_processThreadRunning = false;
    m_processThread.join();
    m_conQueue.clear();
}

std::shared_ptr<DabMpegServiceComponentDecoder::PAD_DATA_CALLBACK> DabMpegServiceComponentDecoder::registerPadDataCallback(DabMpegServiceComponentDecoder::PAD_DATA_CALLBACK cb) {
    return m_padDataDispatcher.add(cb);
}

std::shared_ptr<DabMpegServiceComponentDecoder::AUDIO_COMPONENT_DATA_CALLBACK> DabMpegServiceComponentDecoder::registerAudioDataCallback(DabMpegServiceComponentDecoder::AUDIO_COMPONENT_DATA_CALLBACK cb) {
    return m_audioDataDispatcher.add(cb);
}

void DabMpegServiceComponentDecoder::flushBufferedData() {
    m_conQueue.clear();
}

void DabMpegServiceComponentDecoder::componentDataInput(const std::vector<uint8_t> &frameData, bool synchronized) {
    //std::cout << m_logTag << "componentDataInput: " << frameData.size() << " - " << +m_frameSize << " : " << +m_subChanBitrate << " : " << std::hex << std::setfill('0') << std::setw(2) << +frameData[0] << +frameData[1] << +frameData[2] << std::dec << std::endl;

    if(synchronized) {
        m_conQueue.push(frameData);
    } else {
        if(m_frameSize > 0) {
            synchronizeData(frameData);
        }
    }
}

void DabMpegServiceComponentDecoder::synchronizeData(const std::vector<uint8_t>& unsyncData) {
    std::vector<uint8_t> data;
    if(!m_unsyncDataBuffer.empty()) {
        data.insert(data.begin(), m_unsyncDataBuffer.begin(), m_unsyncDataBuffer.end());
        m_unsyncDataBuffer.clear();
    }

    data.insert(data.end(), unsyncData.begin(), unsyncData.end());

    auto unsyncIter = data.begin();
    while(unsyncIter+m_frameSize < data.end()) {
        if(*unsyncIter == 0xFF && ((*(unsyncIter+1)) & 0xF0) == 0xF0) {
            //Test for ID bit (not very elegant)
            if(!m_frameSizeAdjusted && ((*(unsyncIter+1)) & 0x08) != 0x08) {
                //std::cout << m_logTag << "Adjusting framesize for 24 kHz samplerate" << std::endl;
                m_frameSize *= 2;
                m_frameSizeAdjusted = true;
            }
            //std::cout << m_logTag << "Syncing Inserting: " << +std::distance(data.begin(), unsyncIter) << " from: " << +data.size() << std::endl;
            if(unsyncIter + m_frameSize < data.end()) {
                m_conQueue.push(std::vector<uint8_t>(unsyncIter, unsyncIter+m_frameSize));
                unsyncIter += m_frameSize;
                //std::cout << m_logTag << "Syncing Next: " << std::hex << std::setfill('0') << std::setw(2) << +(*unsyncIter) << std::hex << std::setfill('0') << std::setw(2) << +(*(unsyncIter+1)) << std::dec << std::endl;
                continue;
            } else {
                //std::cout << m_logTag << "Syncing Not enough data left: " << +unsyncData.size() << std::endl;
                break;
            }
        }
        //std::cout << m_logTag << "Searching MPEG start" << std::endl;
        ++unsyncIter;
    }

    m_unsyncDataBuffer.insert(m_unsyncDataBuffer.begin(), unsyncIter, data.end());
}

void DabMpegServiceComponentDecoder::processData() {
    while(m_processThreadRunning) {
        std::vector<uint8_t> frameData;
        if(m_conQueue.tryPop(frameData, std::chrono::milliseconds(50))) {
            auto frameIter = frameData.begin();
            while(frameIter < frameData.end()) {
                if(*frameIter++ == 0xFF && (*frameIter & 0xF0) == 0xF0) {
                    uint8_t mpegAudioVersionId = static_cast<uint8_t>((*frameIter & 0x08) >> 3);
                    uint8_t mpegAudioLayer = static_cast<uint8_t>((*frameIter & 0x06) >> 1);
                    bool crcProtection = (*frameIter++ & 0x01) != 0;

                    uint8_t bitrateIdx = static_cast<uint8_t>((*frameIter & 0xF0) >> 4);

                    //TODO depends on 48 or 24kHz sampling
                    uint16_t bitrate;
                    if(bitrateIdx <= M1L2_BITRATE_IDX_MAX) {
                        bitrate = M1L2_BITRATE_IDX[bitrateIdx];
                    } else {
                        break;
                    }

                    uint8_t samplingIdx = static_cast<uint8_t>((*frameIter & 0x0C) >> 2);
                    uint16_t samplingRate;

                    if(samplingIdx <= M1_SAMPLINGFREQUENCY_IDX_MAX) {
                        samplingRate = M1_SAMPLINGFREQUENCY_IDX[samplingIdx];
                        if(mpegAudioVersionId == 0) {
                            samplingRate /= 2;
                        }
                    } else {
                        break;
                    }

                    bool paddingFlag = ((*frameIter & 0x02) >> 1) != 0;
                    bool privateFlag = (*frameIter++ & 0x01) != 0;

                    uint8_t channelMode = static_cast<uint8_t>((*frameIter & 0xC0) >> 6);
                    uint8_t channels;
                    if(channelMode <= CHANNELMODE_IDX_MAX) {
                        channels = CHANNELMODE_IDX[channelMode];
                    } else {
                        break;
                    }

                    //std::cout << m_logTag << " ID: " << +mpegAudioVersionId << " SamplingIdx: " << +samplingIdx << " SampleRate: " << samplingRate << " Channels: " << +channels << std::endl;

                    uint8_t modeExtension = static_cast<uint8_t>((*frameIter & 0x30) >> 4);
                    bool copyRight = ((*frameIter & 0x08) >> 3) != 0;
                    bool original = ((*frameIter & 0x04) >> 2) != 0;
                    uint8_t emphasis = static_cast<uint8_t>(*frameIter++ & 0x03);

                    uint8_t scaleCrcLen;
                    if (channelMode == 0x03) {
                        if(bitrateIdx > 0x02 ) {
                            scaleCrcLen = 4;
                        } else {
                            scaleCrcLen = 2;
                        }
                    } else {
                        if (bitrateIdx > 0x06) {
                            scaleCrcLen = 4;
                        } else {
                            scaleCrcLen = 2;
                        }
                    }

                    auto padRiter = frameData.rbegin();
                    while (padRiter < frameData.rend()) {
                        //F-PAD
                        bool ciPresent = ((*padRiter & 0x02) >> 1) != 0;
                        bool z = (*padRiter & 0x01) != 0;
                        //if(z) {
                        //    std::cout << m_logTag << " Z-Field: " << std::boolalpha << z << std::noboolalpha << std::endl;
                        //}
                        ++padRiter; //Byte L data field

                        uint8_t fPadType = static_cast<uint8_t>((*padRiter & 0xC0) >> 6); //Byte L-1 data field
                        uint8_t xPadIndicator = static_cast<uint8_t>((*padRiter++ & 0x30) >> 4);

                        if(fPadType == 0) {
                            padRiter += scaleCrcLen;
                            auto scaleRiterPos = padRiter;

                            switch (xPadIndicator) {
                                case 0: {
                                    //m_audioDataDispatcher.invoke(frameData, 0, channels, samplingRate, false);
                                    break;
                                }
                                case 1: {
                                    std::vector<uint8_t> padData;
                                    //padData.insert(padData.end(), frameData.end()-1-2-scaleCrcLen-4, frameData.end()-1-2-scaleCrcLen);
                                    padData.insert(padData.end(), frameData.end()-2-scaleCrcLen-4, frameData.end()-2-scaleCrcLen);
                                    //padData.insert(padData.end(), frameData.end()-1-2, frameData.end());
                                    padData.insert(padData.end(), frameData.end()-2, frameData.end());

                                    //std::string dl(padData.begin(), padData.end());
                                    //std::cout << m_logTag << "Short PAD: " << dl << std::endl;
                                    //m_audioDataDispatcher.invoke(frameData, 0, channels, samplingRate, false);
                                    m_padDataDispatcher.invoke(padData);

                                    break;
                                }
                                case 2: {
                                    //std::vector<uint8_t> audioData;
                                    std::vector<uint8_t> padData;

                                    if(ciPresent) {
                                        m_noCiLastLength = 0;
                                        for(int i = 0; i < 4; i++) {
                                            uint8_t xpadLengthIndex = static_cast<uint8_t>((*padRiter & 0xE0) >> 5);
                                            uint8_t xpadAppType = static_cast<uint8_t>(*padRiter++ & 0x1f);

                                            ++m_noCiLastLength;
                                            if(xpadAppType == 0) {
                                                break;
                                            }

                                            m_noCiLastLength += XPAD_SIZE[xpadLengthIndex];
                                        }

                                        padRiter += (m_noCiLastLength-1);

                                        padData.insert(padData.end(), frameData.end()-1-2-scaleCrcLen-m_noCiLastLength, frameData.end()-2-scaleCrcLen);
                                        padData.insert(padData.end(), frameData.end()-2, frameData.end());

                                        //audioData.insert(audioData.end(), frameData.begin(), frameData.end()-1-2-scaleCrcLen-m_noCiLastLength);
                                        //m_audioDataDispatcher.invoke(frameData, 0, channels, samplingRate, false);
                                        m_padDataDispatcher.invoke(padData);
                                    } else {
                                        if(m_noCiLastLength > 0) {
                                            padData.insert(padData.end(), frameData.end()-2-scaleCrcLen-m_noCiLastLength, frameData.end()-2-scaleCrcLen);
                                            padData.insert(padData.end(), frameData.end()-2, frameData.end());

                                            //audioData.insert(audioData.end(), frameData.begin(), frameData.end()-1-2-scaleCrcLen-m_noCiLastLength);
                                            //m_audioDataDispatcher.invoke(frameData, 0, channels, samplingRate, false);

                                            m_padDataDispatcher.invoke(padData);
                                        }
                                    }
                                    break;
                                }
                            default:
                                break;
                            }

                            m_audioDataDispatcher.invoke(frameData, 0, channels, samplingRate, false, false);
                        } else {
                            std::cout << m_logTag << " Wrong FPAD Type: " << +fPadType << std::endl;
                        }

                        break;
                    }

                    break;
                } else {
                    std::cout << m_logTag << "Wrong MPEG Syncword" << std::endl;
                    break;
                }
            }
        }
    }
}
