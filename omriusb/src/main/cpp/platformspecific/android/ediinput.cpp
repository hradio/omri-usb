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

#include "ediinput.h"

EdiInput::EdiInput(JavaVM* javaVm, JNIEnv* env, jobject tunerEdiStream) {
    std::cout << LOG_TAG << "Constructing...." << std::endl;

    m_javaVm = javaVm;
    m_env = env;

    m_ediTunerObject = env->NewGlobalRef(tunerEdiStream);

    m_ensembleFinishedCb = DabEnsemble::registerEnsembleCollectDoneCallback(std::bind(&EdiInput::ensembleCollectFinished, this));
    m_dabTimeCallback = DabEnsemble::registerDateTimeCallback(std::bind(&EdiInput::dabTimeUpdate, this, std::placeholders::_1));
}

EdiInput::~EdiInput() {
    std::cout << LOG_TAG << "Destructing...." << std::endl;

    m_env->DeleteGlobalRef(m_ediTunerObject);
}

void EdiInput::setJavaClassDabTime(JNIEnv *env, jclass ediDabTimeClass) {
    m_dabTimeClass = ediDabTimeClass;

    m_dabTimeConstructorMID = env->GetMethodID(m_dabTimeClass, "<init>", "(IIIIIII)V");

    m_radioClass = env->FindClass("org/omri/radio/Radio");
    m_radioImplClass = env->FindClass("org/omri/radio/impl/RadioImpl");
    m_radioClassGetInstanceMID = env->GetStaticMethodID(m_radioClass, "getInstance", "()Lorg/omri/radio/Radio;");
    m_radioInstanceObj = env->CallStaticObjectMethod(m_radioClass, m_radioClassGetInstanceMID);
    m_radioInstanceDabTimeMID = env->GetMethodID(m_radioImplClass, "dabTime", "(Lorg/omri/radio/impl/DabTime;)V");
}

void EdiInput::setJavaClassEdiTuner(JNIEnv *env, jclass ediTunerClass) {
    m_ediTunerClass = ediTunerClass;

    m_ediTunerServiceStartedMId = env->GetMethodID(m_ediTunerClass, "serviceStarted", "(Lorg/omri/radioservice/RadioServiceDabEdi;)V");
    m_ediTunerServiceStoppedMId = env->GetMethodID(m_ediTunerClass, "serviceStopped", "(Lorg/omri/radioservice/RadioServiceDabEdi;)V");
    m_ediTunerServiceUpdated = env->GetMethodID(m_ediTunerClass, "serviceUpdated", "(Lorg/omri/radioservice/RadioServiceDabEdi;)V");
    m_ediTunerIsScanningMId = env->GetMethodID(m_ediTunerClass, "isTunerScanning", "()Z");

    //SBT
    m_ediTunerDabTimeUpdate = env->GetMethodID(m_ediTunerClass, "dabTimeUpdate", "(Lorg/omri/radio/impl/DabTime;)V");
    m_ediTunerNewLiveItem = env->GetMethodID(m_ediTunerClass, "newLiveItem", "([B)V");
}

