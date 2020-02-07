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

#ifndef RAONTUNERINPUT_H
#define RAONTUNERINPUT_H

#include "dabusbtunerinput.h"
#include "jtunerusbdevice.h"
#include "jdabservice.h"
#include "../../concurrent_queue.h"

#include <memory>
#include <array>

class RaonTunerInput final : public DabUsbTunerInput, DabEnsemble {

public:
    explicit RaonTunerInput(std::shared_ptr<JTunerUsbDevice> usbDevice);
    virtual ~RaonTunerInput();

    //delete copy and assignment constructors
    RaonTunerInput(RaonTunerInput&) = delete;
    void operator=(RaonTunerInput&) = delete;

    void initialize() override;
    bool isInitialized() const override;
    int getCurrentTunedFrequency() const override;
    void tuneFrequency(int frequencyKHz) override;
    const DabEnsemble& getEnsemble() const override;
    int getMaximumConcurrentSubChannels() const override;

    void addMscCallback(CallbackFunction cb, uint8_t subchanId) override;
    void addFicCallback(CallbackFunction cb) override;

    void startService(std::shared_ptr<JDabService> serviceLink) override;
    void stopService(const DabService& service) override;
    void stopAllRunningServices() override;

    void startServiceScan() override;
    void stopServiceScan() override;

    std::string getDeviceName() const override;

private:
    const std::string LOG_TAG{"[RaonUsbTuner] "};
    constexpr static uint8_t MAXIMUM_CONCURRENT_SUBCHANNELS{1};
    constexpr static uint8_t RAON_ENDPOINT_OUT = 0x02;
    constexpr static uint8_t RAON_ENDPOINT_IN = 0x82;

    uint32_t m_currentFrequency{0};

    std::shared_ptr<JTunerUsbDevice> m_usbDevice{nullptr};
    bool m_isInitialized{false};

    bool m_isScanning{false};

    std::shared_ptr<JDabService> m_startServiceLink{nullptr};
    std::shared_ptr<std::function<void()>> m_ensembleFinishedCb;

    std::atomic<bool> m_readFicThreadRunning{false};
    std::thread m_readFicThread;

    std::atomic<bool> m_readMscThreadRunning{false};
    std::thread m_readMscThread;

    std::atomic<bool> m_readDataThreadRunning{false};
    std::thread m_readDataThread;

    uint8_t m_currentSubchanId{0xFF};

    int m_currentScanningEnsembleNum{0};
    //TODO changed from 100
    constexpr static int MAX_COLLECTION_LOOPS = 200;
    int m_maxCollectionWaitLoops{MAX_COLLECTION_LOOPS}; //5 seconds
    int m_ficCollectionWaitLoops{300};

    ConcurrentQueue<std::function<void(void)>> m_scanCommandQueue;
    std::atomic<bool> m_scanCommandThreadRunning{false};
    std::thread m_scanCommandThread;

    ConcurrentQueue<std::function<void(void)>> m_commandQueue;
    std::atomic<bool> m_commandThreadRunning{false};
    std::thread m_commandThread;

    //int m_antLvlCnt{100};
    int m_antLvlCnt{10};
    uint8_t m_prevAntennaLvl{0};

private:
    void commandProcessing();

    void initializeSync();
    void tuneFrequencySync(int frequencyKHz);
    void startServiceSync(std::shared_ptr<JDabService> serviceLink);

private:
    void ensembleCollectFinished();

    void setService();

    void threadedFicRead();
    void threadedMscRead();

    void threadedDataRead();

    void scanNext();

    void processScanCommands();
    void startScanCommand();
    void stopScanCommand();

private:
    enum REGISTER_PAGE {
        REGISTER_PAGE_OFDM = 0x02,  //For 1seg
        REGISTER_PAGE_FEC =  0x03,  //For 1seg
        REGISTER_PAGE_COMM = 0x04,
        REGISTER_PAGE_FM =   0x06,  //T-DMB OFDM/FM
        REGISTER_PAGE_HOST = 0x07,
        REGISTER_PAGE_CAS =  0x08,
        REGISTER_PAGE_DD =   0x09,	//FEC for T-DMB, DAB, FM

        REGISTER_PAGE_FIC =  0x0A,
        REGISTER_PAGE_MSC0 = 0x0B,
        REGISTER_PAGE_MSC1 = 0x0C,
        REGISTER_PAGE_RF =   0x0F
    };

