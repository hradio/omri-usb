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

#include "ficparser.h"

constexpr uint16_t FicParser::CRC_CCITT_TABLE[];

//calling inherited constructor with FIC_ID
FicParser::FicParser() {
    m_fibProcessThreadRunning = false;

    m_fibProcessorThread = std::thread(&FicParser::processFib, this);
}

FicParser::~FicParser() {
    m_fibDataQueue.clear();
    m_fibProcessThreadRunning = false;

    if(m_fibProcessorThread.joinable()) {
        std::cout << M_LOG_TAG << " Joining FIB processor thread" << std::endl;
        m_fibProcessorThread.join();
    }
}

void FicParser::call(const std::vector<uint8_t> &data) {
    auto ficIter = data.begin();
    while (ficIter < data.end()) {
        std::vector<uint8_t> fib(ficIter, ficIter+32);
        if(FIB_CRC_CHECK(fib.data())) {
            m_fibDataQueue.push(fib);
        } else {
            std::cout << M_LOG_TAG << " FIB crc corrupted: " << std::hex << data[0] << " : " << data[1] << std::dec << std::endl;
        }

        ficIter += 32;

        std::vector<std::string> bla;
        bla.push_back("");
    }
}

void FicParser::processFib() {
    m_fibProcessThreadRunning = true;

    while (m_fibProcessThreadRunning) {
        std::vector<uint8_t> fibData;
        if(m_fibDataQueue.tryPop(fibData, std::chrono::milliseconds(24))) {
            auto figIter = fibData.begin();
            while(figIter < fibData.end() - 2) {
                uint8_t figType = (*figIter & 0xE0) >> 5;
                uint8_t figLength = (*figIter & 0x1F);

                ++figIter;

                if(figType != 7 && figLength != 31 && figLength > 0) {

                    switch (figType) {
                        case 0: {
                            parseFig_00(std::vector<uint8_t>(figIter, figIter+figLength));
                            break;
                        }
                        case 1: {
                            parseFig_01(std::vector<uint8_t>(figIter, figIter+figLength));
                            break;
                        }
                        default:
                            std::cout << M_LOG_TAG << "Unknown FIG Type: " << figType << std::endl;
                            break;
                    }

                    figIter += figLength;
                } else {
                    break;
                }
            }
        }
    }

    std::cout << M_LOG_TAG << "FIB Processor thread stopped" << std::endl;
}

