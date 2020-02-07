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

#include <iostream>
#include <sstream>
#include <iomanip>

#include "fig_00_ext_21.h"

Fig_00_Ext_21::Fig_00_Ext_21(const std::vector<uint8_t>& figData) : Fig_00(figData) {
    parseFigData(figData);
}

Fig_00_Ext_21::~Fig_00_Ext_21() {

}

void Fig_00_Ext_21::parseFigData(const std::vector<uint8_t>& figData) {
    auto figIter = figData.cbegin() +1;
    while(figIter < figData.cend()) {
        uint16_t rfa = static_cast<uint16_t>(((*figIter++ & 0xFF) << 3) | (((*figIter & 0xE0) >> 5) & 0xFF));
        uint8_t lenFiList = static_cast<uint8_t>((*figIter++ & 0x1F) & 0xFF);

        std::vector<uint8_t> fiListData(figData.cbegin() + std::distance(figData.cbegin(), figIter), figData.cbegin() + std::distance(figData.cbegin(), figIter) + lenFiList);

        figIter += lenFiList;

        auto fiListIter = fiListData.cbegin();
        while(fiListIter < fiListData.cend()) {
            uint16_t idField = static_cast<uint16_t>(((*fiListIter++ & 0xFF) << 8) | (*fiListIter++ & 0xFF));

            auto rangeModulation =  static_cast<uint8_t>((((*fiListIter & 0xF0) >> 4) & 0xFF));
            bool contFlag = (((*fiListIter & 0x08) >> 3) & 0xFF) != 0;
            uint8_t frqListEntryLen = static_cast<uint8_t>(((*fiListIter++ & 0x07) & 0xFF));

            std::vector<uint8_t> freqListData(fiListData.cbegin() + std::distance(fiListData.cbegin(), fiListIter), fiListData.cbegin() + std::distance(fiListData.cbegin(), fiListIter) + frqListEntryLen);

            fiListIter += frqListEntryLen;

            FrequencyInformation freqInfo;
            freqInfo.continuousOutput = contFlag;
            freqInfo.isOtherEnsemble = isOtherEnsemble();

            if(frqListEntryLen == 0) {
                freqInfo.isChangeEvent = true;
            }

            auto freqListIter = freqListData.cbegin();
            while(freqListIter < freqListData.cend()) {
                //R&M = 0000: DAB Ensemble
                if(rangeModulation == 0) {
                    /*
                     * Control field: this 5-bit field shall be used to qualify the immediately following Freq (Frequency) a field.
                     * The following functions are defined (the remainder shall be reserved for future use of the Freq a field):
                     *  b23 - b19;
                     *  0 0 0 0 0: geographically adjacent area, no transmission mode signalled;
                     *  0 0 0 1 0: geographically adjacent area, transmission mode I;
                     *  0 0 0 0 1: not geographically adjacent area, no transmission mode signalled;
                     *  0 0 0 1 1: not geographically adjacent area, transmission mode I.
                     */
                    uint8_t controlField = static_cast<uint8_t>((((*freqListIter & 0xF8) >> 3) & 0xFF));

                    switch (controlField) {
                        case 0:
                            freqInfo.adjacent = DabEnsembleAdjacent::GEOGRAPHICALLY_ADJACENT_TRANSMISSION_MODE_NOT_SIGNALLED;
                        case 1:
                            freqInfo.adjacent = DabEnsembleAdjacent::GEOGRAPHICALLY_NOT_ADJACENT_TRANSMISSION_MODE_NOT_SIGNALLED;
                        case 2:
                            freqInfo.adjacent = DabEnsembleAdjacent::GEOGRAPHICALLY_ADJACENT_TRANSMISSION_MODE_ONE;
                        case 3:
                            freqInfo.adjacent = DabEnsembleAdjacent::GEOGRAPHICALLY_NOT_ADJACENT_TRANSMISSION_MODE_ONE;
                    };
                    /*
                     * Freq (Frequency) a: this 19-bit field, coded as an unsigned binary number, shall represent the
                     * carrier frequency associated with the alternative service source or other service.
                     *
                     * The centre carrier frequency of the other ensemble is given by (in this expression, the decimal equivalent of freq a is used):
                     *  0 Hz + (Freq a × 16 kHz).
                     *
                     * The following values of the carrier frequency are defined:
                     *  b18                                  b0    Decimal
                     *   0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0       0       : Not to be used;
                     *   0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1       1       : 16 kHz;
                     *   0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0       2       : 32 kHz;
                     *  ..............................................................
                     *   1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1    524 287    : 8 388 592 kHz.
                     */
                    uint32_t frequency = static_cast<uint32_t>(((*freqListIter++ & 0x07) << 16) | ((*freqListIter++ & 0xFF) << 8) | ((*freqListIter++ & 0xFF)));

                    freqInfo.frequencyInformationType = FrequencyInformationType::DAB_ENSEMBLE;
                    freqInfo.frequenciesKHz.push_back(frequency*16);
                    freqInfo.id = idField;
                }

                //R&M = 1000: FM with RDS
                if(rangeModulation == 8) {
                    /*
                     * Freq (Frequency) b: this 8-bit field, coded as an unsigned binary number, shall represent the carrier frequency associated with the other service:
                     *
                     * The carrier frequency of the FM transmission is given by (in this expression, the decimal equivalent of freq b is used):
                     *  87,5 MHz + (Freq b × 100 kHz).
                     *
                     * The following values of the carrier frequency are defined (other values shall be reserved for future use):
                     *
                     * b7           b0   Decimal
                     * 0 0 0 0 0 0 0 0      0       : Not to be used;
                     * 0 0 0 0 0 0 0 1      1       : 87,6 MHz;
                     * 0 0 0 0 0 0 0 1      1       : 87,6 MHz;
                     * ...............................................
                     * 1 1 0 0 1 1 0 0     204      : 107,9 MHz.
                     */
                    uint8_t frequency = static_cast<uint8_t>((*freqListIter++ & 0xFF));
                    std::cout << m_logTag << " FM Frequency " << +frequency << " : " << +(frequency*100 + 87500) << " kHz" << std::endl;

                    freqInfo.frequencyInformationType = FrequencyInformationType::FM_RDS;
                    freqInfo.frequenciesKHz.push_back(frequency*100 + 87500);
                }

                //R&M = 0110: DRM
                //R&M = 1110: AMSS
                if(rangeModulation == 14 || rangeModulation == 6) {
                    /*
                     * Id field 2: this 8-bit field represents the AMSS Service Identifier (most significant byte) (see ETSI TS 102 386)
                     */
                    /*
                     *  Id field 2: this 8-bit field represents the DRM Service Identifier (most significant byte) (see ETSIES 201 980).
                     */
                    uint8_t idField2 = static_cast<uint8_t>(*freqListIter++ & 0xFF);

                    /*
                     *  Freq (Frequency) c: this 16-bit field, consists of the following fields:
                     */

                    /*
                     * Rfu: this 1 bit field shall be reserved for future use of the frequency field and shall be set to zero until defined.
                     */
                    bool rfu = (((*freqListIter & 0x80) >> 7) & 0xFF) != 0;

                    /*
                     * frequency: this 15-bit field, coded as an unsigned binary number, shall represent the centre frequency associated with the other service in kHz
                     *
                     * The following values of the reference frequency are defined:
                     *
                     * b14                        b0  Decimal
                     * 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0     0      : Not to be used;
                     * 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1     1      : 1 kHz;
                     * 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1     1      : 1 kHz;
                     * ........................................................
                     * 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1   32 767   : 32 767 kHz.
                     */
                    bool multiplier = *freqListIter >> 7U != 0;
                    uint16_t frequency = static_cast<uint8_t>(((*freqListIter++ & 0x7F) << 8) | ((*freqListIter++ & 0xFF)));
                    freqInfo.frequenciesKHz.push_back(multiplier ? frequency*10 : frequency);
                    if(rangeModulation == 6) {
                        freqInfo.frequencyInformationType = FrequencyInformationType::DRM;
                    } else {
                        freqInfo.frequencyInformationType = FrequencyInformationType::AMSS;
                    }

                    freqInfo.id = idField2 << 8 | idField;
                }

                //sorting list for easier compare
                std::sort(freqInfo.frequenciesKHz.begin(), freqInfo.frequenciesKHz.end());
                m_frequencyInformations.push_back(freqInfo);
            }
        }
    }
}
