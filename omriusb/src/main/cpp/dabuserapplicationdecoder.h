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

#ifndef DABUSERAPPLICATIONDECODER_H
#define DABUSERAPPLICATIONDECODER_H

#include <vector>
#include <memory>
#include <functional>

#include "registered_tables.h"

class DabUserapplicationDecoder {

public:
    explicit DabUserapplicationDecoder();
    virtual ~DabUserapplicationDecoder();

    virtual registeredtables::USERAPPLICATIONTYPE getUserApplicationType() const = 0;
    virtual void applicationDataInput(const std::vector<uint8_t>& appData, uint8_t dataType) = 0;

    using UserapplicationDataCallback = std::function<void(std::shared_ptr<void>)>;
    virtual std::shared_ptr<UserapplicationDataCallback> registerUserapplicationDataCallback(UserapplicationDataCallback appSpecificDataCallback) = 0;

    virtual void reset() = 0;
};

#endif // DABUSERAPPLICATIONDECODER_H