void EdiInput::setJavaClassDabService(JNIEnv* env, jclass dabServiceClass) {
    //local reference from GlobalRef in OnLoad
    m_dabServiceClass = dabServiceClass;

    m_dabServiceConstructorMId = env->GetMethodID(m_dabServiceClass, "<init>", "()V");
    m_dabServiceSetEnsembleEccMId = env->GetMethodID(m_dabServiceClass, "setEnsembleEcc", "(I)V");
    m_dabServiceSetEnsembleIdMId = env->GetMethodID(m_dabServiceClass, "setEnsembleId", "(I)V");
    m_dabServiceSetEnsembleLabelMId = env->GetMethodID(m_dabServiceClass, "setEnsembleLabel", "(Ljava/lang/String;)V");
    m_dabServiceSetEnsembleShortLabelMId = env->GetMethodID(m_dabServiceClass, "setEnsembleShortLabel", "(Ljava/lang/String;)V");
    m_dabServiceSetIsCaAppliedMId = env->GetMethodID(m_dabServiceClass, "setIsCaProtected", "(Z)V");
    m_dabServiceSetCaIdMId = env->GetMethodID(m_dabServiceClass, "setCaId", "(I)V");
    m_dabServiceSetEnsembleFrequencyMId = env->GetMethodID(m_dabServiceClass, "setEnsembleFrequency", "(I)V");
    m_dabServiceSetServiceLabelMId = env->GetMethodID(m_dabServiceClass, "setServiceLabel", "(Ljava/lang/String;)V");
    m_dabServiceSetServiceShortLabelMId = env->GetMethodID(m_dabServiceClass, "setShortLabel", "(Ljava/lang/String;)V");
    m_dabServiceSetServiceIdMId = env->GetMethodID(m_dabServiceClass, "setServiceId", "(I)V");
    m_dabServiceSetServiceIsProgrammeMId = env->GetMethodID(m_dabServiceClass, "setIsProgrammeService", "(Z)V");
    m_dabServiceAddServiceComponentMId = env->GetMethodID(m_dabServiceClass, "addServiceComponent", "(Lorg/omri/radioservice/RadioServiceDabComponent;)V");

    m_dabServiceAddGenreTermIdMId = env->GetMethodID(m_dabServiceClass, "addGenre", "(Lorg/omri/radioservice/metadata/TermId;)V");
}

void EdiInput::setJavaClassDabServiceComponent(JNIEnv *env, jclass dabServiceComponentClass) {
    //local reference from GlobalRef in OnLoad
    m_dabServiceComponentClass = dabServiceComponentClass;

    m_dabServiceComponentConstructorMId = env->GetMethodID(m_dabServiceComponentClass, "<init>", "()V");
    m_dabServiceComponentSetBitrateMId = env->GetMethodID(m_dabServiceComponentClass, "setScBitrate", "(I)V");
    m_dabServiceComponentSetCaFlagMId = env->GetMethodID(m_dabServiceComponentClass, "setIsScCaFlagSet", "(Z)V");
    m_dabServiceComponentSetServiceIdMId = env->GetMethodID(m_dabServiceComponentClass, "setServiceId", "(I)V");
    m_dabServiceComponentSetSubchannelIdMId = env->GetMethodID(m_dabServiceComponentClass, "setSubchannelId", "(I)V");
    m_dabServiceComponentSetLabelMId = env->GetMethodID(m_dabServiceComponentClass, "setScLabel", "(Ljava/lang/String;)V");
    m_dabServiceComponentSetPacketAddressMId = env->GetMethodID(m_dabServiceComponentClass, "setPacketAddress", "(I)V");
    m_dabServiceComponentSetIsPrimaryMId = env->GetMethodID(m_dabServiceComponentClass, "setIsScPrimary", "(Z)V");
    m_dabServiceComponentSetScIDsMId = env->GetMethodID(m_dabServiceComponentClass, "setServiceComponentIdWithinService", "(I)V");
    m_dabServiceComponentSetTransportModeIdMId = env->GetMethodID(m_dabServiceComponentClass, "setTmId", "(I)V");
    m_dabServiceComponentSetScTypeMId = env->GetMethodID(m_dabServiceComponentClass, "setServiceComponentType", "(I)V");
    m_dabServiceComponentSetIsDgUsedMId = env->GetMethodID(m_dabServiceComponentClass, "setDatagroupTransportUsed", "(Z)V");
    m_dabServiceComponentSetMscStartAddressMId = env->GetMethodID(m_dabServiceComponentClass, "setMscStartAddress", "(I)V");
    m_dabServiceComponentSetSubchanSizeMId = env->GetMethodID(m_dabServiceComponentClass, "setSubchannelSize", "(I)V");
    m_dabServiceComponentSetProtectionLvlMId = env->GetMethodID(m_dabServiceComponentClass, "setProtectionLevel", "(I)V");
    m_dabServiceComponentSetProtectionTypeMId = env->GetMethodID(m_dabServiceComponentClass, "setProtectionType", "(I)V");
    m_dabServiceComponentSetUepTblIdxMId = env->GetMethodID(m_dabServiceComponentClass, "setUepTableIndex", "(I)V");
    m_dabServiceComponentSetIsFecAppliedMId = env->GetMethodID(m_dabServiceComponentClass, "setIsFecSchemeApplied", "(Z)V");

    m_dabServiceComponentAddUserApplicationMId = env->GetMethodID(m_dabServiceComponentClass, "addScUserApplication", "(Lorg/omri/radioservice/RadioServiceDabUserApplication;)V");
}

