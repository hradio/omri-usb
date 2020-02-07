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

#include <iomanip>
#include "raontunerinput.h"

constexpr uint8_t RaonTunerInput::g_abAdcClkSynTbl[4][7];
constexpr uint8_t RaonTunerInput::g_aeAdcClkTypeTbl_DAB_B3[];
constexpr int RaonTunerInput::g_atPllNF_DAB_BAND3[];
constexpr uint_t RaonTunerInput::AntLvlTbl[DAB_MAX_NUM_ANTENNA_LEVEL];

RaonTunerInput::RaonTunerInput(std::shared_ptr<JTunerUsbDevice> usbDevice) : m_usbDevice{usbDevice} {
    std::cout << LOG_TAG << "Constructing...." << std::endl;

    m_usbDevice->requestPermission([&](bool granted) {
        std::cout << LOG_TAG << (m_usbDevice != nullptr ? (m_usbDevice.get()->getDeviceName()) : "NULL") << " PermissionCallback: " << granted << std::endl;
        if(granted) {
            m_commandQueue.push(std::bind(&RaonTunerInput::initializeSync, this));
            startReadDataThread();
        }
    });

    m_ensembleFinishedCb = DabEnsemble::registerEnsembleCollectDoneCallback(std::bind(&RaonTunerInput::ensembleCollectFinished, this));
}

RaonTunerInput::~RaonTunerInput() {
    std::cout << LOG_TAG << "destructing...." << std::endl;

    stopReadFicThread();

    m_scanCommandThreadRunning = false;
    if(m_scanCommandThread.joinable()) {
        m_scanCommandThread.join();
    }

    stopReadDataThread();
}

void RaonTunerInput::initialize() {
    m_commandQueue.push(std::bind(&RaonTunerInput::initializeSync, this));
}

void RaonTunerInput::initializeSync() {
    if(!m_isInitialized) {
        if(tunerPowerUp()) {
            configurePowerType();
            configureAddClock();
            tdmbInitTop();
            tdmbInitComm();

            tdmbInitHost();
            tdmbInitOfdm();
            tdmbInitFEC();

            rtvResetMemoryFIC();
            rtvOEMConfigureInterrupt();
            rtvStreamDisable();
            rtvConfigureHostIF();
            rtvRFInitilize();
            rtvRfSpecial();

            setupMscThreshold();
            setupMemoryFIC();

            m_isInitialized = true;
            m_usbDevice->callCallback(JTunerUsbDevice::TUNER_CALLBACK_TYPE::TUNER_CALLBACK_READY);
        }
    }
}

bool RaonTunerInput::isInitialized() const {
    return m_isInitialized;
}

int RaonTunerInput::getCurrentTunedFrequency() const {
    return m_currentFrequency;
}

void RaonTunerInput::tuneFrequency(int frequencyKHz) {
    if(!m_isScanning) {
        m_commandQueue.push(std::bind(&RaonTunerInput::tuneFrequencySync, this, frequencyKHz));
    } else {
        tuneFrequencySync(frequencyKHz);
    }
}

void RaonTunerInput::tuneFrequencySync(int frequencyKHz) {
    if(!m_isInitialized) {
        std::cout << LOG_TAG << "Device not initialized" << std::endl;
        return;
    }

    std::cout << LOG_TAG << "Tuning Frequency: " << +frequencyKHz << std::endl;

    if(!m_isScanning) {
        //stopReadDataThread();
    } else {
        stopReadFicThread();
    }

    reset();

    m_ensembleFrequency = static_cast<uint32_t>(frequencyKHz);
    m_currentFrequency = static_cast<uint32_t>(frequencyKHz);

    setFrequency(m_ensembleFrequency);

    softReset();
    if(!m_isScanning) {
        //startReadDataThread();
    } else {
        startReadFicThread();
    }
}

const DabEnsemble &RaonTunerInput::getEnsemble() const {
    return *this;
}

int RaonTunerInput::getMaximumConcurrentSubChannels() const {
    return MAXIMUM_CONCURRENT_SUBCHANNELS;
}

void RaonTunerInput::addMscCallback(DabInput::CallbackFunction cb, uint8_t subchanId) {

}

void RaonTunerInput::addFicCallback(DabInput::CallbackFunction cb) {

}

void RaonTunerInput::startService(std::shared_ptr<JDabService> serviceLink) {
    //check if this service is already running
    if(m_startServiceLink != nullptr) {
        if(m_startServiceLink->getEnsembleFrequency() == serviceLink->getEnsembleFrequency() &&
           m_startServiceLink->getEnsembleEcc() == serviceLink->getEnsembleEcc() &&
           m_startServiceLink->getEnsembleId() == serviceLink->getEnsembleId() &&
           m_startServiceLink->getServiceId() == serviceLink->getServiceId()) {
            std::cout << LOG_TAG << "Starting service is already running" << std::endl;

            //TODO really call 'serviceStarted' if the service is already running?
            //m_usbDevice->serviceStarted(m_startServiceLink->getJavaDabServiceObject());
            return;
        }
    }
    m_commandQueue.push(std::bind(&RaonTunerInput::startServiceSync, this, serviceLink));
}

void RaonTunerInput::startServiceSync(std::shared_ptr<JDabService> serviceLink) {
    std::cout << LOG_TAG << "Starting service..." << std::endl;

    if(m_isScanning) {
        return;
    }

    if(m_startServiceLink != nullptr) {
        m_startServiceLink->decodeAudio(false);
        if(m_startServiceLink->getJavaDabServiceObject() != nullptr) {
            m_usbDevice->serviceStopped(m_startServiceLink->getJavaDabServiceObject());
        }
    }

    if(m_currentFrequency != serviceLink.get()->getEnsembleFrequency()) {
        m_ensembleCollectFinished = false;
        tuneFrequency(serviceLink.get()->getEnsembleFrequency());
        m_startServiceLink = serviceLink;
        return;
    }

    if(m_ensembleCollectFinished) {
        m_startServiceLink = serviceLink;
        setService();
        return;
    }
}

void RaonTunerInput::stopService(const DabService &service) {
    m_commandQueue.push(std::bind(&RaonTunerInput::closeSubchannel, this, m_currentSubchanId));
}

void RaonTunerInput::stopAllRunningServices() {
    m_commandQueue.push(std::bind(&RaonTunerInput::closeSubchannel, this, m_currentSubchanId));
}

void RaonTunerInput::commandProcessing() {
    while(m_commandThreadRunning) {
        std::function<void(void)> command;
        if (m_commandQueue.tryPop(command, std::chrono::milliseconds(2))) {
            command();
        } else {
            /*
            --m_antLvlCnt;
            if(!m_antLvlCnt) {
                std::cout << LOG_TAG << "Pushing GetAntennaLevel" << std::endl;
                m_commandQueue.push(std::bind(&RaonTunerInput::getAntennaLevel, this));
                m_antLvlCnt = 100;
            }

            uint8_t int_type_val1 = readRegister(INT_E_STATL);
            bool ofdmLock = static_cast<bool>((int_type_val1 & 0x80u) >> 7u);
            bool msc1Overrun = (int_type_val1 & MSC1_E_OVER_FLOW) >> 6u;
            bool msc1Underrun = (int_type_val1 & MSC1_E_UNDER_FLOW) >> 5u;
            bool msc1Int = (int_type_val1 & MSC1_E_INT) >> 4u;
            bool msc0Overrun = (int_type_val1 & MSC0_E_OVER_FLOW) >> 3u;
            bool msc0Underrun = (int_type_val1 & MSC0_E_UNDER_FLOW) >> 2u;
            bool msc0Int = (int_type_val1 & MSC0_E_INT) >> 1u;
            bool ficInt = (int_type_val1 & FIC_E_INT);

            if(ficInt) {
                std::cout << LOG_TAG << "Pushing ReadFic" << std::endl;
                m_commandQueue.push(std::bind(&RaonTunerInput::readFicData, this));
            }

            if(msc1Int) {
                std::cout << LOG_TAG << "Pushing ReadMsc" << std::endl;
                m_commandQueue.push(std::bind(&RaonTunerInput::readMscData, this));
            }

            if(msc1Overrun || msc1Underrun) {
                std::cout << LOG_TAG << "Pushing ClearMsc" << std::endl;
                m_commandQueue.push(std::bind(&RaonTunerInput::clearMscBuffer, this));
            }
            */

            m_commandQueue.push(std::bind(&RaonTunerInput::readData, this));
        }
    }

    std::cout << LOG_TAG << (m_usbDevice != nullptr ? (m_usbDevice.get()->getDeviceName()) : "NULL") << " CommandQ Process Thread stopped" << std::endl;

}

