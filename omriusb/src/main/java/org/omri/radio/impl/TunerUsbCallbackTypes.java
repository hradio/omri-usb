package org.omri.radio.impl;

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

public enum TunerUsbCallbackTypes {

	TUNER_CALLBACK_UNKNOWN(-1),
	TUNER_READY(0),
	TUNER_FAILED(1),
	TUNER_FREQUENCY_LOCKED(2),
	TUNER_FREQUENCY_NOT_LOCKED(3),
	TUNER_SCAN_IN_PROGRESS(4);

	private final int mIntType;

	TunerUsbCallbackTypes(int type) {
		mIntType = type;
	}

	public static TunerUsbCallbackTypes getTypeByValue(int value) {
		for(TunerUsbCallbackTypes cbType : TunerUsbCallbackTypes.values()) {
			if(cbType.getIntValue() == value) {
				return cbType;
			}
		}

		return TUNER_CALLBACK_UNKNOWN;
	}

	public int getIntValue() {
		return mIntType;
	}
}
