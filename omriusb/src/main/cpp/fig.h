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

#ifndef FIG
#define FIG

class Fig {

public:
    virtual ~Fig() {}

protected:
    Fig() {}

public:
    enum FIG_00_TYPE : uint8_t {
        //FIG 00 Extension 00 - Ensemble information
        ENSEMBLE_INFORMATION,
        //FIG 00 Extension 01 - Basic sub-channel organization
        BASIC_SUBCHANNEL_ORGANIZATION,
        //FIG 00 Extension 02 - Basic service and service component definition
        BASIC_SERVICE_COMPONENT_DEFINITION,
        //FIG 00 Extension 03 - Service component in packet mode with or without Conditional Access
        SERVICE_COMPONENT_PACKET_MODE,
        //FIG 00 Extension 04 - Service component with Conditional Access in stream mode
        SERVICE_COMPONENT_STREAM_CA,
        //FIG 00 Extension 05 - Service component language
        SERVICE_COMPONENT_LANGUAGE,
        //FIG 00 Extension 06 - Service linking information
        SERVICE_LINKING_INFORMATION,
        //FIG 00 Extension 07 - Configuration information
        CONFIGURATION_INFORMATION,
        //FIG 00 Extension 08 - Service component global definition
        SERVICE_COMPONENT_GLOBAL_DEFINITION,
        //FIG 00 Extension 09 - Country, LTO and International table
        COUNTRY_LTO_INTERNATIONAL_TABLE,
        //FIG 00 Extension 10 - Date and time (d&t)
        DATE_AND_TIME,
        //FIG 00 Extension 13 - User application information
        USERAPPLICATION_INFORMATION = 13,
        //FIG 00 Extension 14 - FEC sub-channel organization
        FEC_SUBCHANNEL_ORGANIZATION,
        //FIG 00 Extension 17 - Programme Type
        PROGRAMME_TYPE = 17,
        //FIG 00 Extension 18 - Announcement support
        ANNOUNCEMENT_SUPPORT,
        //FIG 00 Extension 19 - Announcement switching
        ANNOUNCEMENT_SWITCHING,
        //FIG 00 Extension 20 - Service Component Information
        SERVICE_COMPONENT_INFORMATION,
        //FIG 00 Extension 21 - Frequency Information
        FREQUENCY_INFORMATION,
        //FIG 00 Extension 24 - OE Services
        OE_SERVICES = 24,
        //FIG 00 Extension 25 - OE Announcement support
        OE_ANNOUNCEMENT_SUPPORT,
        //FIG 00 Extension 26 - OE Announcement switching
        OE_ANNOUNCEMENT_SWITCHING
    };

    enum FIG_01_TYPE : uint8_t {
        //FIG 01/02 Extension 00 - Ensemble label
        ENSEMBLE_LABEL,
        //FIG 01/02 Extension 01 - Programme service label
        PROGRAMME_SERVICE_LABEL,
        //FIG 01/02 Extension 04 - Service component label
        SERVICE_COMPONENT_LABEL = 4,
        //FIG 01/02 Extension 05 - Data service label
        DATA_SERVICE_LABEL,
        //FIG 01/02 Extension 06 - X-PAD user application label
        XPAD_USERAPPLICATION_LABEL
    };
};

#endif // FIG

