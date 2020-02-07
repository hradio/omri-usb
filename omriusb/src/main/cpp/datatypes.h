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

#ifndef DATATYPES_H
#define DATATYPES_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>

#include "global_definitions.h"
#include "dynamiclabeldecoder.h"

struct MOT_Data {
    uint16_t transportId;
    MOT_CONTENT_TYPE motContentType;
    uint16_t motContentSubtype;
    std::map<uint8_t, std::vector<uint8_t>> motHeaderParams;
    std::vector<uint8_t> motBodyData;
    uint32_t motBodySize;
    int previousSegmentNumber{-1};
};

struct DabSlideshow {
    DabSlideshow() {};
    bool isCategorized{false};
    std::string contentName{""};
    long triggerTime{0};
    long expireTime{0};
    uint8_t categoryId{0};
    uint8_t slideId{0};
    std::string categoryTitle{""};
    std::string clickThroughUrl{""};
    std::string alternativeLocationUrl{""};
    std::string mimeType{""};
    uint16_t contentSubType{0};
    std::vector<uint8_t> slideshowData;
};

struct DabDynamicLabelPlusTag {
    DynamiclabelDecoder::DL_PLUS_CONTENT_TYPE contentType{DynamiclabelDecoder::DL_PLUS_CONTENT_TYPE::DUMMY};
    std::string dlPlusTagText;
};

struct DabDynamicLabel {
    std::string dynamicLabel;
    uint8_t charset;
    bool itemToggle{false};
    bool itemRunning{false};
    std::vector<DabDynamicLabelPlusTag> dlPlusTags;
};

#endif //DATATYPES_H
