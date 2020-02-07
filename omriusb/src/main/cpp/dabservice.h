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

#ifndef DABSERVICE_H
#define DABSERVICE_H

#include <memory>
#include <vector>

#include "dabservicecomponent.h"

class DabService {

public:
    explicit DabService();
    virtual ~DabService();

    virtual uint32_t getServiceId() const;
    virtual bool isCaApplied() const;
    virtual uint8_t getCaId() const;
    virtual uint8_t getNumberServiceComponents() const;
    virtual bool isProgrammeService() const;

    virtual uint8_t getLabelCharset() const;
    virtual std::string getServiceLabel() const;
    virtual std::string getServiceShortLabel() const;

    virtual const std::vector<std::shared_ptr<DabServiceComponent>>& getServiceComponents() const;

    virtual uint8_t getProgrammeTypeCode() const;
    virtual bool isProgrammeTypeDynamic() const;
    virtual std::string getProgrammeTypeFullName() const;
    virtual std::string getProgrammeType16charName() const;
    virtual std::string getProgrammeType8CharName() const;

    virtual uint32_t getEnsembleFrequency() const;

    virtual void setServiceId(uint32_t serviceId);
    virtual void setIsProgrammeService(bool isProgramme);
    virtual void setCaId(uint8_t caId);
    virtual void setNumberOfServiceComponents(uint8_t numSc);

    virtual void setLabelCharset(uint8_t charset);
    virtual void setServiceLabel(const std::string& label);
    virtual void setServiceShortLabel(const std::string& shortLabel);

    virtual void addServiceComponent(std::shared_ptr<DabServiceComponent> component);

    virtual void setProgrammeTypeCode(uint8_t intPtyCode);
    virtual void setProgrammeTypeIsDynamic(bool dynamic);
    virtual void setEnsembleFrequency(uint32_t ensembleFrequency);

protected:
    const std::string m_logTag = "[DabService]";

    uint32_t m_serviceId{0xFFFFFFFF};
    bool m_isProgrammeService{false};
    uint8_t m_caId{0x00};
    bool m_caApplied{false};
    uint8_t m_numSrvComps{0};

    uint8_t m_labelCharset{0x00};
    std::string m_serviceLabel{""};
    std::string m_serviceShortLabel{""};

    std::vector<std::shared_ptr<DabServiceComponent>> m_components;

    uint8_t m_ptyCode{0x00};
    bool m_ptyIsDynamic{false};
    std::string m_ptyNameFull{"No program type"};
    std::string m_ptyName16{"None"};
    std::string m_ptyName8{"None"};

    uint32_t m_ensembleFrequency{0};
};

#endif // DABSERVICE_H