void FicParser::parseFig_00(const std::vector<uint8_t>& ficData) {
    //ficData[0] ensured by the calling thread - figLEngth > 0
    switch (ficData[0] & 0x1F) {
        case Fig::FIG_00_TYPE::ENSEMBLE_INFORMATION: {
            Fig_00_Ext_00 extZero(ficData);

            m_fig00_00dispatcher.invoke(extZero);
            break;
        }
        case Fig::FIG_00_TYPE::BASIC_SUBCHANNEL_ORGANIZATION: {
            Fig_00_Ext_01 extOne(ficData);

            bool done{false};
            if(!contains<Fig_00_Ext_01>(m_parsedFig0001, extOne)) {
                m_parsedFig0001.push_back(extOne);
                if(m_1wasDone) {
                    std::cout << "FICParser Ext 2 done" << std::endl;
                }
            } else {
                done = true;
                m_1wasDone = true;
            }

            m_fig00_01dispatcher.invoke(extOne);
            //tell callbacks that there will be no new FIGs
            if(done) {
                m_fig00DoneDispatcher.invoke(Fig::FIG_00_TYPE::BASIC_SUBCHANNEL_ORGANIZATION);
            }
            break;
        }
        case Fig::FIG_00_TYPE::BASIC_SERVICE_COMPONENT_DEFINITION: {
            Fig_00_Ext_02 extTwo(ficData);

            bool done{false};
            if(!contains<Fig_00_Ext_02>(m_parsedFig0002, extTwo)) {
                if(m_2wasDone) {
                    std::cout << "FICParser Ext 2 was done" << std::endl;
                }
                m_parsedFig0002.push_back(extTwo);
            } else {
                done = true;
                m_2wasDone = true;
            }

            m_fig00_02dispatcher.invoke(extTwo);
            if(done) {
                m_fig00DoneDispatcher.invoke(Fig::FIG_00_TYPE::BASIC_SERVICE_COMPONENT_DEFINITION);
            }
            break;
        }
        case Fig::FIG_00_TYPE::SERVICE_COMPONENT_PACKET_MODE: {
            Fig_00_Ext_03 extThree(ficData);

            bool done{false};
            if(!contains<Fig_00_Ext_03>(m_parsedFig0003, extThree)) {
                if(m_3wasDone) {
                    std::cout << "FICParser Ext 3 was done" << std::endl;
                }
                m_parsedFig0003.push_back(extThree);
            } else {
                done = true;
                m_3wasDone = true;
            }

            m_fig00_03dispatcher.invoke(extThree);
            if(done) {
                m_fig00DoneDispatcher.invoke(Fig::FIG_00_TYPE::SERVICE_COMPONENT_PACKET_MODE);
            }
            break;
        }
        case Fig::FIG_00_TYPE::SERVICE_COMPONENT_STREAM_CA: {
            Fig_00_Ext_04 extFour(ficData);
            break;
        }
        case Fig::FIG_00_TYPE::SERVICE_COMPONENT_LANGUAGE: {
            Fig_00_Ext_05 extFive(ficData);
            break;
        }
        case Fig::FIG_00_TYPE::SERVICE_LINKING_INFORMATION: {
            Fig_00_Ext_06 extSix(ficData);

            m_fig00_06dispatcher.invoke(extSix);
            break;
        }
        case Fig::FIG_00_TYPE::CONFIGURATION_INFORMATION: {
            Fig_00_Ext_07 extSeven(ficData);
            break;
        }
        case Fig::FIG_00_TYPE::SERVICE_COMPONENT_GLOBAL_DEFINITION: {
            Fig_00_Ext_08 extEight(ficData);

            bool done{false};
            if(!contains<Fig_00_Ext_08>(m_parsedFig0008, extEight)) {
                if(m_8wasDone) {
                    std::cout << "FICParser Ext 8 done" << std::endl;
                }
                m_parsedFig0008.push_back(extEight);
            } else {
                done = true;
                m_8wasDone = true;
            }

            m_fig00_08dispatcher.invoke(extEight);
            if(done) {
                m_fig00DoneDispatcher.invoke(Fig::FIG_00_TYPE::SERVICE_COMPONENT_GLOBAL_DEFINITION);
            }
            break;
        }
        case Fig::FIG_00_TYPE::COUNTRY_LTO_INTERNATIONAL_TABLE: {
            Fig_00_Ext_09 extNine(ficData);
            m_fig00_09dispatcher.invoke(extNine);
            break;
        }
        case Fig::FIG_00_TYPE::DATE_AND_TIME: {
            Fig_00_Ext_10 extTen(ficData);
            m_fig00_10dispatcher.invoke(extTen);
            break;
        }
        case Fig::FIG_00_TYPE::USERAPPLICATION_INFORMATION: {
            Fig_00_Ext_13 ext3Ten(ficData);

            bool done{false};
            if(!contains<Fig_00_Ext_13>(m_parsedFig0013, ext3Ten)) {
                m_parsedFig0013.push_back(ext3Ten);
            } else {
                done = true;
            }

            m_fig00_13dispatcher.invoke(ext3Ten);
            if(done) {
                m_fig00DoneDispatcher.invoke(Fig::FIG_00_TYPE::USERAPPLICATION_INFORMATION);
            }
            break;
        }
        case Fig::FIG_00_TYPE::FEC_SUBCHANNEL_ORGANIZATION: {
            bool done{false};
            Fig_00_Ext_14 ext4Ten(ficData);

            if(!contains<Fig_00_Ext_14>(m_parsedFig0014, ext4Ten)) {
                m_parsedFig0014.push_back(ext4Ten);
            } else {
                done = true;
            }

            m_fig00_14dispatcher.invoke(ext4Ten);
            if(done) {
                m_fig00DoneDispatcher.invoke(Fig::FIG_00_TYPE::FEC_SUBCHANNEL_ORGANIZATION);
            }
            break;
        }
        case Fig::FIG_00_TYPE::PROGRAMME_TYPE: {
            Fig_00_Ext_17 ext7Ten(ficData);

            bool done{false};
            if(!contains<Fig_00_Ext_17>(m_parsedFig0017, ext7Ten)) {
                m_parsedFig0017.push_back(ext7Ten);
            } else {
                done = true;
            }

            m_fig00_17dispatcher.invoke(ext7Ten);
            if(done) {
                m_fig00DoneDispatcher.invoke(Fig::FIG_00_TYPE::PROGRAMME_TYPE);
            }
            break;
        }
        case Fig::FIG_00_TYPE::ANNOUNCEMENT_SUPPORT: {
            Fig_00_Ext_18 ext8Ten(ficData);
            m_fig00_18dispatcher.invoke(ext8Ten);
            break;
        }
        case Fig::FIG_00_TYPE::ANNOUNCEMENT_SWITCHING: {
            Fig_00_Ext_19 ext9Ten(ficData);
            m_fig00_19dispatcher.invoke(ext9Ten);
            break;
        }
        case Fig::FIG_00_TYPE::SERVICE_COMPONENT_INFORMATION: {
            Fig_00_Ext_20 extTwenty(ficData);
            break;
        }
        case Fig::FIG_00_TYPE::FREQUENCY_INFORMATION: {
            Fig_00_Ext_21 ext1Twenty(ficData);
            m_fig00_21dispatcher.invoke(ext1Twenty);
            break;
        }
        case Fig::FIG_00_TYPE::OE_SERVICES: {
            Fig_00_Ext_24 ext4Twenty(ficData);
            m_fig00_24dispatcher.invoke(ext4Twenty);
            break;
        }
        case Fig::FIG_00_TYPE::OE_ANNOUNCEMENT_SUPPORT: {
            Fig_00_Ext_25 ext5Twenty(ficData);
            break;
        }
        case Fig::FIG_00_TYPE::OE_ANNOUNCEMENT_SWITCHING: {
            Fig_00_Ext_26 ext6Twenty(ficData);
            break;
        }
        default:
            std::cout << M_LOG_TAG << "Unknown Extension0: " << +(ficData[0] & 0x1F) << " ##############################################################" << std::endl;
            break;
    }
}