void EdiInput::setJavaClassDabServiceUserApplication(JNIEnv *env, jclass dabServiceUserAppClass) {
    //local reference from GlobalRef in OnLoad
    m_dabServiceUserApplicationClass = dabServiceUserAppClass;

    m_dabServiceUserApplicationConstructorMId = env->GetMethodID(m_dabServiceUserApplicationClass, "<init>", "()V");
    m_dabServiceUserApplicationSetAppTypeMId = env->GetMethodID(m_dabServiceUserApplicationClass, "setUserApplicationType", "(I)V");
    m_dabServiceUserApplicationSetIsCaAppliedMId = env->GetMethodID(m_dabServiceUserApplicationClass, "setIsCaProtected", "(Z)V");
    m_dabServiceUserApplicationSetCaOrgMId = env->GetMethodID(m_dabServiceUserApplicationClass, "setCaOrganization", "(I)V");
    m_dabServiceUserApplicationSetIsXPadMId = env->GetMethodID(m_dabServiceUserApplicationClass, "setIsXpadApptype", "(Z)V");
    m_dabServiceUserApplicationSetXPadAppTypeMId = env->GetMethodID(m_dabServiceUserApplicationClass, "setXpadApptype", "(I)V");
    m_dabServiceUserApplicationSetIsDgUsedMId = env->GetMethodID(m_dabServiceUserApplicationClass, "setIsDatagroupsUsed", "(Z)V");
    m_dabServiceUserApplicationSetDSCTyMId = env->GetMethodID(m_dabServiceUserApplicationClass, "setDSCTy", "(I)V");
    m_dabServiceUserApplicationSetUappDataMId = env->GetMethodID(m_dabServiceUserApplicationClass, "setUappdata", "([B)V");
}

void EdiInput::setJavaClassTermId(JNIEnv *env, jclass dabServiceClass) {
    //local reference from GlobalRef in OnLoad
    m_termIdClass = dabServiceClass;

    m_termIdConstructorMId = env->GetMethodID(m_termIdClass, "<init>", "()V");
    m_termIdSetGenreHrefMId = env->GetMethodID(m_termIdClass, "setGenreHref", "(Ljava/lang/String;)V");
    m_termIdSetTermIdMId = env->GetMethodID(m_termIdClass, "setTermId", "(Ljava/lang/String;)V");
    m_termIdSetGenreTextMId = env->GetMethodID(m_termIdClass, "setGenreText", "(Ljava/lang/String;)V");
}

void EdiInput::startService(std::shared_ptr<JDabService> serviceLink) {
    std::cout << LOG_TAG << "Starting service" << std::endl;

    reset();
    m_ediBuffer.clear();

    if(m_startServiceLink != nullptr) {
        std::cout << LOG_TAG << "Setting old service to dont decode" << std::endl;
        m_startServiceLink->decodeAudio(false);
    }

    reset();
    m_ediBuffer.clear();

    std::cout << LOG_TAG << "Linking new service" << std::endl;
    m_startServiceLink = serviceLink;
}

void EdiInput::stopService(const DabService &service) {
    //TODO stopService()
}

void EdiInput::stopAllRunningServices() {
    //TODO stopAllRunningServices()
}