void RaonTunerInput::processScanCommands() {
    while(m_scanCommandThreadRunning) {
        std::function<void(void)> command;
        if (m_scanCommandQueue.tryPop(command, std::chrono::milliseconds(24))) {
            command();
        }
    }

    std::cout << LOG_TAG << (m_usbDevice != nullptr ? (m_usbDevice.get()->getDeviceName()) : "NULL") << " ScanCommandQ Process Thread stopped" << std::endl;

}

void RaonTunerInput::startScanCommand() {
    std::cout << LOG_TAG << (m_usbDevice != nullptr ? (m_usbDevice.get()->getDeviceName()) : "NULL") << " Starting service scan!" << std::endl;
    m_isScanning = true;
    m_currentScanningEnsembleNum = 0;
    m_maxCollectionWaitLoops = MAX_COLLECTION_LOOPS;
    m_ficCollectionWaitLoops = 300;
    tuneFrequency(DAB_FREQ_TABLE_MHZ[m_currentScanningEnsembleNum] * 1000);
    m_startServiceLink = nullptr;
    m_usbDevice->callCallback(JTunerUsbDevice::TUNER_CALLBACK_TYPE::TUNER_SCAN_IN_PROGRESS);
    m_usbDevice->scanProgress(0);
}

void RaonTunerInput::stopScanCommand() {
    std::cout << LOG_TAG << (m_usbDevice != nullptr ? (m_usbDevice.get()->getDeviceName()) : "NULL") << " Stopping service scan!" << std::endl;

    stopReadFicThread();
    m_isScanning = false;
    m_currentScanningEnsembleNum = 0;
    tuneFrequency(0xFFFFFFFF);
    m_usbDevice->callCallback(JTunerUsbDevice::TUNER_CALLBACK_TYPE::TUNER_CALLBACK_READY);
    startReadDataThread();
}

void RaonTunerInput::startServiceScan() {
    if(!m_isScanning) {
        stopReadDataThread();

        m_scanCommandThreadRunning = false;
        if(m_scanCommandThread.joinable()) {
            m_scanCommandThread.join();
        }

        m_scanCommandThreadRunning = true;
        m_scanCommandThread = std::thread(&RaonTunerInput::processScanCommands, this);
        m_scanCommandQueue.push(std::bind(&RaonTunerInput::startScanCommand, this));
    }
}

void RaonTunerInput::stopServiceScan() {
    if(m_isScanning) {
        m_scanCommandQueue.push(std::bind(&RaonTunerInput::stopScanCommand, this));
    }
}

void RaonTunerInput::scanNext() {
    std::cout << LOG_TAG << (m_usbDevice != nullptr ? (m_usbDevice.get()->getDeviceName()) : "NULL") << " Scanning next Ensemble!" << std::endl;

    m_maxCollectionWaitLoops = MAX_COLLECTION_LOOPS;
    m_ficCollectionWaitLoops = 300;

    if (m_currentScanningEnsembleNum + 1 < NUM_DAB_ENSEMBLES) {
        std::cout << LOG_TAG << (m_usbDevice != nullptr ? (m_usbDevice.get()->getDeviceName()) : "NULL") << " Scan next Ensemble: " << +DAB_FREQ_TABLE_MHZ[m_currentScanningEnsembleNum + 1] << std::endl;
        if(m_ensembleCollectFinished) {
            for (int i = 0; i < NUM_DAB_N_FREQUENCIES; i++) {
                if((m_currentScanningEnsembleNum + 1) == DAB_N_FREQUENCIES_IDX[i]) {
                    std::cout << LOG_TAG << "Skiping NFrequency: " << +DAB_N_FREQUENCIES_IDX[i] << std::endl;
                    ++m_currentScanningEnsembleNum;
                    break;
                }
            }
        }
        tuneFrequency(DAB_FREQ_TABLE_MHZ[++m_currentScanningEnsembleNum] * 1000);

        m_usbDevice->scanProgress(m_currentScanningEnsembleNum*100/NUM_DAB_ENSEMBLES);
    } else {
        std::cout << LOG_TAG << (m_usbDevice != nullptr ? (m_usbDevice.get()->getDeviceName()) : "NULL") << " Scan finished: " << +m_currentScanningEnsembleNum << std::endl;
        m_usbDevice->scanProgress(100);
        m_usbDevice->callCallback(JTunerUsbDevice::TUNER_CALLBACK_TYPE::TUNER_CALLBACK_READY);
        m_isScanning = false;
        m_currentScanningEnsembleNum = 0;

        stopReadFicThread();
        startReadDataThread();
    }
}

std::string RaonTunerInput::getDeviceName() const {
    return m_usbDevice->getDeviceName();
}

//
void RaonTunerInput::threadedFicRead() {

    do {
        readFic();
    } while(m_readFicThreadRunning);
}

void RaonTunerInput::threadedMscRead() {

    do {
        readMsc();
    } while(m_readMscThreadRunning);
}

void RaonTunerInput::threadedDataRead() {
    do {
        readData();
    } while(m_readDataThreadRunning);
}

void RaonTunerInput::setService() {
    if(m_startServiceLink != nullptr) {
        std::cout << LOG_TAG << "Starting service" << std::endl;

        if(m_startServiceLink->getEnsembleFrequency() != m_currentFrequency) {
            m_commandQueue.push(std::bind(&RaonTunerInput::tuneFrequencySync, this, m_startServiceLink->getEnsembleFrequency()));
            return;
        }

        for(const auto& srv : getDabServices()) {
            if(srv->getServiceId() == m_startServiceLink->getServiceId()) {
                m_startServiceLink->setLinkDabService(srv);

                for (const auto& srvComp : srv->getServiceComponents()) {
                    if(srvComp->isPrimary()) {
                        std::cout << LOG_TAG << "Starting SrvComp: " << std::hex << +srvComp->getSubChannelId() << std::dec << std::endl;
                        m_currentSubchanId = srvComp->getSubChannelId();

                        clearAndSetupMscMemory();
                        openSubChannel(srvComp->getSubChannelId());
                        break;
                    }
                }

                m_startServiceLink->decodeAudio(true);
                m_usbDevice->serviceStarted(m_startServiceLink->getJavaDabServiceObject());
                break;
            }
        }
    }
}

void RaonTunerInput::ensembleCollectFinished() {
    std::cout << LOG_TAG << "Ensemble collect finished" << std::endl;

    if(m_isScanning) {
        m_usbDevice->ensembleReady(getEnsemble());
        m_scanCommandQueue.push(std::bind(&RaonTunerInput::scanNext, this));
    }

    setService();
    return;
}
//

//TUNER METHODS START

bool RaonTunerInput::tunerPowerUp() {
    //trying some time to power the chip up
    for(int i = 0; i < 100; i++) {
        switchPage(REGISTER_PAGE_HOST);
        setRegister(0x7D, 0x06);
        if(readRegister(0x7D) == 0x06) {
            std::cout << LOG_TAG << " PowerUp okay!" << std::endl;
            return true;
        }
    }

    std::cout << LOG_TAG << " PowerUp failed!" << std::endl;
    return false;
}