    constexpr static uint8_t g_abAdcClkSynTbl[4][7] = {
            {0x08, 0x01, 0x13, 0x25, 0x06, 0x7D, 0xB4},	// Based 24.576MHz,	8MHz	   External Clock21
            {0x06, 0x01, 0x0F, 0x27, 0x07, 0x60, 0xB8},	// Based 24.576MHz,	8.192MHz   External Clock20
            {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},	// Based 24.576MHz, 9MHz       Unsupported Clock
            {0x0C, 0x01, 0x1F, 0x27, 0x06, 0xE1, 0xB8}	// Based 24.576MHz,	9.6MHz	   External Clock22
    };

    static constexpr uint8_t RTV_ADC_CLK_FREQ_8_MHz     = 0;
    static constexpr uint8_t RTV_ADC_CLK_FREQ_8_192_MHz = 1;
    static constexpr uint8_t RTV_ADC_CLK_FREQ_9_MHz     = 2;
    static constexpr uint8_t RTV_ADC_CLK_FREQ_9_6_MHz   = 3;

    static constexpr uint8_t g_aeAdcClkTypeTbl_DAB_B3[] {
            RTV_ADC_CLK_FREQ_8_192_MHz, //5A : 174928,
            RTV_ADC_CLK_FREQ_9_6_MHz, 	//5B : 176640,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//5C : 178352,
            RTV_ADC_CLK_FREQ_9_6_MHz,	//5D : 180064,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//6A : 181936,
            RTV_ADC_CLK_FREQ_9_6_MHz,	//6B : 183648,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//6C : 185360,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//6D : 187072,
            RTV_ADC_CLK_FREQ_8_MHz,		//7A : 188928,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//7B : 190640,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//7C : 192352,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//7D : 194064,
            RTV_ADC_CLK_FREQ_8_MHz,		//8A : 195936,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//8B : 197648,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//8C : 199360,
            RTV_ADC_CLK_FREQ_8_MHz,	 	//8D : 201072,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//9A : 202928,
            RTV_ADC_CLK_FREQ_8_MHz,		//9B : 204640,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//9C : 206352,
            RTV_ADC_CLK_FREQ_9_6_MHz,	//9D : 208064,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//10A: 209936,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//10N: 210096,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//10B: 211648,
            RTV_ADC_CLK_FREQ_8_MHz,		//10C: 213360,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//10D: 215072,
            RTV_ADC_CLK_FREQ_9_6_MHz,	//11A: 216928,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//11N: 217008,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//11B: 218640,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//11C: 220352,
            RTV_ADC_CLK_FREQ_8_MHz,		//11D: 222064,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//12A: 223936,
            RTV_ADC_CLK_FREQ_8_MHz,		//12N: 224096,
            RTV_ADC_CLK_FREQ_8_MHz,		//12B: 225648,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//12C: 227360,
            RTV_ADC_CLK_FREQ_8_MHz,		//12D: 229072,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//13A: 230784,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//13B: 232496,
            RTV_ADC_CLK_FREQ_8_MHz,		//13C: 234208,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//13D: 235776,
            RTV_ADC_CLK_FREQ_8_MHz, 	//13E: 237488,
            RTV_ADC_CLK_FREQ_8_192_MHz,	//13F: 239200
    };

    constexpr static int DAB_CH_BAND3_START_FREQ_KHz    = 174928;
    constexpr static int DAB_CH_BAND3_STEP_FREQ_KHz     = 1712;

    static constexpr int g_atPllNF_DAB_BAND3[] {
            0x38F3D55/*5A : 174928*/,
            0x3982800/*5B : 176640*/,
            0x3A112AA/*5C : 178352*/,
            0x3A9FD55/*5D : 180064*/,
            0x3B3BD55/*6A : 181936*/,
            0x3BCA800/*6B : 183648*/,
            0x3C592AA/*6C : 185360*/,
            0x3CE7D55/*6D : 187072*/,
            0x3D82800/*7A : 188928*/,
            0x3E112AA/*7B : 190640*/,
            0x3E9FD55/*7C : 192352*/,
            0x3F2E800/*7D : 194064*/,
            0x3FCA800/*8A : 195936*/,
            0x40592AA/*8B : 197648*/,
            0x40E7D55/*8C : 199360*/,
            0x4176800/*8D : 201072*/,
            0x42112AA/*9A : 202928*/,
            0x429FD55/*9B : 204640*/,
            0x432E800/*9C : 206352*/,
            0x43BD2AA/*9D : 208064*/,
            0x44592AA/*10A: 209936*/,
            0x4466800/*10N: 210096*/,
            0x44E7D55/*10B: 211648*/,
            0x4576800/*10C: 213360*/,
            0x46052AA/*10D: 215072*/,
            0x469FD55/*11A: 216928*/,
            0x46AD2AA/*11N: 217088*/,
            0x472E800/*11B: 218640*/,
            0x47BD2AA/*11C: 220352*/,
            0x484BD55/*11D: 222064*/,
            0x48E7D55/*12A: 223936*/,
            0x48F52AA/*12N: 224096*/,
            0x4976800/*12B: 225648*/,
            0x4A052AA/*12C: 227360*/,
            0x4A93D55/*12D: 229072*/,
            0x4B22800/*13A: 230784*/,
            0x4BB12AA/*13B: 232496*/,
            0x4C3FD55/*13C: 234208*/,
            0x4CC2800/*13D: 235776*/,
            0x4D512AA/*13E: 237488*/,
            0x4DDFD55/*13F: 239200*/
    };

