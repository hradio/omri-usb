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

#ifndef MOTDECODER_H
#define MOTDECODER_H

#include <vector>
#include <cstdint>
#include <memory>
#include <map>

#include "concurrent_queue.h"
#include "dabmotuserapplicationdecoder.h"
#include "datatypes.h"

class MotDecoder {

public:
    explicit MotDecoder();
    virtual ~MotDecoder();

    virtual void motDataInput(const std::vector<uint8_t>& mscDatagroup);
    virtual void addUserapplicationDecoder(std::shared_ptr<DabMotUserapplicationDecoder> uAppDecoder);

private:
    std::string m_logTag = "[MotDecoder] ";
    std::vector<std::shared_ptr<DabMotUserapplicationDecoder>> m_uAppDecoders;
    ConcurrentQueue <std::pair<std::vector<uint8_t>, PAD_APPLICATION_TYPE>> m_conQueue;

private:
    std::map<uint16_t, MOT_Data> m_motHdrDataMap;

};


#endif //MOTDECODER_H