void FicParser::parseFig_01(const std::vector<uint8_t>& ficData) {
    //ficData[0] ensured by the calling thread - figLEngth > 0
    switch(ficData[0] & 0x07) {
        case Fig::FIG_01_TYPE::ENSEMBLE_LABEL: {
            Fig_01_Ext_00 extZero(ficData);

            bool done{false};
            if(!contains<Fig_01_Ext_00>(m_parsedFig0100, extZero)) {
                m_parsedFig0100.push_back(extZero);
            } else {
                done = true;
            }

            m_fig01_00dispatcher.invoke(extZero);
            if(done) {
                m_fig01DoneDispatcher.invoke(Fig::FIG_01_TYPE::ENSEMBLE_LABEL);
            }

            break;
        }
        case Fig::FIG_01_TYPE::PROGRAMME_SERVICE_LABEL: {
            Fig_01_Ext_01 extOne(ficData);

            bool done{false};
            if(!contains<Fig_01_Ext_01>(m_parsedFig0101, extOne)) {
                m_parsedFig0101.push_back(extOne);
            } else {
                done = true;
            }

            m_fig01_01dispatcher.invoke(extOne);
            if(done) {
                m_fig01DoneDispatcher.invoke(Fig::FIG_01_TYPE::PROGRAMME_SERVICE_LABEL);
            }
            break;
        }
        case Fig::FIG_01_TYPE::SERVICE_COMPONENT_LABEL: {
            Fig_01_Ext_04 ext4(ficData);

            bool done{false};
            if(!contains<Fig_01_Ext_04>(m_parsedFig0104, ext4)) {
                m_parsedFig0104.push_back(ext4);
            } else {
                done = true;
            }

            m_fig01_04dispatcher.invoke(ext4);
            if(done) {
                m_fig01DoneDispatcher.invoke(Fig::FIG_01_TYPE::SERVICE_COMPONENT_LABEL);
            }
            break;
        }
        case Fig::FIG_01_TYPE::DATA_SERVICE_LABEL: {
            Fig_01_Ext_05 ext5(ficData);

            bool done{false};
            if(!contains<Fig_01_Ext_05>(m_parsedFig0105, ext5)) {
                m_parsedFig0105.push_back(ext5);
            } else {
                done = true;
            }

            m_fig01_05dispatcher.invoke(ext5);
            if(done) {
                m_fig01DoneDispatcher.invoke(Fig::FIG_01_TYPE::DATA_SERVICE_LABEL);
            }
            break;
        }
        case Fig::FIG_01_TYPE::XPAD_USERAPPLICATION_LABEL: {
            Fig_01_Ext_06 ext6(ficData);

            bool done{false};
            if(!contains<Fig_01_Ext_06>(m_parsedFig0106, ext6)) {
                m_parsedFig0106.push_back(ext6);
            } else {
                done = true;
            }

            m_fig01_06dispatcher.invoke(ext6);
            if(done) {
                m_fig01DoneDispatcher.invoke(Fig::FIG_01_TYPE::XPAD_USERAPPLICATION_LABEL);
            }
            break;
        }
        default:
            std::cout << M_LOG_TAG << "Unknown Extension1: " << +(ficData[0] & 0x07) << " ##############################################################" << std::endl;
            break;
    }

}

