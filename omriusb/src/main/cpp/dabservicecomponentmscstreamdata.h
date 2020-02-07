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

#ifndef DABSERVICECOMPONENTMSCSTREAMDATA_H
#define DABSERVICECOMPONENTMSCSTREAMDATA_H

#include "dabservicecomponentmscstream.h"

class DabServiceComponentMscStreamData : public DabServiceComponentMscStream {

public:
    explicit DabServiceComponentMscStreamData();
    virtual ~DabServiceComponentMscStreamData();

    virtual uint8_t getDataServiceComponentType() const;

    virtual void setDataServiceComponentType(uint8_t dscty);

    virtual void componentMscDataInput(const std::vector<uint8_t>& mscData, bool synchronized) override;
    virtual void flushBufferedData() override;

private:
    std::string m_logTag = "[DabServiceComponentMscStreamData]";

    uint8_t m_dscTy;
};

#endif // DABSERVICECOMPONENTMSCSTREAMDATA_H