void RaonTunerInput::switchPage(RaonTunerInput::REGISTER_PAGE regPage) {
    std::vector<uint8_t> switchData{0x21, 0x00, 0x00, 0x02, 0x03, static_cast<uint8_t >(regPage)};
    int bytesTransfered = m_usbDevice->writeBulkTransferData(RAON_ENDPOINT_OUT, switchData);
}

void RaonTunerInput::setRegister(uint8_t reg, uint8_t val) {
    std::vector<uint8_t> setRegData{0x21, 0x00, 0x00, 0x02, reg, val};
    int bytesTransfered = m_usbDevice->writeBulkTransferData(RAON_ENDPOINT_OUT, setRegData);
}

uint8_t RaonTunerInput::readRegister(uint8_t reg) {
    std::vector<uint8_t> readRegReqData{0x22, 0x00, 0x01, 0x00, reg};
    int bytesTransfered = m_usbDevice->writeBulkTransferData(RAON_ENDPOINT_OUT, readRegReqData);

    std::vector<uint8_t> readBuffer(5);
    bytesTransfered = m_usbDevice->readBulkTransferData(RAON_ENDPOINT_IN, readBuffer);

    return readBuffer[4];
}

void RaonTunerInput::configurePowerType() {
    uint8_t REG30 = 0xF2 & 0xF0; /*IOLDOCON__REG*/

    //DEFINE RTV_IO_1_8V
    uint8_t io_type = 0x02;

    REG30 = (REG30 | (io_type << 1)); /*IO Type Select.*/

    uint8_t REG2F = 0x61; /*DCDC_OUTSEL = 0x03,*/
    uint8_t REG52 = 0x07; /*LDODIG_HT = 0x07;*/
    uint8_t REG54 = 0x1C;

    REG2F = static_cast<uint8_t>(REG2F | 0x10); /*PDDCDC_I2C = 1, PDLDO12_I2C = 0 ;*/

    switchPage(REGISTER_PAGE_RF);

    setRegister(0x54, REG54);
    setRegister(0x52, REG52);
    setRegister(0x30, REG30);
    setRegister(0x2F, REG2F);
}

void RaonTunerInput::configureAddClock() {
    uint8_t REGE8 = (0x46 & 0xC0);
    uint8_t REGE9 = (0x54 & 0xF0);
    uint8_t REGEA = (0x07 & 0xC0);
    uint8_t REGEB = (0x27 & 0xC0);
    uint8_t REGEC = (0x1E & 0xC0);
    uint8_t REGED = (0x18 & 0x00);
    uint8_t REGEE = (0xB8 & 0x00);

    switchPage(REGISTER_PAGE_RF);

    setRegister(0xE8, (REGE8 | RaonTunerInput::g_abAdcClkSynTbl[0][0]));
    setRegister(0xE9, (REGE9 | RaonTunerInput::g_abAdcClkSynTbl[0][1]));
    setRegister(0xEA, (REGEA | RaonTunerInput::g_abAdcClkSynTbl[0][2]));
    setRegister(0xEB, (REGEB | RaonTunerInput::g_abAdcClkSynTbl[0][3]));
    setRegister(0xEC, (REGEC | RaonTunerInput::g_abAdcClkSynTbl[0][4]));
    setRegister(0xED, (REGED | RaonTunerInput::g_abAdcClkSynTbl[0][5]));
    setRegister(0xEE, (REGEE | RaonTunerInput::g_abAdcClkSynTbl[0][6]));

    int i = 0;
    for(i = 0; i < 10; i++) {
        uint8_t RD15 = readRegister(0x15);
        RD15 = static_cast<uint8_t>(RD15 & 0x01);
        if(RD15 > 0) {
            break;
        } else {
            std::cout << LOG_TAG << " ADCClock SYNTH 1st step  UnLock..." << std::endl;
        }
    }
}

bool RaonTunerInput::changedAdcClock(uint8_t g_aeAdcClkTypeTbl_DAB_B3) {
    uint8_t RD15;

    //raontv_rf.c line 659
    uint8_t REGE8 = (0x46 & 0xC0);
    uint8_t REGE9 = (0x54 & 0xF0);
    uint8_t REGEA = (0x07 & 0xC0);
    uint8_t REGEB = (0x27 & 0xC0);
    uint8_t REGEC = (0x1E & 0xC0);
    uint8_t REGED = (0x18 & 0x00);
    uint8_t REGEE = (0xB8 & 0x00);

    switchPage(REGISTER_PAGE_RF);

    setRegister(0xE8, (REGE8 | g_abAdcClkSynTbl[g_aeAdcClkTypeTbl_DAB_B3][0]));
    setRegister(0xE9, (REGE9 | g_abAdcClkSynTbl[g_aeAdcClkTypeTbl_DAB_B3][1]));
    setRegister(0xEA, (REGEA | g_abAdcClkSynTbl[g_aeAdcClkTypeTbl_DAB_B3][2]));
    setRegister(0xEB, (REGEB | g_abAdcClkSynTbl[g_aeAdcClkTypeTbl_DAB_B3][3]));
    setRegister(0xEC, (REGEC | g_abAdcClkSynTbl[g_aeAdcClkTypeTbl_DAB_B3][4]));
    setRegister(0xED, (REGED | g_abAdcClkSynTbl[g_aeAdcClkTypeTbl_DAB_B3][5]));
    setRegister(0xEE, (REGEE | g_abAdcClkSynTbl[g_aeAdcClkTypeTbl_DAB_B3][6]));

    int i{0};
    bool setOk{false};
    for(i = 0; i < 10; i++) {
        RD15 = readRegister(0x15);
        RD15 = static_cast<uint8_t>(RD15 & 0x01);
        if(RD15 > 0) {
            setOk = true;
            break;
        }
    }

    switch(g_aeAdcClkTypeTbl_DAB_B3) {
        case RTV_ADC_CLK_FREQ_8_MHz: {
            switchPage(REGISTER_PAGE_COMM);
            setRegister(0x6A, 0x01);

            switchPage(REGISTER_PAGE_FM);
            setRegister(0x3c,0x4B);
            setRegister(0x3d,0x37);
            setRegister(0x3e,0x89);
            setRegister(0x3f,0x41);
            setRegister(0x54, 0x58);

            setRegister(0x40,0x8F); //PNCO
            setRegister(0x41,0xC2); //PNCO
            setRegister(0x42,0xF5); //PNCO
            setRegister(0x43,0x00); //PNCO
            break;
        }
        case RTV_ADC_CLK_FREQ_8_192_MHz: {
            switchPage(REGISTER_PAGE_COMM);
            setRegister(0x6A, 0x01);

            switchPage(REGISTER_PAGE_FM);
            setRegister(0x3c,0x00);
            setRegister(0x3d,0x00);
            setRegister(0x3e,0x00);
            setRegister(0x3f,0x40);
            setRegister(0x54, 0x58);

            setRegister(0x40,0x00); //PNCO
            setRegister(0x41,0x00); //PNCO
            setRegister(0x42,0xF0); //PNCO
            setRegister(0x43,0x00); //PNCO
            break;
        }
        case RTV_ADC_CLK_FREQ_9_MHz: {
            switchPage(REGISTER_PAGE_COMM);
            setRegister(0x6A, 0x01);

            switchPage(REGISTER_PAGE_FM);
            setRegister(0x3c,0xB5);
            setRegister(0x3d,0x14);
            setRegister(0x3e,0x41);
            setRegister(0x3f,0x3A);
            setRegister(0x54, 0x58);

            setRegister(0x40,0x0D); //PNCO
            setRegister(0x41,0x74); //PNCO
            setRegister(0x42,0xDA); //PNCO
            setRegister(0x43,0x00); //PNCO
            break;
        }
        case RTV_ADC_CLK_FREQ_9_6_MHz: {
            switchPage(REGISTER_PAGE_COMM);
            setRegister( 0x6A,  0x31);

            switchPage(REGISTER_PAGE_FM);
            setRegister(0x3c,0x69);
            setRegister(0x3d,0x03);
            setRegister(0x3e,0x9D);
            setRegister(0x3f,0x36);
            setRegister(0x54, 0x58);

            setRegister(0x40,0xCC); //PNCO
            setRegister(0x41,0xCC); //PNCO
            setRegister(0x42,0xCC); //PNCO
            setRegister(0x43,0x00); //PNCO
            break;
        }
    }

    return setOk;
}

