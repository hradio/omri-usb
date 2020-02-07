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

#include "jtunerusbdevice.h"

#include <iostream>

JTunerUsbDevice::JTunerUsbDevice(JavaVM* javaVm, JNIEnv* env, jobject tunerUsbDevice) : JUsbDevice(javaVm, env, env->CallObjectMethod(tunerUsbDevice, env->GetMethodID(env->FindClass("org/omri/radio/impl/TunerUsb"), "getUsbDevice", "()Landroid/hardware/usb/UsbDevice;"))) {
    std::cout << m_logTag << "Creating JTuner" << std::endl;

    m_javaVm = javaVm;
    m_env = env;

    m_usbTunerObject = env->NewGlobalRef(tunerUsbDevice);
}

JTunerUsbDevice::~JTunerUsbDevice() {
    m_env->DeleteGlobalRef(m_usbTunerObject);
}

void JTunerUsbDevice::setJavaClassUsbTuner(JNIEnv* env, jclass usbTunerClass) {
    //local reference from GlobalRef in OnLoad
    m_usbTunerClass = usbTunerClass;

    m_usbTunerCallbackMId = env->GetMethodID(m_usbTunerClass, "callBack", "(I)V");
    m_usbTunerGetServicesMId = env->GetMethodID(m_usbTunerClass, "getRadioServices", "()Ljava/util/List;");
    m_usbTunerScanProgressMId = env->GetMethodID(m_usbTunerClass, "scanProgressCallback", "(I)V");
    m_usbTunerServiceFoundMId = env->GetMethodID(m_usbTunerClass, "serviceFound", "(Lorg/omri/radioservice/RadioServiceDab;)V");
    m_usbTunerServiceStartedMId = env->GetMethodID(m_usbTunerClass, "serviceStarted", "(Lorg/omri/radioservice/RadioServiceDab;)V");
    m_usbTunerServiceStoppedMId = env->GetMethodID(m_usbTunerClass, "serviceStopped", "(Lorg/omri/radioservice/RadioServiceDab;)V");
    m_usbTunerReceptionStatisticsMId = env->GetMethodID(m_usbTunerClass, "receptionStatistics", "(ZI)V");
}