    static constexpr uint8_t MSC1_E_OVER_FLOW  = 0x40;
    static constexpr uint8_t MSC1_E_UNDER_FLOW = 0x20;
    static constexpr uint8_t MSC1_E_INT        = 0x10;
    static constexpr uint8_t MSC0_E_OVER_FLOW  = 0x08;
    static constexpr uint8_t MSC0_E_UNDER_FLOW = 0x04;
    static constexpr uint8_t MSC0_E_INT        = 0x02;
    static constexpr uint8_t FIC_E_INT         = 0x01;
    static constexpr uint8_t RE_CONFIG_E_INT   = 0x04;

    static constexpr uint8_t MSC1_INTR_BITS = (MSC1_E_INT|MSC1_E_UNDER_FLOW|MSC1_E_OVER_FLOW);
    static constexpr uint8_t MSC0_INTR_BITS	= (MSC0_E_INT|MSC0_E_UNDER_FLOW|MSC0_E_OVER_FLOW);

    static constexpr uint8_t INT_E_UCLRL        = 0x35;  /// [2]MSC1 int clear [1]MSC0 int clear [0]FIC int clear
    static constexpr uint8_t INT_E_UCLRH        = 0x36;  /// [6]OFDM TII done clear

    static constexpr uint8_t INT_E_STATL        = 0x33;  /// [7]OFDM Lock status [6]MSC1 overrun [5]MSC1 underrun [4]MSC1 int [3]MSC0 overrun [2]MSC0 underrun [1]MSC0 int [0]FIC int
    static constexpr uint8_t INT_E_STATH        = 0x34;  /// [7]OFDM NIS [6]OFDM TII [5]OFDM scan [4]OFDM window position [3]OFDM unlock [2]FEC re-configuration [1]FEC CIF end [0]FEC soft reset

    static constexpr uint8_t RTV_DAB_OFDM_LOCK_MASK     = 0x01;
    static constexpr uint8_t RTV_DAB_FEC_LOCK_MASK      = 0x02;
    static constexpr uint8_t RTV_DAB_CHANNEL_LOCK_OK    = (RTV_DAB_OFDM_LOCK_MASK|RTV_DAB_FEC_LOCK_MASK);

    static constexpr uint8_t DAB_MAX_NUM_ANTENNA_LEVEL  = 0x07;
    static constexpr uint_t  AntLvlTbl[DAB_MAX_NUM_ANTENNA_LEVEL] {
            810,
            700,
            490,
            400,
            250,
            180,
            0
    };

    //TunerSpecific
    void setRegister(uint8_t reg, uint8_t val);
    uint8_t readRegister(uint8_t reg);
    void switchPage(REGISTER_PAGE regPage);

    bool tunerPowerUp();
    void configurePowerType();
    void configureAddClock();
    bool changedAdcClock(uint8_t g_aeAdcClkTypeTbl_DAB_B3);

    void tdmbInitTop();
    void tdmbInitComm();
    void tdmbInitHost();
    void tdmbInitOfdm();
    void tdmbInitFEC();
    void rtvResetMemoryFIC();
    void rtvOEMConfigureInterrupt();
    void rtvStreamDisable();
    void rtvConfigureHostIF();
    void rtvRFInitilize();
    void rtvRfSpecial();

    void setFrequency(uint32_t frequencyKhz);

    void softReset();
    void setupMemoryFIC();
    void setupMscThreshold();
    void clearAndSetupMscMemory();

    void openSubChannel(uint8_t subchanId);
    void closeSubchannel(uint8_t subchanId);

    void readFic();
    void readMsc();

    void readData();

    /* ** */
    void readMscData();
    void readFicData();
    void clearMscBuffer();
    /* ** */

    void startReadFicThread();
    void stopReadFicThread();

    void startReadDataThread();
    void stopReadDataThread();

    uint8_t getLockStatus();

    void getAntennaLevel();
};


#endif //RAONTUNERINPUT_H