void RaonTunerInput::tdmbInitTop() {
    switchPage(REGISTER_PAGE_FM);

    setRegister(0x07, 0x08);
    setRegister(0x05, 0x17);
    setRegister(0x06, 0x10);
    setRegister(0x0A, 0x00);
}

void RaonTunerInput::tdmbInitComm() {
    switchPage(REGISTER_PAGE_COMM);

    setRegister(0x10, 0x91);
    setRegister(0xE1, 0x00);

    setRegister(0x35, 0x7B);
    setRegister(0x3B, 0x3C);

    setRegister(0x36, 0x68);
    setRegister(0x3A, 0x44);

    setRegister(0x3C, 0x20);
    setRegister(0x3D, 0x0B);
    setRegister(0x3D, 0x09);

    setRegister(0xA6, 0x30); //0x30 ==>NO TSOUT@Error packet, 0x10 ==> NULL PID PACKET@Error packet

    setRegister(0xAA, 0x01); //Enable 0x47 insertion to video frame.

    setRegister(0xAF, 0x07); //FEC
}

void RaonTunerInput::tdmbInitHost() {
    switchPage(REGISTER_PAGE_HOST);

    setRegister(0x10, 0x00);
    setRegister(0x13, 0x16);
    setRegister(0x14, 0x00);
    setRegister(0x19, 0x0A);
    setRegister(0xF0, 0x00);
    setRegister(0xF1, 0x00);
    setRegister(0xF2, 0x00);
    setRegister(0xF3, 0x00);
    setRegister(0xF4, 0x00);
    setRegister(0xF5, 0x00);
    setRegister(0xF6, 0x00);
    setRegister(0xF7, 0x00);
    setRegister(0xF8, 0x00);
    setRegister(0xFB, 0xFF);
}

void RaonTunerInput::tdmbInitOfdm() {
    uint8_t INV_MODE = 0x1;
    uint8_t PWM_COM = 0x08;
    uint8_t WAGC_COM = 0x03;
    uint8_t AGC_MODE = 0x06;
    uint8_t POST_INIT = 0x09;
    uint8_t AGC_CYCLE = 0x10;

    switchPage(REGISTER_PAGE_FM);

    setRegister(0x12, 0x04);
    setRegister(0x13, 0x72);
    setRegister(0x14, 0x63);
    setRegister(0x15, 0x64);
    setRegister(0x16, 0x6C);
    setRegister(0x38, 0x01);

    setRegister(0x1A, 0xB4);

    setRegister(0x20, 0x5B);
    setRegister(0x25, 0x09);

    setRegister(0x44, (0x00 | POST_INIT));

    setRegister(0x46, 0xA0);
    setRegister(0x47, 0x0F);

    setRegister(0x48, 0xB8);
    setRegister(0x49, 0x0B);
    setRegister(0x54, 0x58);

    setRegister(0x55, 0x06);

    setRegister(0x56, (0x00 | AGC_CYCLE));

    setRegister(0x59, 0x51);

    setRegister(0x5A, 0x1C);

    setRegister(0x6D, 0x00);
    setRegister(0x8B, 0x34);
    setRegister(0x6A, 0x1C);

    setRegister(0x6B, 0x2D);
    setRegister(0x85, 0x32);

    setRegister(0x8D, 0x0C);
    setRegister(0x8E, 0x01);

    setRegister(0x33, (0x00 | (INV_MODE << 1)) );
    setRegister(0x53, (0x00 | AGC_MODE) );

    setRegister(0x6F, (0x00 | WAGC_COM) );

    setRegister(0xBA, PWM_COM);


    switchPage(REGISTER_PAGE_COMM);

    setRegister(0x6A, 0x01);

    switchPage(REGISTER_PAGE_FM); //instead of OFDM page

    setRegister(0x3C, 0x4B);
    setRegister(0x3D, 0x37);
    setRegister(0x3E, 0x89);
    setRegister(0x3F, 0x41);


    setRegister(0x40, 0x8F);

    setRegister(0x41, 0xC2);
    
    setRegister(0x42, 0xF5);
    setRegister(0x43, 0x00);

    setRegister(0x94, 0x08);

    setRegister(0x98, 0x05);
    setRegister(0x99, 0x03);
    setRegister(0x9B, 0xCF);
    setRegister(0x9C, 0x10);
    setRegister(0x9D, 0x1C);
    setRegister(0x9F, 0x32);
    setRegister(0xA0, 0x90);

    setRegister(0xA2, 0xA0);

    setRegister(0xA3, 0x08);

    setRegister(0xA4, 0x01);

    setRegister(0xA8, 0xF6);
    setRegister(0xA9, 0x89);
    setRegister(0xAA, 0x0C);
    setRegister(0xAB, 0x32);

    setRegister(0xAC, 0x14);
    setRegister(0xAD, 0x09);

    setRegister(0xAE, 0xFF);

    setRegister(0xEB, 0x6B);

    setRegister(0x93,0x10);
    setRegister(0x94,0x29);
    setRegister(0xA2,0x50);
    setRegister(0xA4,0x02);
    setRegister(0xAF,0x01);
}

void RaonTunerInput::tdmbInitFEC() {
    switchPage(REGISTER_PAGE_DD); //instead of FEC_PAGE

    setRegister(0x80, 0x80);
    setRegister(0x81, 0xFF);
    setRegister(0x87, 0x07);

    setRegister(0x45, 0xA1);

    setRegister(0xDD, 0xD0);
    setRegister(0x39, 0x07);
    setRegister(0xE6, 0x10);
    setRegister(0xA5, 0xA0);
}

void RaonTunerInput::rtvResetMemoryFIC() {
    switchPage(REGISTER_PAGE_DD);
    setRegister(0x46, 0x00);
}

void RaonTunerInput::rtvOEMConfigureInterrupt() {
    switchPage(REGISTER_PAGE_DD);

    setRegister(0x09, 0x00); /* [6]INT1 [5]INT0 - 1: Input mode, 0: Output mode */

    setRegister(0x0B, 0x00); /* [2]INT1 PAD disable [1]INT0 PAD disable */

    switchPage(REGISTER_PAGE_HOST);

    setRegister(0x28, 0x01); /* [5:3]INT1 out sel [2:0] INI0 out sel - 0:Toggle 1:Level,, 2:"0", 3:"1"*/

    setRegister(0x2A, 0x13); /* [5]INT1 pol [4]INT0 pol - 0:Active High, 1:Active Low [3:0] Period = (INT_TIME+1)/8MHz*/
}

void RaonTunerInput::rtvStreamDisable() {
    switchPage(REGISTER_PAGE_HOST);

    setRegister(0x29, 0x08);
}

void RaonTunerInput::rtvConfigureHostIF() {
    switchPage(REGISTER_PAGE_HOST);

    setRegister(0x77, 0x14); /*SPI Mode Enable*/
    setRegister(0x04, 0x28); /*SPI Mode Enable*/
    setRegister(0x0C, 0xF5);
}

