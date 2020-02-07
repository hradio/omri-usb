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

#ifndef DABSERVICECOMPONENTMSCSTREAMAUDIO_H
#define DABSERVICECOMPONENTMSCSTREAMAUDIO_H

#include "dabservicecomponentmscstream.h"
#include "dabplusservicecomponentdecoder.h"
#include "paddecoder.h"

#include "slideshowdecoder.h"

class DabServiceComponentMscStreamAudio : public DabServiceComponentMscStream {

public:
    explicit DabServiceComponentMscStreamAudio();
    virtual ~DabServiceComponentMscStreamAudio();

    virtual uint8_t getAudioServiceComponentType() const;

    virtual void setAudioServiceComponentType(uint8_t ascty);
    virtual void setSubchannelBitrate(uint16_t bitrateKbits) override;
    virtual void setSubchannelId(uint8_t subChanId) override;

    virtual void addUserApplication(const DabUserApplication& uApp) override;
    virtual void componentMscDataInput(const std::vector<uint8_t>& mscData, bool synchronized) override;
    virtual void flushBufferedData() override;

    using AUDIO_DATA_CALLBACK = std::function<void (const std::vector<uint8_t>&, int, int, int, bool, bool)>;
    virtual std::shared_ptr<DabServiceComponentMscStreamAudio::AUDIO_DATA_CALLBACK> registerAudioDataCallback(AUDIO_DATA_CALLBACK cb);

private:
    std::string m_logTag = "[DabServiceComponentMscStreamAudio]";

    uint8_t m_ascTy;

    //ComponentDecoder base pointer
    std::shared_ptr<DabServiceComponentDecoder> m_componentDecoder;

    //pointer for audiodata callback
    std::shared_ptr<DabServiceComponentDecoder::Component_Data_Callback> m_audioCallback;
    std::shared_ptr<DabPlusServiceComponentDecoder::PAD_DATA_CALLBACK> m_padCallback;

    PadDecoder m_padDecoder;

    CallbackDispatcher<AUDIO_DATA_CALLBACK> m_audioDataDispatcher;

private:
    std::shared_ptr<DabUserapplicationDecoder::UserapplicationDataCallback> m_slideCallPtr;
};

#endif // DABSERVICECOMPONENTMSCSTREAMAUDIO_H
