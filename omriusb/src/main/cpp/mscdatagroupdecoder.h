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

#ifndef MSCDATAGROUPDECODER_H
#define MSCDATAGROUPDECODER_H

#include <cstdint>
#include <vector>

class MscDatagroupDecoder {

public:
    explicit MscDatagroupDecoder();
    virtual ~MscDatagroupDecoder();

    void mscDatagroupInput(const std::vector<uint8_t>& mscData);
};

#endif // MSCDATAGROUPDECODER_H
