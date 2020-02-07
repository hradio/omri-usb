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

#include <iostream>
#include "jdabservice.h"

//constexpr char const * JDabService::EBU_SET[16][16];

static constexpr uint16_t MIME_LINK_TABLE[4] = {
        7,
        1,
        4,
        2
};

JDabService::JDabService(JavaVM* javaVm, JNIEnv* env, jclass dabserviceClass, jclass dynamicLabelClass, jclass dynamicLabelPlusItemClass, jclass slideshowClass, jobject dabserviceObject) {
    std::cout << m_logTag << "Constructing" << std::endl;

    m_javaVm = javaVm;

    m_linkedJavaDabServiceObject = env->NewGlobalRef(dabserviceObject);

    //Global Refs from JNI_OnLoad
    m_javaDabServiceClass = dabserviceClass;
    m_javaDlsClass = dynamicLabelClass;
    m_javaDlPlusItemClass = dynamicLabelPlusItemClass;
    m_javaSlsClass = slideshowClass;

    m_javaDabSrvGetEnsembleFrequencyMId = env->GetMethodID(m_javaDabServiceClass, "getEnsembleFrequency", "()I");
    m_javaDabSrvGetEnsembleEccMId = env->GetMethodID(m_javaDabServiceClass, "getEnsembleEcc", "()I");
    m_javaDabSrvGetEnsembleIdMId = env->GetMethodID(m_javaDabServiceClass, "getEnsembleId", "()I");
    m_javaDabSrvGetServiceIdMId = env->GetMethodID(m_javaDabServiceClass, "getServiceId", "()I");

    //Audio data callback
    m_javaDabSrvAudioDataCallbackMId = env->GetMethodID(m_javaDabServiceClass, "audioData", "([BII)V");
    m_javaDabSrvAudioformatChangedCallbackMId = env->GetMethodID(m_javaDabServiceClass, "audioFormatChanged", "(IIIZZ)V");

    //DLS
    m_javaDlsConstructorMId = env->GetMethodID(m_javaDlsClass, "<init>", "()V");
    m_javaDlsSetFullTextMId = env->GetMethodID(m_javaDlsClass, "setText", "(Ljava/lang/String;)V");
    m_javaDlsSetFulltextBytesMId = env->GetMethodID(m_javaDlsClass, "setTextBytes", "([BI)V");
    m_javaDlsSetItemRunningMId = env->GetMethodID(m_javaDlsClass, "setItemRunning", "(Z)V");
    m_javaDlsSetItemToggledMId = env->GetMethodID(m_javaDlsClass, "setItemToggled", "(Z)V");
    m_javaDlsAddTagItemMId = env->GetMethodID(m_javaDlsClass, "addDlPlusItem", "(Lorg/omri/radioservice/metadata/TextualDabDynamicLabelPlusItem;)V");
    //DLS Callback
    m_javaDabSrvdynamicLabelReceivedCallbackMId = env->GetMethodID(m_javaDabServiceClass, "labeReceived", "(Lorg/omri/radioservice/metadata/Textual;)V");

    //DLPlusItem
    m_javaDlPlusItemConstructorMId = env->GetMethodID(m_javaDlPlusItemClass, "<init>", "()V");
    m_javaDlPlusItemSetContentTypeMId = env->GetMethodID(m_javaDlPlusItemClass, "setDlPlusContentType", "(I)V");
    m_javaDlPlusItemSetTextMId = env->GetMethodID(m_javaDlPlusItemClass, "setDlPlusContentText", "([BI)V");

    //SLS
    m_javaSlsConstructorMId = env->GetMethodID(m_javaSlsClass, "<init>", "()V");
    m_javaSlsSetContentNameMId = env->GetMethodID(m_javaSlsClass, "setContentName", "(Ljava/lang/String;)V");
    m_javaSlsSetVisualDataMId = env->GetMethodID(m_javaSlsClass, "setVisualData", "([B)V");
    m_javaSlsSetMimeTypeMId = env->GetMethodID(m_javaSlsClass, "setVisualMimeType", "(I)V");
    m_javaSlsSetContentTypeMId = env->GetMethodID(m_javaSlsClass, "setContentType", "(I)V");
    m_javaSlsSetContentSubTypeMId = env->GetMethodID(m_javaSlsClass, "setContentSubType", "(I)V");
    m_javaSlsSetAltLocUrlMId = env->GetMethodID(m_javaSlsClass, "setAlternativeLocationURL", "(Ljava/lang/String;)V");
    m_javaSlsSetSlideIdMId = env->GetMethodID(m_javaSlsClass, "setSlideId", "(I)V");
    m_javaSlsSetCategoryTextMId = env->GetMethodID(m_javaSlsClass, "setCategoryText", "(Ljava/lang/String;)V");
    m_javaSlsSetCategoryIdMId = env->GetMethodID(m_javaSlsClass, "setCategoryId", "(I)V");
    m_javaSlsSetClickThroughUrlMId = env->GetMethodID(m_javaSlsClass, "setCategoryClickThroughLink", "(Ljava/lang/String;)V");
    //SLS Callback
    m_javaDabSrvslideshowReceivedCallbackMId = env->GetMethodID(m_javaDabServiceClass, "slideshowReceived", "(Lorg/omri/radioservice/metadata/VisualDabSlideShow;)V");

    m_ensembleFrequency = static_cast<uint32_t >(env->CallIntMethod(m_linkedJavaDabServiceObject, m_javaDabSrvGetEnsembleFrequencyMId));
    m_ensembleEcc = static_cast<uint8_t >(env->CallIntMethod(m_linkedJavaDabServiceObject, m_javaDabSrvGetEnsembleEccMId));
    m_ensembleId = static_cast<uint16_t >(env->CallIntMethod(m_linkedJavaDabServiceObject, m_javaDabSrvGetEnsembleIdMId));
    m_serviceId = static_cast<uint32_t >(env->CallIntMethod(m_linkedJavaDabServiceObject, m_javaDabSrvGetServiceIdMId));
}

