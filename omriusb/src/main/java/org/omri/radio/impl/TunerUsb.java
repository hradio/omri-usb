package org.omri.radio.impl;

import android.hardware.usb.UsbDevice;

import org.omri.radioservice.RadioServiceDab;
import org.omri.tuner.Tuner;

/**
 * Copyright (C) 2018 IRT GmbH
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * @author Fabian Sattler, IRT GmbH
 */

interface TunerUsb extends Tuner {

	UsbDevice getUsbDevice();

	void callBack(int callbackType);

	void scanProgressCallback(int percentDone);

	void serviceFound(RadioServiceDab service);

	void serviceStarted(RadioServiceDab startedService);

	void serviceStopped(RadioServiceDab stoppedService);

	void receptionStatistics(boolean rfLocked, int rssi);
}
