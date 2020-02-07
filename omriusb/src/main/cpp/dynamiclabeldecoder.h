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

#ifndef DYNAMICLABELDECODER_H
#define DYNAMICLABELDECODER_H

#include "dabuserapplicationdecoder.h"
#include "callbackhandle.h"

class DynamiclabelDecoder : public DabUserapplicationDecoder {

public:
    explicit DynamiclabelDecoder();
    virtual ~DynamiclabelDecoder();

    virtual registeredtables::USERAPPLICATIONTYPE getUserApplicationType() const override;
    virtual void applicationDataInput(const std::vector<uint8_t>& appData, uint8_t dataType) override;
    virtual std::shared_ptr<UserapplicationDataCallback> registerUserapplicationDataCallback(UserapplicationDataCallback appSpecificDataCallback) override;

    virtual void reset() override;

public:
    enum DL_PLUS_CONTENT_TYPE {
        DUMMY,
        ITEM_TITLE,
        ITEM_ALBUM,
        ITEM_TRACKNUMBER,
        ITEM_ARTIST,
        ITEM_COMPOSITION,
        ITEM_MOVEMENT,
        ITEM_CONDUCTOR,
        ITEM_COMPOSER,
        ITEM_BAND,
        ITEM_COMMENT,
        ITEM_GENRE,
        INFO_NEWS,
        INFO_NEWS_LOCAL,
        INFO_STOCKMARKET,
        INFO_SPORT,
        INFO_LOTTERY,
        INFO_HOROSCOPE,
        INFO_DAILY_DIVERSION,
        INFO_HEALTH,
        INFO_EVENT,
        INFO_SCENE,
        INFO_CINEMA,
        INFO_TV,
        INFO_DATE_TIME,
        INFO_WEATHER,
        INFO_TRAFFIC,
        INFO_ALARM,
        INFO_ADVERTISEMENT,
        INFO_URL,
        INFO_OTHER,
        STATIONNAME_SHORT,
        STATIONNAME_LONG,
        PROGRAMME_NOW,
        PROGRAMME_NEXT,
        PROGRAMME_PART,
        PROGRAMME_HOST,
        PROGRAMME_EDITORIAL_STAFF,
        PROGRAMME_FREQUENCY,
        PROGRAMME_HOMEPAGE,
        PROGRAMME_SUBCHANNEL,
        PHONE_HOTLINE,
        PHONE_STUDIO,
        PHONE_OTHER,
        SMS_STUDIO,
        SMS_OTHER,
        EMAIL_HOTLINE,
        EMAIL_STUDIO,
        EMAIL_OTHER,
        MMS_OTHER,
        CHAT,
        CHAT_CENTER,
        VOTE_QUESTION,
        VOTE_CENTRE,

        RFU_1,
        RFU_2,

        PRIVATE_CLASS_1,
        PRIVATE_CLASS_2,
        PRIVATE_CLASS_3,

        DESCRIPTOR_PLACE = 59,
        DESCRIPTOR_APPOINTMENT,
        DESCRIPTOR_IDENTIFIER,
        DESCRIPTOR_PURCHASE,
        DESCRIPTOR_GET_DATA
    };

    const static std::string DL_PLUS_CONTENT_TYPE_STRING[];

private:
    std::string m_logTag{"[DynamiclabelDecoder]"};

    std::vector<uint8_t> m_dlsData;
    uint8_t m_dlsFullSegNum{0};
    std::vector<uint8_t> m_dlsFullData;
    uint8_t m_currentDlsCharset{0xFF};

    bool m_isDynamicPlus{false};

    CallbackDispatcher<UserapplicationDataCallback> m_userappDataDispatcher;

private:
    enum SEGMENT_TYPE {
        INTERMEDIATE,
        LAST,
        FIRST,
        ONE_AND_ONLY
    };

    enum DLS_COMMAND {
        COMMAND_UNKNOWN,
        CLEAR_DISPLAY,
        DL_PLUS_COMMAND
    };
};

#endif // DYNAMICLABELDECODER_H