JDabService::~JDabService() {
    std::cout << m_logTag << "Destroying" << std::endl;

    bool wasDetached = false;
    JNIEnv* enve;

    int envState = m_javaVm->GetEnv((void**)&enve, JNI_VERSION_1_6);
    if(envState == JNI_EDETACHED) {
        if(m_javaVm->AttachCurrentThread(&enve, NULL) == 0) {
            wasDetached = true;
        } else {
            std::cout << m_logTag << "jniEnv thread failed to attach!" << std::endl;
            return;
        }
    }

    if(!enve->IsSameObject(m_linkedJavaDabServiceObject, NULL)) {
        enve->DeleteGlobalRef(m_linkedJavaDabServiceObject);
        m_linkedJavaDabServiceObject = nullptr;
    }

    if(wasDetached) {
        m_javaVm->DetachCurrentThread();
    }

    m_audioDataCb = nullptr;
}

void JDabService::setLinkDabService(std::shared_ptr<DabService> linkedDabSrv) {
    std::cout << m_logTag << "Linking DABServices..." << std::endl;
    m_linkedDabService = linkedDabSrv;

    if(linkedDabSrv->isProgrammeService()) {
        for(const auto& srvComp : linkedDabSrv->getServiceComponents()) {
            switch(srvComp->getServiceComponentType()) {
                case DabServiceComponent::SERVICECOMPONENTTYPE::MSC_STREAM_AUDIO: {
                    std::cout << m_logTag << "Registering audiocallback" << std::endl;
                    std::shared_ptr<DabServiceComponentMscStreamAudio> audioComponent = std::static_pointer_cast<DabServiceComponentMscStreamAudio>(srvComp);
                    m_audioDataCb = audioComponent->registerAudioDataCallback(std::bind(&JDabService::audioDataInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));

                    for(const auto& uApp : srvComp->getUserApplications()) {
                        std::cout << m_logTag << "Registering UserApplication Type: " << +uApp.getUserApplicationType() << ", DSCTy: " << +uApp.getDataServiceComponentType() << std::endl;
                        switch(uApp.getUserApplicationType()) {
                            case registeredtables::USERAPPLICATIONTYPE::DYNAMIC_LABEL: {
                                m_dlsCallback = uApp.getUserApplicationDecoder()->registerUserapplicationDataCallback(std::bind(&JDabService::dynamicLabelInput, this, std::placeholders::_1));
                                break;
                            }
                            case registeredtables::USERAPPLICATIONTYPE::MOT_SLIDESHOW: {
                                m_slsCallback = uApp.getUserApplicationDecoder()->registerUserapplicationDataCallback(std::bind(&JDabService::slideshowInput, this, std::placeholders::_1));
                                break;
                            }
                            //Other UserApp decoders not yet implemented
                            default:
                                break;
                        }
                    }
                    break;
                }

                default: {
                    break;
                }
            }
        }
    }
}