std::shared_ptr<FicParser::Fig_00_Done_Callback> FicParser::registerFig_00_Done_Callback(Fig_00_Done_Callback cb) {
    return m_fig00DoneDispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_01_Done_Callback> FicParser::registerFig_01_Done_Callback(FicParser::Fig_01_Done_Callback cb) {
    return m_fig01DoneDispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_00_Callback> FicParser::registerFig_00_00_Callback(Fig_00_00_Callback cb) {
    return m_fig00_00dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_01_Callback> FicParser::registerFig_00_01_Callback(Fig_00_01_Callback cb) {
    //newly registered callback will get all already parsed FIGs
    for(const auto& fig : m_parsedFig0001) {
        cb(fig);
    }
    return m_fig00_01dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_02_Callback> FicParser::registerFig_00_02_Callback(Fig_00_02_Callback cb) {
    for(const auto& fig : m_parsedFig0002) {
        cb(fig);
    }
    return m_fig00_02dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_03_Callback> FicParser::registerFig_00_03_Callback(Fig_00_03_Callback cb) {
    for(const auto& fig : m_parsedFig0003) {
        cb(fig);
    }
    return m_fig00_03dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_04_Callback> FicParser::registerFig_00_04_Callback(Fig_00_04_Callback cb) {
    return m_fig00_04dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_05_Callback> FicParser::registerFig_00_05_Callback(Fig_00_05_Callback cb) {
    return m_fig00_05dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_06_Callback> FicParser::registerFig_00_06_Callback(Fig_00_06_Callback cb) {
    return m_fig00_06dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_07_Callback> FicParser::registerFig_00_07_Callback(Fig_00_07_Callback cb) {
    return m_fig00_07dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_08_Callback> FicParser::registerFig_00_08_Callback(Fig_00_08_Callback cb) {
    for(const auto& fig : m_parsedFig0008) {
        cb(fig);
    }
    return m_fig00_08dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_09_Callback> FicParser::registerFig_00_09_Callback(Fig_00_09_Callback cb) {
    return m_fig00_09dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_10_Callback> FicParser::registerFig_00_10_Callback(Fig_00_10_Callback cb) {
    return m_fig00_10dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_13_Callback> FicParser::registerFig_00_13_Callback(Fig_00_13_Callback cb) {
    for(const auto& fig : m_parsedFig0013) {
        cb(fig);
    }
    return m_fig00_13dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_14_Callback> FicParser::registerFig_00_14_Callback(Fig_00_14_Callback cb) {
    for(const auto& fig : m_parsedFig0014) {
        cb(fig);
    }
    return m_fig00_14dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_17_Callback> FicParser::registerFig_00_17_Callback(Fig_00_17_Callback cb) {
    for(const auto& fig : m_parsedFig0017) {
        cb(fig);
    }
    return m_fig00_17dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_18_Callback> FicParser::registerFig_00_18_Callback(Fig_00_18_Callback cb) {
    return m_fig00_18dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_19_Callback> FicParser::registerFig_00_19_Callback(Fig_00_19_Callback cb) {
    return m_fig00_19dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_20_Callback> FicParser::registerFig_00_20_Callback(Fig_00_20_Callback cb) {
    return m_fig00_20dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_21_Callback> FicParser::registerFig_00_21_Callback(Fig_00_21_Callback cb) {
    return m_fig00_21dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_24_Callback> FicParser::registerFig_00_24_Callback(Fig_00_24_Callback cb) {
    return m_fig00_24dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_25_Callback> FicParser::registerFig_00_25_Callback(Fig_00_25_Callback cb) {
    return m_fig00_25dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_00_26_Callback> FicParser::registerFig_00_26_Callback(Fig_00_26_Callback cb) {
    return m_fig00_26dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_01_00_Callback> FicParser::registerFig_01_00_Callback(Fig_01_00_Callback cb) {
    for(const auto& fig : m_parsedFig0100) {
        cb(fig);
    }
    return m_fig01_00dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_01_01_Callback> FicParser::registerFig_01_01_Callback(Fig_01_01_Callback cb) {
    for(const auto& fig : m_parsedFig0101) {
        cb(fig);
    }
    return m_fig01_01dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_01_04_Callback> FicParser::registerFig_01_04_Callback(Fig_01_04_Callback cb) {
    for(const auto& fig : m_parsedFig0104) {
        cb(fig);
    }
    return m_fig01_04dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_01_05_Callback> FicParser::registerFig_01_05_Callback(Fig_01_05_Callback cb) {
    for(const auto& fig : m_parsedFig0105) {
        cb(fig);
    }
    return m_fig01_05dispatcher.add(cb);
}

std::shared_ptr<FicParser::Fig_01_06_Callback> FicParser::registerFig_01_06_Callback(Fig_01_06_Callback cb) {
    for(const auto& fig : m_parsedFig0106) {
        cb(fig);
    }
    return m_fig01_06dispatcher.add(cb);
}

void FicParser::reset() {
    m_fibDataQueue.clear();
    ficBuffer.clear();
    m_parsedFig0001.clear();
    m_parsedFig0002.clear();
    m_parsedFig0003.clear();
    m_parsedFig0008.clear();
    m_parsedFig0013.clear();
    m_parsedFig0014.clear();
    m_parsedFig0017.clear();
}
