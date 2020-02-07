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

#ifndef DABSERVICECOMPONENTDECODER
#define DABSERVICECOMPONENTDECODER

#include <cstdint>
#include <vector>
#include <memory>
#include <functional>

#include "callbackhandle.h"

class DabServiceComponentDecoder {

public:
    explicit DabServiceComponentDecoder();
    virtual ~DabServiceComponentDecoder();

    virtual void setSubchannelBitrate(uint16_t bitrate);
    virtual void componentDataInput(const std::vector<uint8_t>& frameData, bool synchronized) = 0;
    virtual void flushBufferedData();

    using Component_Data_Callback = std::function<void(const std::vector<uint8_t>&)>;
    std::shared_ptr<DabServiceComponentDecoder::Component_Data_Callback> registerComponentDataCallback(DabServiceComponentDecoder::Component_Data_Callback cb);

protected:
    CallbackDispatcher<DabServiceComponentDecoder::Component_Data_Callback> m_componentDataDispatcher;
    uint16_t m_subChanBitrate{0x00};
    int m_frameSize{0x00};
};

#endif // DABSERVICECOMPONENTDECODER