std::shared_ptr<DabService> JDabService::getLinkDabService() const {
    return m_linkedDabService;
}

jobject JDabService::getJavaDabServiceObject() const {
    std::cout << m_logTag << "Returning linked Java DabService object" << std::endl;
    return m_linkedJavaDabServiceObject;
}

void JDabService::linkServices() {
    std::cout << m_logTag << "Linking Services" << std::endl;
}

uint32_t JDabService::getEnsembleFrequency() const {
    return m_ensembleFrequency;
}

uint16_t JDabService::getEnsembleId() const {
    return m_ensembleId;
}

uint8_t JDabService::getEnsembleEcc() const {
    return m_ensembleEcc;
}

uint32_t JDabService::getServiceId() const {
    return m_serviceId;
}

void JDabService::audioDataInput(const std::vector<uint8_t>& audioData, int ascty, int channels, int sampleRate, bool sbrUsed, bool psUsed) {
    if(!m_decodeAudio) {
        return;
    }

    bool wasDetached = false;
    JNIEnv* enve;

    int envState = m_javaVm->GetEnv((void**)&enve, JNI_VERSION_1_6);
    if(envState == JNI_EDETACHED) {
        if(m_javaVm->AttachCurrentThread(&enve, nullptr) == 0) {
            wasDetached = true;
        } else {
            std::cout << m_logTag << "jniEnv thread failed to attach!" << std::endl;
            return;
        }
    }

    if(m_ascty != ascty || m_audioChannelCount != channels || m_audioSamplingRate != sampleRate || m_audioSbrUsed != sbrUsed || m_audioPsUsed != psUsed) {
        std::cout << m_logTag << "audioFormatChanged: ASCTY: " << +ascty <<  ", Sampling: " << +sampleRate << " : " << +channels << std::endl;
        m_ascty = ascty;
        m_audioChannelCount = channels;
        m_audioSamplingRate = sampleRate;
        m_audioSbrUsed = sbrUsed;
        m_audioPsUsed = psUsed;

        if(!enve->IsSameObject(m_linkedJavaDabServiceObject, NULL)) {
            enve->CallVoidMethod(m_linkedJavaDabServiceObject, m_javaDabSrvAudioformatChangedCallbackMId, m_ascty, m_audioChannelCount, m_audioSamplingRate, m_audioSbrUsed, m_audioPsUsed);
        } else {
            std::cout << m_logTag << "LinkedDabServiceObject is NULL at changed audioparams!" << std::endl;
        }
    }

    jbyteArray data = enve->NewByteArray(static_cast<jsize>(audioData.size()));
    if(data == nullptr) {
        return;
    }
    enve->SetByteArrayRegion(data, 0, static_cast<jsize>(audioData.size()), (jbyte*)audioData.data());

    if(!enve->IsSameObject(m_linkedJavaDabServiceObject, NULL)) {
        enve->CallVoidMethod(m_linkedJavaDabServiceObject, m_javaDabSrvAudioDataCallbackMId, data, channels, sampleRate);
    } else {
        std::cout << m_logTag << "LinkedDabServiceObject is NULL at audiodata callback" << std::endl;
    }

    enve->DeleteLocalRef(data);

    if(wasDetached) {
        m_javaVm->DetachCurrentThread();
    }
}

