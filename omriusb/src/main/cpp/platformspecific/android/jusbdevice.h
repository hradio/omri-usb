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

#ifndef CMAKETEST_JUSBDEVICE_H
#define CMAKETEST_JUSBDEVICE_H

#include <jni.h>
#include <string>
#include <functional>
#include <vector>
#include <map>

class JUsbDevice {

public:
    explicit JUsbDevice(JavaVM* javaVm, JNIEnv* env, jobject usbDevice);
    virtual ~JUsbDevice();

    virtual std::string getDeviceName() const;
    virtual uint16_t getProductId() const;
    virtual uint16_t getVendorId() const;

    virtual bool isPermissionGranted() const;

    virtual void permissionGranted(JNIEnv *env, bool granted);

    using PermissionCallbackFunction = std::function<void(const bool permissionGranted)>;
    virtual void requestPermission(PermissionCallbackFunction permissionCallback);

    virtual int writeBulkTransferData(uint8_t endPointAddress, const std::vector<uint8_t>& buffer, int timeOutMs = 5000);
    virtual int readBulkTransferData(uint8_t endPointAddress, std::vector<uint8_t>& buffer, int timeOutMs = 5000);

private:
    JavaVM* m_javaVm;
    JNIEnv* m_env;
    jobject m_usbDeviceObject;
    jobject m_usbDeviceConnectionObject;

    std::string m_usbDeviceName;
    uint16_t m_vendorId{0xFFFF};
    uint16_t m_productId{0xFFFF};
    bool m_permissionGranted{false};

    std::map<uint8_t, jobject> m_endpointsMap;

    PermissionCallbackFunction m_permissionCallback;

    uint8_t m_interfaceNum{0};

private:
    jclass m_usbHelperClass;
    jmethodID m_usbHelperGetInstanceMId;
    jmethodID m_usbHelperRequestPermissionMId;
    jmethodID m_usbHelperOpenDeviceMId;

    jclass m_usbDeviceClass;
    jmethodID m_usbDeviceGetNameMId;
    jmethodID m_usbDeviceGetProductIdMId;
    jmethodID m_usbDeviceGetVendorIdMId;
    //Min API >= 21
    //jmethodID m_usbDeviceGetManufacturerNameMId;
    //jmethodID m_usbDeviceGetSerialNumberMId;
    //jmethodID m_usbDeviceGetVersionMId;

    jmethodID m_usbDeviceGetInterfaceCountMId;
    jmethodID m_usbDeviceGetInterfaceMId;

    jclass m_usbDeviceConnectionClass;
    jmethodID m_usbDeviceConnectionClaimInterfaceMId;
    jmethodID m_usbDeviceConnectionReleaseInterfaceMId;
    jmethodID m_usbDeviceConnectionBulkTransferMId;
    jmethodID m_usbDeviceConnectionBulkTransferWithOffsetMId;

    jclass m_usbDeviceInterfaceClass;
    jmethodID m_usbDeviceInterfaceGetEndpointCountMId;
    jmethodID m_usbDeviceInterfaceGetEndpointMId;

    jclass m_usbDeviceEndpointClass;
    jmethodID m_usbDeviceEndpointGetEndpointNumberMId;
    jmethodID m_usbDeviceEndpointGetEndpointAddressMId;
    jmethodID m_usbDeviceEndpointGetDirectionMId;
    jmethodID m_usbDeviceEndpointGetIntervalMId;
};


#endif //CMAKETEST_JUSBDEVICE_H
