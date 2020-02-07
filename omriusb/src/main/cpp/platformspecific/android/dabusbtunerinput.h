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

#ifndef DABUSBTUNERINPUT_H
#define DABUSBTUNERINPUT_H

#include "../../dabinput.h"
#include "jdabservice.h"

#include <memory>
#include <string>

class DabUsbTunerInput : public DabInput {

public:
    virtual void startService(std::shared_ptr<JDabService> serviceLink) = 0;
    virtual void stopService(const DabService& service) = 0;

    virtual void startServiceScan() = 0;
    virtual void stopServiceScan() = 0;
    virtual void stopAllRunningServices() = 0;

    virtual std::string getDeviceName() const = 0;
};
#endif //DABUSBTUNERINPUT_H