void JDabService::decodeAudio(bool decode) {
    m_decodeAudio = decode;
    if(!m_decodeAudio) {
        m_audioSamplingRate = -1;
        m_audioChannelCount = -1;
    } else {
        if(m_lastDynamicLabel != nullptr) {
            callJavaDynamiclabelCallback(m_lastDynamicLabel);
        }
        if(m_lastSlideshow != nullptr) {
            callJavaSlideshowCallback(m_lastSlideshow);
        }
    }
}

bool JDabService::isDecodingAudio() {
    return m_decodeAudio;
}

void JDabService::dynamicLabelInput(std::shared_ptr<void> label) {
    m_lastDynamicLabel = std::static_pointer_cast<DabDynamicLabel>(label);

    if(m_lastDynamicLabel == nullptr) {
        std::cout << m_logTag << "SharedPointerCast to DynamicLabel failed!" << std::endl;
        return;
    }

    if(m_decodeAudio) {
        callJavaDynamiclabelCallback(m_lastDynamicLabel);
    }
}

void JDabService::slideshowInput(std::shared_ptr<void> slideShow) {
    m_lastSlideshow = std::static_pointer_cast<DabSlideshow>(slideShow);

    if(m_lastSlideshow == nullptr) {
        std::cout << m_logTag << "SharedPointerCast to Slideshow failed!" << std::endl;
        return;
    }

    if(m_lastSlideshow->slideshowData.empty()) {
        std::cout << m_logTag << "Slideshow is empty!" << std::endl;
        return;
    }

    if(m_decodeAudio) {
        callJavaSlideshowCallback(m_lastSlideshow);
    }
}

void JDabService::callJavaSlideshowCallback(const std::shared_ptr<DabSlideshow>& slide) {
    bool wasDetached = false;
    JNIEnv* enve;

    int envState = m_javaVm->GetEnv((void**)&enve, JNI_VERSION_1_6);
    if(envState == JNI_EDETACHED) {
        if(m_javaVm->AttachCurrentThread(&enve, nullptr) == 0) {
            wasDetached = true;
        } else {
            std::cout << m_logTag << "jniEnv thread failed to attach!" << std::endl;
            return;
        }
    }

    jobject slsObject = enve->NewObject(m_javaSlsClass, m_javaSlsConstructorMId);

    jstring slsContentName = enve->NewStringUTF(slide->contentName.c_str());
    jbyteArray visualData = enve->NewByteArray(static_cast<jsize>(slide->slideshowData.size()));
    if(visualData == nullptr) {
        return;
    }
    enve->SetByteArrayRegion(visualData, 0, static_cast<jsize>(slide->slideshowData.size()), (jbyte*)slide->slideshowData.data());

    enve->CallVoidMethod(slsObject, m_javaSlsSetContentNameMId, slsContentName);
    enve->CallVoidMethod(slsObject, m_javaSlsSetVisualDataMId, visualData);
    enve->CallVoidMethod(slsObject, m_javaSlsSetMimeTypeMId, MIME_LINK_TABLE[slide->contentSubType]);
    enve->CallVoidMethod(slsObject, m_javaSlsSetContentTypeMId, 2);
    enve->CallVoidMethod(slsObject, m_javaSlsSetContentSubTypeMId, slide->contentSubType);

    if(slide->isCategorized) {
        jstring slsCategoryName = enve->NewStringUTF(slide->categoryTitle.c_str());
        enve->CallVoidMethod(slsObject, m_javaSlsSetCategoryTextMId, slsCategoryName);
        enve->DeleteLocalRef(slsCategoryName);

        enve->CallVoidMethod(slsObject, m_javaSlsSetSlideIdMId, slide->slideId);
        enve->CallVoidMethod(slsObject, m_javaSlsSetCategoryIdMId, slide->categoryId);

        if(!slide->alternativeLocationUrl.empty()) {
            jstring slsAltLocUrl = enve->NewStringUTF(slide->alternativeLocationUrl.c_str());
            enve->CallVoidMethod(slsObject, m_javaSlsSetAltLocUrlMId, slsAltLocUrl);
            enve->DeleteLocalRef(slsAltLocUrl);
        }
        if(!slide->clickThroughUrl.empty()) {
            jstring slsCtUrl = enve->NewStringUTF(slide->clickThroughUrl.c_str());
            enve->CallVoidMethod(slsObject, m_javaSlsSetClickThroughUrlMId, slsCtUrl);
            enve->DeleteLocalRef(slsCtUrl);
        }
    }

    if(!enve->IsSameObject(m_linkedJavaDabServiceObject, NULL)) {
        enve->CallVoidMethod(m_linkedJavaDabServiceObject, m_javaDabSrvslideshowReceivedCallbackMId, slsObject);
    }

    enve->DeleteLocalRef(visualData);
    enve->DeleteLocalRef(slsContentName);
    enve->DeleteLocalRef(slsObject);

    if(wasDetached) {
        m_javaVm->DetachCurrentThread();
    }
}