void JTunerUsbDevice::setJavaClassDabService(JNIEnv* env, jclass dabServiceClass) {
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

void JTunerUsbDevice::setJavaClassDabServiceComponent(JNIEnv *env, jclass dabServiceComponentClass) {
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

void JTunerUsbDevice::setJavaClassDabServiceUserApplication(JNIEnv *env, jclass dabServiceUserAppClass) {
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

void JTunerUsbDevice::setJavaClassTermId(JNIEnv *env, jclass dabServiceClass) {
    //local reference from GlobalRef in OnLoad
    m_termIdClass = dabServiceClass;

    m_termIdConstructorMId = env->GetMethodID(m_termIdClass, "<init>", "()V");
    m_termIdSetGenreHrefMId = env->GetMethodID(m_termIdClass, "setGenreHref", "(Ljava/lang/String;)V");
    m_termIdSetTermIdMId = env->GetMethodID(m_termIdClass, "setTermId", "(Ljava/lang/String;)V");
    m_termIdSetGenreTextMId = env->GetMethodID(m_termIdClass, "setGenreText", "(Ljava/lang/String;)V");
}

void JTunerUsbDevice::callCallback(TUNER_CALLBACK_TYPE callbackType) {
    std::cout << m_logTag << "Calling tuner callback: " << +callbackType << std::endl;

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

    enve->CallVoidMethod(m_usbTunerObject, m_usbTunerCallbackMId, callbackType);

    if(wasDetached) {
        m_javaVm->DetachCurrentThread();
    }
}

const JavaVM* JTunerUsbDevice::getJavaVM() const {
    return m_javaVm;
}

void JTunerUsbDevice::scanProgress(int percentDone) {
    std::cout << m_logTag << "Calling tuner scanprogress: " << +percentDone << std::endl;

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

    enve->CallVoidMethod(m_usbTunerObject, m_usbTunerScanProgressMId, percentDone);

    if(wasDetached) {
        m_javaVm->DetachCurrentThread();
    }
}

void JTunerUsbDevice::ensembleReady(const DabEnsemble& ensemble) {
    std::cout << m_logTag << " Scan ensemble ready: " << +ensemble.getDabServices().size() << " : "
              << ensemble.getEnsembleId() << std::endl;

    bool wasDetached = false;
    JNIEnv *enve;

    int envState = m_javaVm->GetEnv((void **) &enve, JNI_VERSION_1_6);
    if (envState == JNI_EDETACHED) {
        if (m_javaVm->AttachCurrentThread(&enve, nullptr) == 0) {
            wasDetached = true;
        } else {
            std::cout << "jniEnv thread failed to attach!" << std::endl;
            return;
        }
    }

    jstring ensembleLabel;
    jstring ensembleShortLabel;
    if (ensemble.getEnsembleLabelCharset() == 0) {
        ensembleLabel = enve->NewStringUTF(convertEbuToUtf(ensemble.getEnsembleLabel()).c_str());
        ensembleShortLabel = enve->NewStringUTF(convertEbuToUtf(ensemble.getEnsembleShortLabel()).c_str());
    } else {
        ensembleLabel = enve->NewStringUTF(ensemble.getEnsembleLabel().c_str());
        ensembleShortLabel = enve->NewStringUTF(ensemble.getEnsembleShortLabel().c_str());
    }

    for(const auto& srv : ensemble.getDabServices()) {
        std::cout << m_logTag << " Scan service: " << srv->getServiceLabel() << std::endl;

        jobject dabServiceObject = enve->NewObject(m_dabServiceClass, m_dabServiceConstructorMId);

        enve->CallVoidMethod(dabServiceObject, m_dabServiceSetEnsembleEccMId, ensemble.getEnsembleEcc());
        enve->CallVoidMethod(dabServiceObject, m_dabServiceSetEnsembleIdMId, ensemble.getEnsembleId());
        enve->CallVoidMethod(dabServiceObject, m_dabServiceSetEnsembleLabelMId, ensembleLabel);
        enve->CallVoidMethod(dabServiceObject, m_dabServiceSetEnsembleShortLabelMId, ensembleShortLabel);
        enve->CallVoidMethod(dabServiceObject, m_dabServiceSetEnsembleFrequencyMId, (jint)srv->getEnsembleFrequency());
        //enve->CallVoidMethod(dabServiceObject, m_dabServiceSetIsCaAppliedMId, static_cast<jboolean>(srv->isCaApplied()));
        if(srv->isCaApplied() > 0) {
            enve->CallVoidMethod(dabServiceObject, m_dabServiceSetIsCaAppliedMId, JNI_TRUE);
        } else {
            enve->CallVoidMethod(dabServiceObject, m_dabServiceSetIsCaAppliedMId, JNI_FALSE);
        }

        enve->CallVoidMethod(dabServiceObject, m_dabServiceSetCaIdMId, (jint)srv->getCaId());

        jstring dabServiceLabel;
        jstring dabServiceShortLabel;

        if(srv->getLabelCharset() == 0) {
            dabServiceLabel = enve->NewStringUTF(convertEbuToUtf(srv->getServiceLabel()).c_str());
            dabServiceShortLabel = enve->NewStringUTF(convertEbuToUtf(srv->getServiceShortLabel()).c_str());
        } else {
            dabServiceLabel = enve->NewStringUTF(srv->getServiceLabel().c_str());
            dabServiceShortLabel = enve->NewStringUTF(srv->getServiceShortLabel().c_str());
        }

        enve->CallVoidMethod(dabServiceObject, m_dabServiceSetServiceLabelMId, dabServiceLabel);
        enve->CallVoidMethod(dabServiceObject, m_dabServiceSetServiceShortLabelMId, dabServiceShortLabel);
        enve->CallVoidMethod(dabServiceObject, m_dabServiceSetServiceIdMId, srv->getServiceId());

        if(srv->isProgrammeService() > 0) {
            enve->CallVoidMethod(dabServiceObject, m_dabServiceSetServiceIsProgrammeMId, JNI_TRUE);
        } else {
            enve->CallVoidMethod(dabServiceObject, m_dabServiceSetServiceIsProgrammeMId, JNI_FALSE);
        }

        jstring genrePty = enve->NewStringUTF(srv->getProgrammeTypeFullName().data());
        jobject termIdObject = enve->NewObject(m_termIdClass, m_termIdConstructorMId);

        enve->CallVoidMethod(termIdObject, m_termIdSetGenreTextMId, genrePty);
        enve->CallVoidMethod(dabServiceObject, m_dabServiceAddGenreTermIdMId, termIdObject);
        enve->DeleteLocalRef(genrePty);

        enve->DeleteLocalRef(termIdObject);

        //DABServiceComponent creation
        for(const auto& srvComp : srv->getServiceComponents()) {
            jobject dabServiceComponentObject = enve->NewObject(m_dabServiceComponentClass, m_dabServiceComponentConstructorMId);

            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetBitrateMId, (jint)srvComp->getSubchannelBitrate());
            //enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetCaFlagMId, static_cast<jboolean>(srvComp->isCaApplied()));
            if(srvComp->isCaApplied() > 0) {
                enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetCaFlagMId, JNI_TRUE);
            } else {
                enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetCaFlagMId, JNI_FALSE);
            }

            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetServiceIdMId, (jint)srv->getServiceId());
            enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetSubchannelIdMId, srvComp->getSubChannelId());

            jstring dabServiceComponentLabel;
            if(srvComp->getLabelCharset() == 0) {
                dabServiceComponentLabel = enve->NewStringUTF(convertEbuToUtf(srvComp->getServiceComponentLabel()).c_str());
            } else {
                dabServiceComponentLabel = enve->NewStringUTF(srvComp->getServiceComponentLabel().c_str());
            }

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
                    dgUsed = packetComp->isDataGroupTransportUsed() ? 1 : 0;
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
            //enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetIsPrimaryMId, static_cast<jboolean>(srvComp->isPrimary()));
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
            //enve->CallVoidMethod(dabServiceComponentObject, m_dabServiceComponentSetIsFecAppliedMId, static_cast<jboolean>(srvComp->isFecSchemeApplied()));
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
                    std::cout << m_logTag << "Filtering DLS Userapplication" << std::endl;
                    continue;
                }

                jobject dabServiceUserapplicationObject = enve->NewObject(m_dabServiceUserApplicationClass, m_dabServiceUserApplicationConstructorMId);
                enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetAppTypeMId, (jint)uApp.getUserApplicationType());
                //enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetIsCaAppliedMId, uApp.isCaApplied() ? 1 : 0);
                if(uApp.isCaApplied() > 0) {
                    enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetIsCaAppliedMId, JNI_TRUE);
                } else {
                    enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetIsCaAppliedMId, JNI_FALSE);
                }

                enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetCaOrgMId, (jint)uApp.getCaOrganization());
                //enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetIsXPadMId, uApp.isXpadApp() ? 1 : 0);
                if(uApp.isXpadApp() > 0) {
                    enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetIsXPadMId, JNI_TRUE);
                } else {
                    enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetIsXPadMId, JNI_FALSE);
                }

                enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetXPadAppTypeMId, (jint)uApp.getXpadAppType());

                //enve->CallVoidMethod(dabServiceUserapplicationObject, m_dabServiceUserApplicationSetIsDgUsedMId, uApp.dataGroupsUsed() ? 1 : 0);
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
            enve->CallVoidMethod(dabServiceObject, m_dabServiceAddServiceComponentMId, dabServiceComponentObject);

            enve->DeleteLocalRef(dabServiceComponentObject);
        }
        //DABServiceComponent creation END

        enve->DeleteLocalRef(dabServiceLabel);
        enve->DeleteLocalRef(dabServiceShortLabel);

        enve->CallVoidMethod(m_usbTunerObject, m_usbTunerServiceFoundMId, dabServiceObject);

        enve->DeleteLocalRef(dabServiceObject);
    }

    enve->DeleteLocalRef(ensembleLabel);
    enve->DeleteLocalRef(ensembleShortLabel);

    if(wasDetached) {
        m_javaVm->DetachCurrentThread();
    }
}

void JTunerUsbDevice::serviceStarted(jobject dabService) {
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

    enve->CallVoidMethod(m_usbTunerObject, m_usbTunerServiceStartedMId, dabService);

    if(wasDetached) {
        m_javaVm->DetachCurrentThread();
    }
}

void JTunerUsbDevice::serviceStopped(jobject dabService) {
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

    enve->CallVoidMethod(m_usbTunerObject, m_usbTunerServiceStoppedMId, dabService);

    if(wasDetached) {
        m_javaVm->DetachCurrentThread();
    }
}

void JTunerUsbDevice::receptionStatistics(bool rfLock, int qual) {
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

    enve->CallVoidMethod(m_usbTunerObject, m_usbTunerReceptionStatisticsMId, (jboolean)rfLock, (jint)qual);

    if(wasDetached) {
        m_javaVm->DetachCurrentThread();
    }
}