void RaonTunerInput::rtvRFInitilize() {
    switchPage(REGISTER_PAGE_RF);

    setRegister(0x27, 0x96);
    setRegister(0x2B, 0x88);
    setRegister(0x2D, 0xEC);
    setRegister(0x2E, 0xB0);
    setRegister(0x31, 0x04);
    setRegister(0x34, 0xF8);
    setRegister(0x35, 0x14);
    setRegister(0x3A, 0x62);
    setRegister(0x3B, 0x74);
    setRegister(0x3C, 0xA8);
    setRegister(0x43, 0x36);
    setRegister(0x44, 0x70);
    setRegister(0x46, 0x07);
    setRegister(0x49, 0x1F);
    setRegister(0x4A, 0x50);
    setRegister(0x4B, 0x80);
    setRegister(0x53, 0x20);
    setRegister(0x55, 0xD6);
    setRegister(0x5A, 0x83);
    setRegister(0x60, 0x13);
    setRegister(0x6B, 0x85);
    setRegister(0x6E, 0x88);
    setRegister(0x6F, 0x78);
    setRegister(0x72, 0xF0);
    setRegister(0x73, 0xCA);
    setRegister(0x77, 0x80);
    setRegister(0x78, 0x47);
    setRegister(0x84, 0x80);
    setRegister(0x85, 0x90);
    setRegister(0x86, 0x80);
    setRegister(0x87, 0x98);
    setRegister(0x8A, 0xF6);
    setRegister(0x8B, 0x80);
    setRegister(0x8C, 0x75);
    setRegister(0x8F, 0xF9);
    setRegister(0x90, 0x91);
    setRegister(0x93, 0x2D);
    setRegister(0x94, 0x23);
    setRegister(0x99, 0x77);
    setRegister(0x9A, 0x2D);
    setRegister(0x9B, 0x1E);
    setRegister(0x9C, 0x47);
    setRegister(0x9D, 0x3A);
    setRegister(0x9E, 0x03);
    setRegister(0x9F, 0x1E);
    setRegister(0xA0, 0x22);
    setRegister(0xA1, 0x33);
    setRegister(0xA2, 0x51);
    setRegister(0xA3, 0x36);
    setRegister(0xA4, 0x0C);
    setRegister(0xAE, 0x37);
    setRegister(0xB5, 0x9B);
    setRegister(0xBD, 0x45);
    setRegister(0xBE, 0x68);
    setRegister(0xBF, 0x5C);
    setRegister(0xC0, 0x33);
    setRegister(0xC1, 0xCA);
    setRegister(0xC2, 0x43);
    setRegister(0xC3, 0x90);
    setRegister(0xC4, 0xEC);
    setRegister(0xC5, 0x17);
    setRegister(0xC6, 0xDA);
    setRegister(0xC7, 0x41);
    setRegister(0xC8, 0x49);
    setRegister(0xC9, 0xBC);
    setRegister(0xCA, 0x3A);
    setRegister(0xCB, 0x20);
    setRegister(0xCC, 0x43);
    setRegister(0xCD, 0xA6);
    setRegister(0xCE, 0x4C);
    setRegister(0xCF, 0x1F);
    setRegister(0xD0, 0x8D);
    setRegister(0xD1, 0x3D);
    setRegister(0xD2, 0xDF);
    setRegister(0xD3, 0xA4);
    setRegister(0xD4, 0x30);
    setRegister(0xD5, 0x00);
    setRegister(0xD6, 0x41);
    setRegister(0xD7, 0x81);
    setRegister(0xD8, 0xD0);
    setRegister(0xD9, 0x00);
    setRegister(0xDA, 0x00);
    setRegister(0xDB, 0x41);
    setRegister(0xDC, 0x6D);
    setRegister(0xDD, 0xAC);
    setRegister(0xDE, 0x39);
    setRegister(0xDF, 0x4E);
    setRegister(0xE0, 0x43);
    setRegister(0xE1, 0xA0);
    setRegister(0xE2, 0x4C);
    setRegister(0xE3, 0x1D);
    setRegister(0xE4, 0x99);
    setRegister(0xE5, 0x3B);
    setRegister(0xE6, 0x00);
    setRegister(0xE7, 0x0D);
    setRegister(0xA5, 0x00);
    setRegister(0xE9, 0x41);
    setRegister(0xAE, 0x77);
    setRegister(0xE9, 0x51);

    /* Auto LNA from above definition */
    setRegister(0x37, 0x3B);
    setRegister(0x39, 0x2C);

    short checkVal = (short)(((readRegister(0x10) << 8) | readRegister(0x11)) & 0xFFFF);
    if(checkVal != (short)0xFFFF) {
        setRegister(0x2C, 0x48);
        setRegister(0x47, 0xE0);
        setRegister(0x35, 0x04);

        setRegister(0x2A, 0x05);
        setRegister(0x2D, 0x8C);
        setRegister(0x61, 0x25);
    } else {
        setRegister(0x2C, 0xC8);
        setRegister(0x47, 0xB0);
        setRegister(0x35, 0x04);

        setRegister(0x2A, 0x07);
        setRegister(0x2D, 0xEC);
        setRegister(0x61, 0x2A);
    }

    switchPage(REGISTER_PAGE_RF);
    setRegister(0x6B, 0xC5);
}

void RaonTunerInput::rtvRfSpecial() {
    std::vector<uint8_t> readReq = {0x10, 0x00, 0x00, 0x01};
    int bytesTransfered = m_usbDevice->writeBulkTransferData(RAON_ENDPOINT_OUT, readReq);

    std::vector<uint8_t> readReqBuf(4);
    bytesTransfered = m_usbDevice->readBulkTransferData(RAON_ENDPOINT_IN, readReqBuf);

    readReq = {0x00, 0x00, 0x00, 0x00};
    bytesTransfered = m_usbDevice->writeBulkTransferData(RAON_ENDPOINT_OUT, readReq);
    bytesTransfered = m_usbDevice->readBulkTransferData(RAON_ENDPOINT_IN, readReqBuf);

    readReq = {0x01, 0x00, 0x00, 0x00};
    bytesTransfered = m_usbDevice->writeBulkTransferData(RAON_ENDPOINT_OUT, readReq);
    bytesTransfered = m_usbDevice->readBulkTransferData(RAON_ENDPOINT_IN, readReqBuf);
}

void RaonTunerInput::setFrequency(uint32_t frequencyKhz) {

    int nNumTblEntry = 0;

    int freqMhz = frequencyKhz/1000;
    int nIdx = (freqMhz - DAB_CH_BAND3_START_FREQ_KHz) / DAB_CH_BAND3_STEP_FREQ_KHz;

    if(freqMhz >= 224096) {
        nIdx += 3;
    }  else if(freqMhz >= 217008) {
        nIdx += 2;
    } else if(freqMhz >= 210096) {
        nIdx += 1;
    }

    uint8_t adcClkFrqType = g_aeAdcClkTypeTbl_DAB_B3[nIdx];

    if(changedAdcClock(adcClkFrqType)) {
        switchPage(REGISTER_PAGE_RF);

        int pllNf = g_atPllNF_DAB_BAND3[nIdx];
        setRegister(0x23, ((pllNf >> 22) & 0xFF));
        setRegister(0x24, ((pllNf >> 14) & 0xFF));
        setRegister(0x25, ((pllNf >> 5) & 0xFF));
        setRegister(0x26, (((pllNf & 0x0000003F) << 2) | 1));

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        setRegister(0x20, 0x00);

        uint8_t verifyByte15 = readRegister(0x15);
        uint8_t verifyByte12 = readRegister(0x12);
        uint8_t verifyByte13 = readRegister(0x13);
        uint8_t verifyByte14 = readRegister(0x14);
        
        if( (verifyByte15 & 0x02) == 0x02) {

        } else {
            //setRegister(0x20, 0x00);
        }

        switchPage(REGISTER_PAGE_FM);
        setRegister(0x10, 0x48);
        setRegister(0x10, 0xC9);
    }
}

void RaonTunerInput::softReset() {
    std::cout << LOG_TAG << "SoftReset..." << std::endl;
    switchPage(REGISTER_PAGE_FM);

    setRegister(0x10, 0x48);
    //there should be a little wait here
    setRegister(0x10, 0xC9);
}

void RaonTunerInput::setupMemoryFIC() {
    std::cout << LOG_TAG << "Setting up FIC Memory..." << std::endl;

    switchPage(REGISTER_PAGE_DD);

    /* auto user clr, get fic  CRC 2byte including[4] */
    setRegister(0x46, 0x14);

    setRegister(0x46, 0x16);
}