void EdiInput::dabTimeUpdate(const Fig_00_Ext_10::DabTime& dabTime) {
    //std::cout << LOG_TAG << "SBT DabTime: " << dabTime.unixTimestampSeconds << std::endl;

    bool wasDetached = false;
    JNIEnv* enve;

    int envState = m_javaVm->GetEnv((void**)&enve, JNI_VERSION_1_6);
    if(envState == JNI_EDETACHED) {
        if(m_javaVm->AttachCurrentThread(&enve, NULL) == 0) {
            wasDetached = true;
        } else {
            std::cout << "jniEnv thread failed to attach!" << std::endl;
            return;
        }
    }

    jobject dabtimeObject = enve->NewObject(m_dabTimeClass, m_dabTimeConstructorMID, dabTime.year, dabTime.month, dabTime.day, dabTime.hour, dabTime.minute, dabTime.second, dabTime.milliseconds);
    enve->CallVoidMethod(m_ediTunerObject, m_ediTunerDabTimeUpdate, dabtimeObject);

    //enve->CallVoidMethod(m_radioInstanceObj, m_radioInstanceDabTimeMID, dabtimeObject);

    enve->DeleteLocalRef(dabtimeObject);

    if(wasDetached) {
        m_javaVm->DetachCurrentThread();
    }
}

void EdiInput::newLiveItem(std::vector<uint8_t> liveItemData) {
    std::cout << LOG_TAG << "SBT newLiveItem..." << std::endl;

    bool wasDetached = false;
    JNIEnv* enve;

    int envState = m_javaVm->GetEnv((void**)&enve, JNI_VERSION_1_6);
    if(envState == JNI_EDETACHED) {
        if(m_javaVm->AttachCurrentThread(&enve, NULL) == 0) {
            wasDetached = true;
        } else {
            std::cout << "jniEnv thread failed to attach!" << std::endl;
            return;
        }
    }

    jbyteArray data = enve->NewByteArray(static_cast<jsize>(liveItemData.size()));
    if(data == nullptr) {
        return;
    }
    enve->SetByteArrayRegion(data, 0, static_cast<jsize>(liveItemData.size()), (jbyte*)liveItemData.data());

    enve->CallVoidMethod(m_ediTunerObject, m_ediTunerNewLiveItem, data);

    enve->DeleteLocalRef(data);

    if(wasDetached) {
        m_javaVm->DetachCurrentThread();
    }
}

