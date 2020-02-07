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

#ifndef DABINPUT
#define DABINPUT

#include <functional>
#include <vector>

#include "dabensemble.h"

/*
 * Abstract base class each input has to derive from
 */
class DabInput {

public:
    explicit DabInput() = default;
    DabInput(DabInput&&) = default;
    DabInput& operator=(DabInput&&) = default;
    virtual ~DabInput() = default;

    virtual void initialize() = 0;
    virtual bool isInitialized() const = 0;
    virtual int getCurrentTunedFrequency() const = 0;
    virtual const DabEnsemble& getEnsemble() const = 0;
    virtual int getMaximumConcurrentSubChannels() const = 0;

    virtual void tuneFrequency(int frequencyKHz) = 0;

public:
    using CallbackFunction = std::function<void(const std::vector<uint8_t>&)>;
    virtual void addMscCallback(CallbackFunction cb, uint8_t subchanId) = 0;
    virtual void addFicCallback(CallbackFunction cb) = 0;
};

#endif // DABINPUT