void RaonTunerInput::setupMscThreshold() {
    std::cout << LOG_TAG << "Setting up MSC Threshold..." << std::endl;

    switchPage(REGISTER_PAGE_DD);

    //1536
    //setRegister(0x56, 0x00);
    //setRegister(0x57, 0xD8);

    //Threshold at 1024
    //setRegister(0x56, 0x04);
    //setRegister(0x57, 0x00);

    //Threshold at 2048
    setRegister(0x56, 0x08);
    setRegister(0x57, 0x00);

    switchPage(REGISTER_PAGE_DD);

}

void RaonTunerInput::clearAndSetupMscMemory() {
    switchPage(REGISTER_PAGE_DD);

    setRegister(0x48, 0x00);
    setRegister(0x48, 0x05);
}

void RaonTunerInput::openSubChannel(uint8_t subchanId) {
    std::cout << LOG_TAG << "Opening subchannel: " << std::hex << +subchanId << std::dec << std::endl;

    switchPage(REGISTER_PAGE_DD);

    setRegister(0x35, 0x70); /* MSC1 Interrupt status clear. */

    switchPage(REGISTER_PAGE_HOST);
    setRegister(0x62, 0x00);

    switchPage(REGISTER_PAGE_DD);
    setRegister(0x3A, static_cast<uint8_t>(0x80 | subchanId));
}

void RaonTunerInput::closeSubchannel(uint8_t subchanId) {
    std::cout << LOG_TAG << "Closing subchannel: " << std::hex << +subchanId << std::dec << std::endl;

    switchPage(REGISTER_PAGE_DD);

    setRegister(0x35, 0x70); /* MSC1 Interrupt status clear. */

    switchPage(REGISTER_PAGE_HOST);
    setRegister(0x62, 0x00);

    switchPage(REGISTER_PAGE_DD);
    setRegister(0x3A, subchanId);

    m_currentSubchanId = 0xFF;

    if(m_startServiceLink != nullptr) {
        m_startServiceLink->decodeAudio(false);
        if(m_startServiceLink->getJavaDabServiceObject() != nullptr) {
            m_usbDevice->serviceStopped(m_startServiceLink->getJavaDabServiceObject());
        }

        m_startServiceLink = nullptr;
    }
}

void RaonTunerInput::readFic() {
    uint8_t lockStatus = getLockStatus();

    if(lockStatus != RTV_DAB_CHANNEL_LOCK_OK) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if(lockStatus == RTV_DAB_OFDM_LOCK_MASK || lockStatus == RTV_DAB_FEC_LOCK_MASK) {
            m_maxCollectionWaitLoops += 8;
        }

        m_maxCollectionWaitLoops -= 10;
        if(m_maxCollectionWaitLoops <= 0) {
            m_scanCommandQueue.push(std::bind(&RaonTunerInput::scanNext, this));
        }

        std::cout << LOG_TAG << "ScanRetries: " << +m_maxCollectionWaitLoops << " LockStat: " << +lockStatus << std::endl;
        return;
    }

    switchPage(REGISTER_PAGE_FM);

    switchPage(REGISTER_PAGE_DD);
    uint8_t demodStat = readRegister(INT_E_STATL);
    bool ficInt = (demodStat & FIC_E_INT);

    if(ficInt) {
        switchPage(REGISTER_PAGE_FIC);

        std::vector<uint8_t> reqFic = {0x22, 0x00, 0x80, 0x01, 0x10};
        int bytesTransfered = m_usbDevice->writeBulkTransferData(RAON_ENDPOINT_OUT, reqFic);

        std::vector<uint8_t> reFicRet(400);
        bytesTransfered = m_usbDevice->readBulkTransferData(RAON_ENDPOINT_IN, reFicRet);

        switchPage(REGISTER_PAGE_DD);
        /* FIC interrupt status clear */
        setRegister(INT_E_UCLRL, 0x01);

        if(bytesTransfered >= 4) {
            for(int i = 0; i < ((bytesTransfered - 4) / 32); i++) {
                dataInput(std::vector<uint8_t>(reFicRet.begin()+4+i*32, reFicRet.begin()+4+i*32+32), 0x64, false);
            }
        }

        --m_ficCollectionWaitLoops;
        std::cout << LOG_TAG << "FicRetries: " << +m_ficCollectionWaitLoops << std::endl;
        if(m_ficCollectionWaitLoops <= 0) {
            m_scanCommandQueue.push(std::bind(&RaonTunerInput::scanNext, this));
        }
    }
}

void RaonTunerInput::readMsc() {
    switchPage(REGISTER_PAGE_DD);

    uint8_t int_type_val1 = readRegister(INT_E_STATL);
    bool ofdmLock = static_cast<bool>((int_type_val1 & 0x80) >> 7);
    bool msc1Overrun = (int_type_val1 & MSC1_E_OVER_FLOW) >> 6;
    bool msc1Underrun = (int_type_val1 & MSC1_E_UNDER_FLOW) >> 5;
    bool msc1Int = (int_type_val1 & MSC1_E_INT) >> 4;
    bool msc0Overrun = (int_type_val1 & MSC0_E_OVER_FLOW) >> 3;
    bool msc0Underrun = (int_type_val1 & MSC0_E_UNDER_FLOW) >> 2;
    bool msc0Int = (int_type_val1 & MSC0_E_INT) >> 1;
    bool ficInt = (int_type_val1 & FIC_E_INT);

    //std::cout << LOG_TAG << "STATL OFDM: " << std::boolalpha << ofdmLock << \
                            " MSC1_O: " << msc1Overrun << \
                            " MSC1_U: " << msc1Underrun << \
                            " MSC1_I: " << msc1Int << \
                            " MSC0_O: " << msc0Overrun << \
                            " MSC0_U: " << msc0Underrun << \
                            " MSC0_I: " << msc0Int << \
                            " FIC0_I: " << ficInt << \
                            std::noboolalpha << std::endl;

    uint8_t int_type_val2 = readRegister(INT_E_STATH);
    bool ofdmNis = static_cast<bool>((int_type_val2 & 0x80) >> 7);
    bool ofdmTii = static_cast<bool>((int_type_val2 & 0x40) >> 6);
    bool ofdmScan = static_cast<bool>((int_type_val2 & 0x20) >> 5);
    bool ofdmWindowPos = static_cast<bool>((int_type_val2 & 0x10) >> 4);
    bool ofdmUnlock = static_cast<bool>((int_type_val2 & 0x08) >> 3);
    bool fecReconfig = static_cast<bool>((int_type_val2 & 0x04) >> 2);
    bool fecCifEnd = static_cast<bool>((int_type_val2 & 0x02) >> 1);
    bool fecSoftreset = static_cast<bool>((int_type_val2 & 0x01));

    //std::cout << LOG_TAG << "STATH OFDMNis: " << std::boolalpha << ofdmNis << \
              " OFDMTii: " << ofdmTii << \
              " OFDMScan: " << ofdmScan << \
              " OFDMWinPos: " << ofdmWindowPos << \
              " OFDMUnlock: " << ofdmUnlock << \
              " FECReconfig: " << fecReconfig << \
              " FecCifEnd: " << fecCifEnd << \
              " FecSoftReset: " << fecSoftreset << \
              std::noboolalpha << std::endl;


    if(msc1Overrun || msc1Underrun) {
        std::cout << LOG_TAG << "Clearing MSC memory for OverUnderRun" << std::endl;

        //ClearAndSetupMSCMemory
        /*
        switchPage(REGISTER_PAGE_DD);
        setRegister(0x48, 0x00);
        setRegister(0x48, 0x05);
        */
        clearAndSetupMscMemory();

        setRegister(INT_E_UCLRL, 0x04);
        return;
    }

    if(msc1Int) {
        //std::cout << LOG_TAG << "Reading MSC memory" << std::endl;

        switchPage(REGISTER_PAGE_MSC1);

        std::vector<uint8_t> mscReqBuf = {0x22, 0x00, 0xD8, 0x00, 0x10};
        std::vector<uint8_t> mscRecBuff(1536);

        int bytesTransfered = m_usbDevice->writeBulkTransferData(RAON_ENDPOINT_OUT, mscReqBuf);
        bytesTransfered = m_usbDevice->readBulkTransferData(RAON_ENDPOINT_IN, mscRecBuff);

        if(m_startServiceLink != nullptr) {
            dataInput(std::vector<uint8_t>(mscRecBuff.begin()+4, mscRecBuff.begin()+bytesTransfered), m_currentSubchanId, false);
        } else {
            std::cout << LOG_TAG << "StartServiceLink is null" << std::endl;
        }

        //clear buffer
        switchPage(REGISTER_PAGE_DD);
        setRegister(INT_E_UCLRL, 0x04);
    }
}