void JDabService::callJavaDynamiclabelCallback(const std::shared_ptr<DabDynamicLabel>& label) {
    bool wasDetached = false;
    JNIEnv* enve;

    int envState = m_javaVm->GetEnv((void**)&enve, JNI_VERSION_1_6);
    if(envState == JNI_EDETACHED) {
        if(m_javaVm->AttachCurrentThread(&enve, nullptr) == 0) {
            wasDetached = true;
        } else {
            std::cout << m_logTag << "jniEnv thread failed to attach!" << std::endl;
            return;
        }
    }

    jobject dlsObject = enve->NewObject(m_javaDlsClass, m_javaDlsConstructorMId);

    jstring fullDlsString;
    switch(label->charset) {
        case 0: {
            //std::cout << m_logTag << "EBU-CHAR String" << std::endl;
            fullDlsString = enve->NewStringUTF(convertEbuToUtf(label->dynamicLabel).c_str());
            break;
        }
        case 4: {
            //std::cout << m_logTag << "ISO-8859-1 String" << std::endl;
            fullDlsString = enve->NewStringUTF(label->dynamicLabel.c_str());
            break;
        }
        default: {
            //std::cout << m_logTag << "Charset String: " << +label->charset << " : " << label->dynamicLabel << std::endl;
            fullDlsString = enve->NewStringUTF(label->dynamicLabel.c_str());
            break;
        }
    }
    enve->CallVoidMethod(dlsObject, m_javaDlsSetFullTextMId, fullDlsString, label->charset);
    enve->DeleteLocalRef(fullDlsString);

    enve->CallVoidMethod(dlsObject, m_javaDlsSetItemRunningMId, label->itemRunning);
    enve->CallVoidMethod(dlsObject, m_javaDlsSetItemToggledMId, label->itemToggle);

    for(const auto& tag : label->dlPlusTags) {
        jobject dlPlusItemObject = enve->NewObject(m_javaDlPlusItemClass, m_javaDlPlusItemConstructorMId);
        enve->CallVoidMethod(dlPlusItemObject, m_javaDlPlusItemSetContentTypeMId, (jint)tag.contentType);

        jbyteArray tagTextBytes = enve->NewByteArray(static_cast<jsize>(tag.dlPlusTagText.size()));
        if(tagTextBytes == nullptr) {
            return;
        }
        enve->SetByteArrayRegion(tagTextBytes, 0, static_cast<jsize>(tag.dlPlusTagText.size()), (jbyte*)tag.dlPlusTagText.data());
        enve->CallVoidMethod(dlPlusItemObject, m_javaDlPlusItemSetTextMId, tagTextBytes, label->charset);
        enve->DeleteLocalRef(tagTextBytes);

        //add item to dls
        enve->CallVoidMethod(dlsObject, m_javaDlsAddTagItemMId, dlPlusItemObject);

        enve->DeleteLocalRef(dlPlusItemObject);
    }
    if(!enve->IsSameObject(m_linkedJavaDabServiceObject, NULL)) {
        enve->CallVoidMethod(m_linkedJavaDabServiceObject, m_javaDabSrvdynamicLabelReceivedCallbackMId, dlsObject);
    }

    enve->DeleteLocalRef(dlsObject);
    if(wasDetached) {
        m_javaVm->DetachCurrentThread();
    }
}

void JDabService::setSubchanHandle(uint8_t subChanHdl) {
    m_subchanHandle = subChanHdl;
}

uint16_t JDabService::getSubchanHandle() {
    return m_subchanHandle;
}


