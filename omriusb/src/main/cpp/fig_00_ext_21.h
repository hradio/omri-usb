/*
 * Copyright (C) 2018 IRT GmbH
 *
 * Author:
 * Fabian Sattler
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

#ifndef FIG_00_EXT_21_H
#define FIG_00_EXT_21_H

#include "fig_00.h"

/*
 * ETS 300 401 clause 8.1.8 Frequency Information
 *
 * The Frequency Information feature is assigned to providing radio Frequency Information (FI).
 * The feature is encoded in Extension 21 of FIG type 0 (FIG 0/21).
 *
 * When the FI applies to the tuned ensemble, or to a Primary service component from the tuned ensemble carried via FM
 * with RDS, AM with AMSS or DRM, the OE flag shall be set to "0".
 *
 * When the FI applies to other ensembles, or to services not in the tuned ensemble carried via FM with RDS, AM with
 * AMSS or DRM, the OE flag shall be set to "1".
 *
 * This feature shall use the SIV signalling (see clause 5.2.2.1). The database shall be divided by use of a database key.
 * Changes to the database shall be signalled using the CEI.
 *
 * Frequency information forms part of the signalling used for service following and OE announcements, which is described in detail in ETSI TS 103 176.
 *
 * The database key comprises the OE and P/D flags (see clause 5.2.2.1) and the Rfa, Id field, and R&M fields.
 * The Change Event Indication (CEI) is signalled by the Length of Freq list field = 0.
 *
 * The repetition rates for FIG 0/21 are provided in ETSI TS 103 176.
 */
class Fig_00_Ext_21 : public Fig_00 {

public:
    enum FrequencyInformationType {
        FREQUENCY_INFORMATIONTYPE_UNKNOWN,
        DAB_ENSEMBLE,
        DRM,
        FM_RDS,
        AMSS
    };

    enum DabEnsembleAdjacent {
        GEOGRAPHICALLY_ADJACENT_UNKNOWN,
        GEOGRAPHICALLY_ADJACENT_TRANSMISSION_MODE_NOT_SIGNALLED,
        GEOGRAPHICALLY_ADJACENT_TRANSMISSION_MODE_ONE,
        GEOGRAPHICALLY_NOT_ADJACENT_TRANSMISSION_MODE_NOT_SIGNALLED,
        GEOGRAPHICALLY_NOT_ADJACENT_TRANSMISSION_MODE_ONE
    };

    struct FrequencyInformation {
        std::vector<uint32_t> frequenciesKHz{0x00};
        uint16_t id{0x00};
        FrequencyInformationType frequencyInformationType{FrequencyInformationType::FREQUENCY_INFORMATIONTYPE_UNKNOWN};
        DabEnsembleAdjacent adjacent{DabEnsembleAdjacent::GEOGRAPHICALLY_ADJACENT_UNKNOWN};

        /*
         * If R&M = "0000" the continuity flag shall signal that:
         *  0: continuous output not expected;
         *  1: continuous output possible.
         *
         * If R&M = "0110", "1000" or "1110" the continuity flag shall indicate whether, or not, there is an appropriate time delay
         * on the audio signal of an alternative service source on DRM/FM/AM to compensate the decoding time delay of DAB:
         *  0: no compensating time delay on DRM/FM/AM audio signal;
         *  1: compensating time delay on DRM/FM/AM audio signal.
         *
         * For R&M = "0110", "1000" and "1110", the Continuity flag is only valid for OE = "0". For OE = "1", the
         * bit b3 is reserved for future addition.
         */
        bool continuousOutput{false};

        bool isOtherEnsemble{false};

        /* The Change Event Indication (CEI) is signalled by the Length of Freq list field = 0 */
        bool isChangeEvent{false};

        /* The database key comprises the OE and P/D flags (see clause 5.2.2.1) and the Rfa, Id field, and R&M field */
        uint32_t freqDbKey{0x00};

        inline bool operator==(const FrequencyInformation& other) const {
            return other.id == id &&
                   other.isOtherEnsemble == isOtherEnsemble &&
                   other.continuousOutput == continuousOutput &&
                   other.frequencyInformationType == frequencyInformationType &&
                   other.adjacent == adjacent &&
                   containsAllFreqs(other.frequenciesKHz);
        }

        inline bool operator!=(const FrequencyInformation& other) const { return !operator==(other); }

        inline bool containsAllFreqs(const std::vector<uint32_t>& otherFreqList) const {
            return std::includes(frequenciesKHz.begin(), frequenciesKHz.end(), otherFreqList.begin(), otherFreqList.end());
        }
    };

public:
    explicit Fig_00_Ext_21(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_21();

    const std::vector<Fig_00_Ext_21::FrequencyInformation>& getFrequencyInformations() const { return m_frequencyInformations; };

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_21]"};

    std::vector<FrequencyInformation> m_frequencyInformations;
};

#endif // FIG_00_EXT_21_H