void RaonTunerInput::startReadFicThread() {
    std::cout << LOG_TAG << "Starting FIC thread..." << std::endl;

    if(!m_readFicThreadRunning) {
        m_readFicThreadRunning = true;
        m_readFicThread = std::thread(&RaonTunerInput::threadedFicRead, this);
    }
}

void RaonTunerInput::stopReadFicThread() {
    std::cout << LOG_TAG << "Stopping FIC thread..." << std::endl;
    if(m_readFicThreadRunning) {
        m_readFicThreadRunning = false;
        if(m_readFicThread.joinable()) {
            std::cout << LOG_TAG << "Joining FIC thread..." << std::endl;
            m_readFicThread.join();
            std::cout << LOG_TAG << "Joining FIC thread done" << std::endl;
        }
    }
}

void RaonTunerInput::startReadDataThread() {
    std::cout << LOG_TAG << "Starting Data thread..." << std::endl;

    if(!m_commandThreadRunning) {
        m_commandThreadRunning = true;
        m_commandThread = std::thread(&RaonTunerInput::commandProcessing, this);
    }
}

void RaonTunerInput::stopReadDataThread() {
    std::cout << LOG_TAG << "Stopping Data thread..." << std::endl;
    if(m_commandThreadRunning) {
        m_commandThreadRunning = false;
        if(m_commandThread.joinable()) {
            std::cout << LOG_TAG << "Joining Data thread..." << std::endl;
            m_commandThread.join();
            std::cout << LOG_TAG << "Joining Data thread done" << std::endl;
        }
    }
}

void RaonTunerInput::readMscData() {
    switchPage(REGISTER_PAGE_MSC1);

    //std::vector<uint8_t> mscReqBuf = {0x22, 0x00, 0xD8, 0x00, 0x10};
    //std::vector<uint8_t> mscRecBuff(1536);
    //std::vector<uint8_t> mscReqBuf = {0x22, 0x00, 0xFF, 0x00, 0x10};

    //1024
    //std::vector<uint8_t> mscReqBuf = {0x22, 0x04, 0x00, 0x00, 0x10};

    //2048
    //std::vector<uint8_t> mscReqBuf = {0x22, 0x00, 0x00, 0x08, 0x10};
    std::vector<uint8_t> mscReqBuf = {0x22, 0x00, 0x00, 0x08, 0x10};

    std::vector<uint8_t> mscRecBuff(4096);

    int bytesTransfered = m_usbDevice->writeBulkTransferData(RAON_ENDPOINT_OUT, mscReqBuf);
    bytesTransfered = m_usbDevice->readBulkTransferData(RAON_ENDPOINT_IN, mscRecBuff);

    //std::cout << LOG_TAG << "ReadData: " << +bytesTransfered << std::endl;

    if(m_startServiceLink != nullptr && bytesTransfered > 0) {
        dataInput(std::vector<uint8_t>(mscRecBuff.begin()+4, mscRecBuff.begin()+bytesTransfered), m_currentSubchanId, false);
    } else {
        std::cout << LOG_TAG << "StartServiceLink is null" << std::endl;
    }

    //clear buffer
    switchPage(REGISTER_PAGE_DD);
    setRegister(INT_E_UCLRL, 0x04);
}

void RaonTunerInput::readFicData() {
    switchPage(REGISTER_PAGE_FIC);

    std::vector<uint8_t> reqFic = {0x22, 0x00, 0x80, 0x01, 0x10};
    int bytesTransfered = m_usbDevice->writeBulkTransferData(RAON_ENDPOINT_OUT, reqFic);

    std::vector<uint8_t> reFicRet(400);
    bytesTransfered = m_usbDevice->readBulkTransferData(RAON_ENDPOINT_IN, reFicRet);

    switchPage(REGISTER_PAGE_DD);
    /* FIC interrupt status clear */
    setRegister(INT_E_UCLRL, 0x01);

    //std::cout << LOG_TAG << "FicData Hdr: " << +reFicRet.size() << " : " << +bytesTransfered  << " : " << std::hex << std::setfill('0') << std::setw(2) << +reFicRet[0] << +reFicRet[1] << +reFicRet[2] << +reFicRet[3] << std::dec << std::endl;
    //std::cout << LOG_TAG << "FicData: " << std::hex << std::setfill('0') << std::setw(2) << +reFicRet[5] << +reFicRet[6] << +reFicRet[7] << +reFicRet[8] << std::dec << std::endl;

    for(int i = 0; i < ((bytesTransfered - 4) / 32); i++) {
        try {
            dataInput(std::vector<uint8_t>(reFicRet.begin() + 4 + i * 32, reFicRet.begin() + 4 + i * 32 + 32), 0x64, false);
        } catch(std::length_error& lenErr) {
            std::cout << LOG_TAG << "Length error..." << std::endl;
        }
    }
}

void RaonTunerInput::clearMscBuffer() {
    switchPage(REGISTER_PAGE_DD);
    setRegister(0x48, 0x00);
    setRegister(0x48, 0x05);

    setRegister(INT_E_UCLRL, 0x04);
}

