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

#ifndef SLIDESHOWDECODER_H
#define SLIDESHOWDECODER_H

#include "dabmotuserapplicationdecoder.h"
#include "callbackhandle.h"

class SlideshowDecoder : public DabMotUserapplicationDecoder {

public:
    explicit SlideshowDecoder();
    virtual ~SlideshowDecoder();

    virtual registeredtables::USERAPPLICATIONTYPE getUserApplicationType() const override;
    virtual void applicationDataInput(const std::vector<uint8_t>& appData, uint8_t dataType) override;
    virtual void motApplicationDataInput(const MOT_Data& motData) override;

    virtual std::shared_ptr<UserapplicationDataCallback> registerUserapplicationDataCallback(UserapplicationDataCallback appSpecificDataCallback) override;

    virtual void reset() override;

private:
    std::string m_logTag{"[SlideshowDecoder]"};

    CallbackDispatcher<UserapplicationDataCallback> m_userappDataDispatcher;

private:
    const std::string IMAGE_MIMETYPE[4] {
            "image/gif",
            "image/jpeg",
            "image/bmp",
            "image/png"
    };
};


#endif //SLIDESHOWDECODER_H
