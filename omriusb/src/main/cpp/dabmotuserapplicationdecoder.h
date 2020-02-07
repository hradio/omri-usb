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

#ifndef DABMOTUSERAPPLICATIONDECODER_H
#define DABMOTUSERAPPLICATIONDECODER_H

#include "dabuserapplicationdecoder.h"
#include "datatypes.h"

class DabMotUserapplicationDecoder : public DabUserapplicationDecoder {

public:
    explicit DabMotUserapplicationDecoder();
    virtual ~DabMotUserapplicationDecoder();

    virtual void motApplicationDataInput(const MOT_Data& motData) = 0;
};


#endif //DABMOTUSERAPPLICATIONDECODER_H