void RaonTunerInput::readData() {
    --m_antLvlCnt;
    if(!m_antLvlCnt) {
        getAntennaLevel();
        m_antLvlCnt = 10;
    }

    switchPage(REGISTER_PAGE_DD);

    uint8_t int_type_val1 = readRegister(INT_E_STATL);
    bool ofdmLock = static_cast<bool>((int_type_val1 & 0x80u) >> 7u);
    bool msc1Overrun = (int_type_val1 & MSC1_E_OVER_FLOW) >> 6u;
    bool msc1Underrun = (int_type_val1 & MSC1_E_UNDER_FLOW) >> 5u;
    bool msc1Int = (int_type_val1 & MSC1_E_INT) >> 4u;
    bool msc0Overrun = (int_type_val1 & MSC0_E_OVER_FLOW) >> 3u;
    bool msc0Underrun = (int_type_val1 & MSC0_E_UNDER_FLOW) >> 2u;
    bool msc0Int = (int_type_val1 & MSC0_E_INT) >> 1u;
    bool ficInt = (int_type_val1 & FIC_E_INT);

    //TODO ensemble reconfiguration signalling
    //added to possibly see reconfigure
    uint8_t int_type_val2 = readRegister(INT_E_STATH);
    bool ofdmNis = static_cast<bool>((int_type_val2 & 0x80) >> 7);
    bool ofdmTii = static_cast<bool>((int_type_val2 & 0x40) >> 6);
    bool ofdmScan = static_cast<bool>((int_type_val2 & 0x20) >> 5);
    bool ofdmWindowPos = static_cast<bool>((int_type_val2 & 0x10) >> 4);
    bool ofdmUnlock = static_cast<bool>((int_type_val2 & 0x08) >> 3);
    bool fecReconfig = static_cast<bool>((int_type_val2 & 0x04) >> 2);
    bool fecCifEnd = static_cast<bool>((int_type_val2 & 0x02) >> 1);
    bool fecSoftreset = static_cast<bool>((int_type_val2 & 0x01));

    //std::cout << LOG_TAG << "STATH OFDMNis: " << std::boolalpha << ofdmNis << \
              " OFDMTii: " << ofdmTii << \
              " OFDMScan: " << ofdmScan << \
              " OFDMWinPos: " << ofdmWindowPos << \
              " OFDMUnlock: " << ofdmUnlock << \
              " FECReconfig: " << fecReconfig << \
              " FecCifEnd: " << fecCifEnd << \
              " FecSoftReset: " << fecSoftreset << \
              std::noboolalpha << std::endl;

    if(ficInt) {
        switchPage(REGISTER_PAGE_FIC);

        std::vector<uint8_t> reqFic = {0x22, 0x00, 0x80, 0x01, 0x10};
        int bytesTransfered = m_usbDevice->writeBulkTransferData(RAON_ENDPOINT_OUT, reqFic);

        std::vector<uint8_t> reFicRet(400);
        bytesTransfered = m_usbDevice->readBulkTransferData(RAON_ENDPOINT_IN, reFicRet);

        switchPage(REGISTER_PAGE_DD);
        /* FIC interrupt status clear */
        setRegister(INT_E_UCLRL, 0x01);

        //std::cout << LOG_TAG << "FicData Hdr: " << +reFicRet.size() << " : " << +bytesTransfered  << " : " << std::hex << std::setfill('0') << std::setw(2) << +reFicRet[0] << +reFicRet[1] << +reFicRet[2] << +reFicRet[3] << std::dec << std::endl;
        //std::cout << LOG_TAG << "FicData: " << std::hex << std::setfill('0') << std::setw(2) << +reFicRet[5] << +reFicRet[6] << +reFicRet[7] << +reFicRet[8] << std::dec << std::endl;

        for(int i = 0; i < ((bytesTransfered - 4) / 32); i++) {
            try {
                dataInput(std::vector<uint8_t>(reFicRet.begin() + 4 + i * 32, reFicRet.begin() + 4 + i * 32 + 32), 0x64, false);
            } catch(std::length_error& lenErr) {
                std::cout << LOG_TAG << "Length error..." << std::endl;
            }
        }
    }

    if(msc1Overrun) {
        std::cout << LOG_TAG << "MSC memory for OverRun" << std::endl;
    }

    if(msc1Underrun) {
        std::cout << LOG_TAG << "MSC memory for UnderRun" << std::endl;
    }

    if(msc1Overrun || msc1Underrun) {
        std::cout << LOG_TAG << "Clearing MSC memory for OverUnderRun, MSC0Int" << +msc0Int << ", MSC0Overrun: " << +msc0Overrun << ", MSC0Underrun: " << +msc0Underrun << std::endl;

        //ClearAndSetupMSCMemory
        switchPage(REGISTER_PAGE_DD);
        setRegister(0x48, 0x00);
        setRegister(0x48, 0x05);

        setRegister(INT_E_UCLRL, 0x04);
        return;
    }

    if(msc1Int) {
        //std::cout << LOG_TAG << "Reading MSC memory" << std::endl;

        switchPage(REGISTER_PAGE_MSC1);

        //std::vector<uint8_t> mscReqBuf = {0x22, 0x00, 0xD8, 0x00, 0x10};
        //std::vector<uint8_t> mscRecBuff(1536);
        //std::vector<uint8_t> mscReqBuf = {0x22, 0x00, 0xFF, 0x00, 0x10};

        //1024
        //std::vector<uint8_t> mscReqBuf = {0x22, 0x04, 0x00, 0x00, 0x10};

        //2048
        //std::vector<uint8_t> mscReqBuf = {0x22, 0x00, 0x00, 0x08, 0x10};
        std::vector<uint8_t> mscReqBuf = {0x22, 0x00, 0x00, 0x08, 0x10};

        std::vector<uint8_t> mscRecBuff(4096);

        int bytesTransfered = m_usbDevice->writeBulkTransferData(RAON_ENDPOINT_OUT, mscReqBuf);
        bytesTransfered = m_usbDevice->readBulkTransferData(RAON_ENDPOINT_IN, mscRecBuff);

        //std::cout << LOG_TAG << "ReadData: " << +bytesTransfered << std::endl;

        if(m_startServiceLink != nullptr && bytesTransfered >= 4) {
            dataInput(std::vector<uint8_t>(mscRecBuff.begin()+4, mscRecBuff.begin()+bytesTransfered), m_currentSubchanId, false);
        } else {
            std::cout << LOG_TAG << "StartServiceLink is null" << std::endl;
        }

        //clear buffer
        switchPage(REGISTER_PAGE_DD);
        setRegister(INT_E_UCLRL, 0x04);

        return;
    } else {
        //std::cout << LOG_TAG << "MSC1 not ready ####" << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

uint8_t RaonTunerInput::getLockStatus() {
    switchPage(REGISTER_PAGE_DD);

    uint8_t lockState = readRegister(0x37);
    uint8_t lockSt{0};

    if(lockState & 0x01) {
        lockSt = RTV_DAB_OFDM_LOCK_MASK;
    }

    lockState = readRegister(0xFB);
    if((lockState & 0x03) == 0x03) {
        lockSt |= RTV_DAB_FEC_LOCK_MASK;
    }

    std::cout << LOG_TAG << "LockState at: " << +m_currentFrequency << " : " << +lockSt << std::endl;

    return lockSt;
}

void RaonTunerInput::getAntennaLevel() {
    bool lock_stat{false};
    uint8_t rcnt3{0}, rcnt2{0}, rcnt1{0}, rcnt0{0};
    uint32_t cer_cnt, cer_period_cnt, ret_val;
    uint8_t fec_sync;

    switchPage(REGISTER_PAGE_DD);

    lock_stat = (readRegister(0x37) & 0x01) != 0;
    if(lock_stat) {
        // MSC CER period counter for accumulation
        rcnt3 = readRegister(0x88);
        rcnt2 = readRegister(0x89);
        rcnt1 = readRegister(0x8A);
        rcnt0 = readRegister(0x8B);
        cer_period_cnt = (rcnt3 << 24) | (rcnt2 << 16) | (rcnt1 << 8) | rcnt0; // 442368

        rcnt3 = readRegister(0x8C);
        rcnt2 = readRegister(0x8D);
        rcnt1 = readRegister(0x8E);
        rcnt0 = readRegister(0x8F);
    } else {
        cer_period_cnt = 0;
        m_usbDevice.get()->receptionStatistics(false, 0);
        return;
    }

    fec_sync = (uint8_t)((readRegister(0xD7) >> 4) & 0x1);

    if(cer_period_cnt != 0) {
        cer_cnt = (rcnt3 << 24) | (rcnt2 << 16) | (rcnt1 << 8) | rcnt0;
        if(cer_cnt <= 4000) {
            ret_val = 0;
        } else {
            ret_val = ((cer_cnt * 1000)/cer_period_cnt) * 10;
            if(ret_val > 1200) {
                ret_val = 2000;
            }
        }
    } else {
        ret_val = 2000;
    }

    if ((fec_sync == 0) || (ret_val == 2000)) {
        setRegister(0x03, 0x09);
        setRegister(0x46, 0x1E);
        setRegister(0x35, 0x01);
        setRegister(0x46, 0x16);
    }

    //AntennaLvl
    uint_t curLevel = 0;
    uint_t prevLevel = m_prevAntennaLvl;

    do {
        if(ret_val >= AntLvlTbl[curLevel]) { /* Use equal for CER 0 */
            break;
        }
    } while(++curLevel != DAB_MAX_NUM_ANTENNA_LEVEL);

    if (curLevel != prevLevel) {
        if (curLevel < prevLevel) {
            prevLevel--;
        } else {
            prevLevel++;
        }

        m_prevAntennaLvl = static_cast<uint8_t>(prevLevel);
    }

    m_usbDevice.get()->receptionStatistics(true, prevLevel);
}

//TUNER METHODS END