void EdiInput::ensembleCollectFinished() {
    std::cout << LOG_TAG << "Ensemble collect finished" << std::endl;

    bool wasDetached = false;
    JNIEnv* enve;

    int envState = m_javaVm->GetEnv((void**)&enve, JNI_VERSION_1_6);
    if(envState == JNI_EDETACHED) {
        if(m_javaVm->AttachCurrentThread(&enve, NULL) == 0) {
            wasDetached = true;
        } else {
            std::cout << "jniEnv thread failed to attach!" << std::endl;
            return;
        }
    }

    jboolean isScanning = enve->CallBooleanMethod(m_ediTunerObject, m_ediTunerIsScanningMId);
    if(isScanning) {
        jstring ensembleLabel = enve->NewStringUTF(getEnsembleLabel().data());
        jstring ensembleShortLabel = enve->NewStringUTF(getEnsembleShortLabel().data());

        enve->CallVoidMethod(m_startServiceLink->getJavaDabServiceObject(), m_dabServiceSetEnsembleEccMId, getEnsembleEcc());
        enve->CallVoidMethod(m_startServiceLink->getJavaDabServiceObject(), m_dabServiceSetEnsembleIdMId, getEnsembleId());
        enve->CallVoidMethod(m_startServiceLink->getJavaDabServiceObject(), m_dabServiceSetEnsembleLabelMId, ensembleLabel);
        enve->CallVoidMethod(m_startServiceLink->getJavaDabServiceObject(), m_dabServiceSetEnsembleShortLabelMId, ensembleShortLabel);
        enve->CallVoidMethod(m_startServiceLink->getJavaDabServiceObject(), m_dabServiceSetEnsembleFrequencyMId, (jint)0);

        auto srv = getDabServices()[0];
        std::cout << LOG_TAG << "Scan Service: " << srv->getServiceLabel() << std::endl;
        if(srv->isCaApplied() > 0) {
            enve->CallVoidMethod(m_startServiceLink->getJavaDabServiceObject(), m_dabServiceSetIsCaAppliedMId, JNI_TRUE);
        } else {
            enve->CallVoidMethod(m_startServiceLink->getJavaDabServiceObject(), m_dabServiceSetIsCaAppliedMId, JNI_FALSE);
        }

        enve->CallVoidMethod(m_startServiceLink->getJavaDabServiceObject(), m_dabServiceSetCaIdMId, (jint)srv->getCaId());

        jstring dabServiceLabel = enve->NewStringUTF(srv->getServiceLabel().data());
        jstring dabServiceShortLabel = enve->NewStringUTF(srv->getServiceShortLabel().data());

        enve->CallVoidMethod(m_startServiceLink->getJavaDabServiceObject(), m_dabServiceSetServiceLabelMId, dabServiceLabel);
        enve->CallVoidMethod(m_startServiceLink->getJavaDabServiceObject(), m_dabServiceSetServiceShortLabelMId, dabServiceShortLabel);
        enve->CallVoidMethod(m_startServiceLink->getJavaDabServiceObject(), m_dabServiceSetServiceIdMId, srv->getServiceId());

        if(srv->isProgrammeService() > 0) {
            enve->CallVoidMethod(m_startServiceLink->getJavaDabServiceObject(), m_dabServiceSetServiceIsProgrammeMId, JNI_TRUE);
        } else {
            enve->CallVoidMethod(m_startServiceLink->getJavaDabServiceObject(), m_dabServiceSetServiceIsProgrammeMId, JNI_FALSE);
        }

        jstring genrePty = enve->NewStringUTF(srv->getProgrammeTypeFullName().data());
        jobject termIdObject = enve->NewObject(m_termIdClass, m_termIdConstructorMId);

        enve->CallVoidMethod(termIdObject, m_termIdSetGenreTextMId, genrePty);
        enve->CallVoidMethod(m_startServiceLink->getJavaDabServiceObject(), m_dabServiceAddGenreTermIdMId, termIdObject);
        enve->DeleteLocalRef(genrePty);

        enve->DeleteLocalRef(termIdObject);

        //DABServiceComponent creation
        for(const auto& srvComp : srv->getServiceComponents()) {
            jobject dabServiceComponentObject = enve->NewObject(m_dabServiceComponentClass, m_dabServiceComponentConstructorMId);

            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetBitrateMId, (jint)srvComp->getSubchannelBitrate());
            if(srvComp->isCaApplied() > 0) {
                enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetCaFlagMId, JNI_TRUE);
            } else {
                enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetCaFlagMId, JNI_FALSE);
            }

            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetServiceIdMId, (jint)srv->getServiceId());
            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetSubchannelIdMId, srvComp->getSubChannelId());

            jstring dabServiceComponentLabel = enve->NewStringUTF(srvComp->getServiceComponentLabel().data());
            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetLabelMId, dabServiceComponentLabel);

            jint packetAddress;
            uint8_t tmId;
            jint serviceComponentType;
            jboolean dgUsed = 0;
            switch(srvComp->getServiceComponentType()) {
                case DabServiceComponent::SERVICECOMPONENTTYPE::MSC_STREAM_AUDIO: {
                    auto audioComp = std::static_pointer_cast<DabServiceComponentMscStreamAudio>(srvComp);
                    serviceComponentType = (jint)audioComp->getAudioServiceComponentType();
                    packetAddress = -1;
                    tmId = 0;
                    dgUsed = 0;
                    break;
                }
                case DabServiceComponent::SERVICECOMPONENTTYPE::MSC_STREAM_DATA: {
                    //TODO values for MSC_DATA_STREAM_MODE
                    packetAddress = -1;
                    tmId = 1;
                    serviceComponentType = -1;
                    dgUsed = 0;
                    break;
                }
                case DabServiceComponent::SERVICECOMPONENTTYPE::MSC_PACKET_MODE_DATA: {
                    auto packetComp = std::static_pointer_cast<DabServiceComponentMscPacketData>(srvComp);
                    packetAddress = (jint)packetComp->getPacketAddress();
                    serviceComponentType = (jint)packetComp->getDataServiceComponentType();
                    dgUsed = (jboolean)(packetComp->isDataGroupTransportUsed() ? 1 : 0);
                    tmId = 3;
                    break;
                }
                default: {
                    packetAddress = -1;
                    serviceComponentType = -1;
                    tmId = 2;
                    dgUsed = JNI_FALSE;
                    break;
                }
            }

            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetPacketAddressMId, packetAddress);
            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetIsDgUsedMId, dgUsed);
            if(srvComp->isPrimary() > 0) {
                enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetIsPrimaryMId, JNI_TRUE);
            } else {
                enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetIsPrimaryMId, JNI_FALSE);
            }

            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetScIDsMId, (jint)srvComp->getServiceComponentIdWithinService());
            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetTransportModeIdMId, (jint)tmId);
            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetMscStartAddressMId, (jint)srvComp->getMscStartAddress());
            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetSubchanSizeMId, (jint)srvComp->getSubchannelSize());
            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetProtectionLvlMId, (jint)srvComp->getProtectionLevel());
            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetProtectionTypeMId, (jint)srvComp->getProtectionType());
            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetUepTblIdxMId, (jint)srvComp->getUepTableIndex());
            if(srvComp->isFecSchemeApplied() > 0) {
                enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetIsFecAppliedMId, JNI_TRUE);
            } else {
                enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetIsFecAppliedMId, JNI_FALSE);
            }

            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetScTypeMId, serviceComponentType);

            enve->DeleteLocalRef(dabServiceComponentLabel);

            //DABUserApplication creation
            for(const auto& uApp : srvComp->getUserApplications()) {
                if(uApp.getUserApplicationType() == registeredtables::USERAPPLICATIONTYPE::DYNAMIC_LABEL) {
                    continue;
                }

                jobject dabServiceUserapplicationObject = enve->NewObject(m_dabServiceUserApplicationClass, m_dabServiceUserApplicationConstructorMId);
                enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetAppTypeMId, (jint)uApp.getUserApplicationType());
                if(uApp.isCaApplied() > 0) {
                    enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetIsCaAppliedMId, JNI_TRUE);
                } else {
                    enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetIsCaAppliedMId, JNI_FALSE);
                }

                enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetCaOrgMId, (jint)uApp.getCaOrganization());
                if(uApp.isXpadApp() > 0) {
                    enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetIsXPadMId, JNI_TRUE);
                } else {
                    enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetIsXPadMId, JNI_FALSE);
                }

                enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetXPadAppTypeMId, (jint)uApp.getXpadAppType());

                if(uApp.dataGroupsUsed() > 0) {
                    enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetIsDgUsedMId, JNI_TRUE);
                } else {
                    enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetIsDgUsedMId, JNI_FALSE);
                }

                enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetDSCTyMId, (jint)uApp.getDataServiceComponentType());

                if(!uApp.getUserApplicationData().empty()) {
                    jbyteArray uAppData = enve->NewByteArray(uApp.getUserApplicationData().size());
                    enve->SetByteArrayRegion(uAppData, 0, uApp.getUserApplicationData().size(), (jbyte*)uApp.getUserApplicationData().data());

                    enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetUappDataMId, uAppData);
                    enve->DeleteLocalRef(uAppData);
                }

                //Add Userapplication to DabServiceComponent
                enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentAddUserApplicationMId, dabServiceUserapplicationObject);

                enve->DeleteLocalRef(dabServiceUserapplicationObject);
            }
            //DABUserApplication creation END

            //Add DabServiceComponent to DabService
            enve->CallVoidMethod(m_startServiceLink->getJavaDabServiceObject(), m_dabServiceAddServiceComponentMId, dabServiceComponentObject);

            enve->DeleteLocalRef(dabServiceComponentObject);
        }

        enve->DeleteLocalRef(ensembleLabel);
        enve->DeleteLocalRef(ensembleShortLabel);

        enve->DeleteLocalRef(dabServiceLabel);
        enve->DeleteLocalRef(dabServiceShortLabel);

        enve->CallVoidMethod(m_ediTunerObject, m_ediTunerServiceUpdated, m_startServiceLink->getJavaDabServiceObject());
    } else {
        m_startServiceLink->setLinkDabService(getDabServices()[0]);
        m_startServiceLink->decodeAudio(true);

        enve->CallVoidMethod(m_ediTunerObject, m_ediTunerServiceStartedMId, m_startServiceLink->getJavaDabServiceObject());
    }

    if(wasDetached) {
        m_javaVm->DetachCurrentThread();
    }
}

