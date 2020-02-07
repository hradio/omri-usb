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

#ifndef DABUSERAPPLICATION_H
#define DABUSERAPPLICATION_H

#include <vector>
#include <memory>

#include "registered_tables.h"
#include "dabuserapplicationdecoder.h"

class DabUserApplication {

public:
    explicit DabUserApplication();
    virtual ~DabUserApplication();

    virtual registeredtables::USERAPPLICATIONTYPE getUserApplicationType() const;
    virtual bool isCaApplied() const;
    virtual bool isXpadApp() const;
    virtual uint8_t getXpadAppType() const;
    virtual bool dataGroupsUsed() const;
    virtual registeredtables::DATA_SERVICE_COMPONENT_TYPE getDataServiceComponentType() const;
    virtual uint16_t getCaOrganization() const;
    virtual const std::vector<uint8_t>& getUserApplicationData() const;
    virtual std::shared_ptr<DabUserapplicationDecoder> getUserApplicationDecoder() const;

    virtual void setUserApplicationType(registeredtables::USERAPPLICATIONTYPE uAppType);
    virtual void setIsCaApplied(bool caApplied);
    virtual void setIsXpadApp(bool isXpadApp);
    virtual void setXpadAppType(uint8_t xpadAppType);
    virtual void setIsDataGroupsUsed(bool dgUsed);
    virtual void setDataServiceComonentType(registeredtables::DATA_SERVICE_COMPONENT_TYPE dscty);
    virtual void setCaOrganization(uint16_t caOrg);
    virtual void setUserApplicationData(const std::vector<uint8_t>& uAppData);

    virtual void userApplicationDataInput(const std::vector<uint8_t>& uAppData, uint8_t dataType);

private:
    const std::string m_logTag{"[DabUserApplication]"};

    registeredtables::USERAPPLICATIONTYPE m_userAppType{registeredtables::USERAPPLICATIONTYPE::RFU};
    bool m_caApplied{false};
    bool m_isXpadApp{false};
    uint8_t m_xpadAppType{0xFF};
    bool m_dataGroupsUsed{false};
    registeredtables::DATA_SERVICE_COMPONENT_TYPE m_dataServiceComponentType{registeredtables::DATA_SERVICE_COMPONENT_TYPE::UNSPECIFIED_DATA};
    uint16_t m_caOrganization{0x0000};
    std::vector<uint8_t> m_userAppData{0x00};

    std::shared_ptr<DabUserapplicationDecoder> m_userAppDecoder{nullptr};

public:
    inline bool operator==(const DabUserApplication& other) const {
        return  other.m_caApplied == m_caApplied && \
                other.m_xpadAppType == m_xpadAppType && \
                other.m_isXpadApp == m_isXpadApp && \
                other.m_userAppType == m_userAppType && \
                other.m_dataGroupsUsed == m_dataGroupsUsed && \
                other.m_dataServiceComponentType == m_dataServiceComponentType && \
                other.m_caOrganization == m_caOrganization && \
                other.m_userAppData == m_userAppData;
    }
    inline bool operator!=(const DabUserApplication& other) const { return !operator==(other); }

};

#endif // DABUSERAPPLICATION_H