void EdiInput::flushComponentBuffer() {
    m_ediBuffer.clear();
    flushAllBufferedComponentData();
}

uint16_t m_lastSeq = 0;
void EdiInput::ediDataInput(const std::vector<uint8_t> &data, int size) {
    //std::cout << LOG_TAG << "EDI ChunkSize: " << +data.size() << " : " << +size << std::endl;

    std::vector<uint8_t> ediData;
    if(!m_ediBuffer.empty()) {
        ediData.insert(ediData.begin(), m_ediBuffer.begin(), m_ediBuffer.end());
        m_ediBuffer.clear();
    }

    ediData.insert(ediData.end(), data.begin(), data.end());

    //Application Frame
    auto ediIter = ediData.cbegin();
    while(ediIter < ediData.cend()) {
        if(*ediIter++ == 'A' && *ediIter++ == 'F') {
            long bytesRemaining = std::distance(ediIter, ediData.cend());

            //No more data left for reading AF length
            if(bytesRemaining < 4) {
                //std::cout << LOG_TAG << "Not enough data for AF Size, available: " << +bytesRemaining << std::endl;
                m_ediBuffer.insert(m_ediBuffer.begin(), ediIter-2, ediData.cend());
                return;
            }

            uint32_t afLenBytes = static_cast<uint32_t>(((*ediIter++ & 0xFF) << 24) | ((*ediIter++ & 0xFF) << 16) | ((*ediIter++ & 0xFF) << 8) | (*ediIter++ & 0xFF));

            if(bytesRemaining-4 < afLenBytes) {
                bytesRemaining -= 4;
                //std::cout << LOG_TAG << "Not enough data for AF, needed: " << +afLenBytes << ", available: " << +bytesRemaining << " : " << *(ediIter-6) << " : " << *(ediIter-5) <<std::endl;
                m_ediBuffer.insert(m_ediBuffer.begin(), ediIter-6, ediData.cend());
                return;
            }

            uint16_t seqNum = static_cast<uint16_t>(((*ediIter++ & 0xFF) << 8) | (*ediIter++ & 0xFF));
            if(m_lastSeq+1 != seqNum) {
                std::cout << LOG_TAG << "SeqNum: " << +m_lastSeq << " : " << +seqNum << std::endl;
            }

            m_lastSeq = seqNum;

            //AR
            bool crcFlag = (*ediIter & 0x80 >> 7) != 0;
            uint8_t majVer = static_cast<uint8_t>((*ediIter & 0x70 >> 4));
            uint8_t minVer = static_cast<uint8_t>(*ediIter++ & 0x0F);

            uint8_t protoType = static_cast<uint8_t >(*ediIter++ & 0xFF);

            //std::cout << LOG_TAG << "AF Sync Len: " << +afLenBytes << " : " << size << std::endl;

            auto afIter = ediIter;
            auto afIterEnd = ediIter+afLenBytes+2;

            ediIter += afLenBytes+2;

            while(afIter < afIterEnd) {
                //Other Tags inside
                uint32_t tagName = static_cast<uint32_t>(((*afIter++ & 0xFF) << 24) | ((*afIter++ & 0xFF) << 16) | ((*afIter++ & 0xFF) << 8) | (*afIter++ & 0xFF));

                uint32_t tagLenBits;
                if (tagName == '*ptr') {
                    tagLenBits = static_cast<uint32_t>(((*afIter++ & 0xFF) << 24) | ((*afIter++ & 0xFF) << 16) | ((*afIter++ & 0xFF) << 8) | (*afIter++ & 0xFF));
                    uint32_t protoName = static_cast<uint32_t>(((*afIter++ & 0xFF) << 24) | ((*afIter++ & 0xFF) << 16) | ((*afIter++ & 0xFF) << 8) | (*afIter++ & 0xFF));
                    uint16_t majVer = static_cast<uint16_t>(((*afIter++ & 0xFF) << 8) | (*afIter++ & 0xFF));
                    uint16_t minVer = static_cast<uint16_t>(((*afIter++ & 0xFF) << 8) | (*afIter++ & 0xFF));
                }

                if (tagName == 'deti') {
                    tagLenBits = static_cast<uint32_t>(((*afIter++ & 0xFF) << 24) | ((*afIter++ & 0xFF) << 16) | ((*afIter++ & 0xFF) << 8) | (*afIter++ & 0xFF));
                    bool atstF = ((*afIter & 0x80) >> 7) != 0;
                    bool ficF = ((*afIter & 0x40) >> 6) != 0;
                    bool rfudF = ((*afIter & 0x20) >> 5) != 0;

                    uint8_t fctH = static_cast<uint8_t>((*afIter++ & 0x1F) & 0xFF);
                    uint8_t fct = static_cast<uint8_t>((*afIter++) & 0xFF);

                    //ETI Header
                    uint8_t stat = static_cast<uint8_t>((*afIter++) & 0xFF);
                    uint8_t modeId = static_cast<uint8_t>((*afIter & 0xC0) >> 6);
                    uint8_t fp = static_cast<uint8_t>((*afIter++ & 0x38) >> 3);
                    uint16_t mnsc = static_cast<uint16_t>(((*afIter++ & 0xFF) << 8) | (*afIter++ & 0xFF));

                    if(atstF) {
                        afIter += 8;
                    }

                    if(ficF) {
                        if(modeId != 3) {
                            dataInput(std::vector<uint8_t>(afIter, afIter+96), 0x64, true);
                            afIter += 96;
                        } else {
                            dataInput(std::vector<uint8_t>(afIter, afIter+128), 0x64, true);
                            afIter += 128;
                        }
                    }

                    if(rfudF) {
                        afIter += 3;
                    }
                }

                //TODO only one substream handled now
                if(tagName == 0x65737401 /* est1 */) {
                    tagLenBits = static_cast<uint32_t>(((*afIter++ & 0xFF) << 24) | ((*afIter++ & 0xFF) << 16) | ((*afIter++ & 0xFF) << 8) | (*afIter++ & 0xFF));
                    uint32_t mstLenBytes = (tagLenBits/8) -3;
                    uint8_t subChanId = static_cast<uint8_t>((*afIter & 0xFC) >> 2);
                    uint16_t sad = static_cast<uint16_t >(((*afIter++ & 0x03) << 8) | (*afIter++ & 0xFF));
                    uint8_t tpl = static_cast<uint8_t>((*afIter++ & 0xFC) >> 2);
                    dataInput(std::vector<uint8_t>(afIter, afIter+mstLenBytes), subChanId, true);

                    afIter += mstLenBytes;
                }

                //SBT live item. dlpt -> Dynamic Label Plus Tag
                if(tagName == 'dlpt') {
                    tagLenBits = static_cast<uint32_t>(((*afIter++ & 0xFF) << 24) | ((*afIter++ & 0xFF) << 16) | ((*afIter++ & 0xFF) << 8) | (*afIter++ & 0xFF));
                    uint32_t dlptLenBytes = tagLenBits / 8;

                    newLiveItem(std::vector<uint8_t>(afIter, afIter+dlptLenBytes));

                    afIter += dlptLenBytes;
                }
            }
        }
    }

    m_ediBuffer.insert(m_ediBuffer.begin(), ediIter, ediData.cend());
}