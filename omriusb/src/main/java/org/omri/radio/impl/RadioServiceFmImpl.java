package org.omri.radio.impl;

import java.io.Serializable;

import org.omri.radioservice.RadioService;
import org.omri.radioservice.RadioServiceFm;
import org.omri.radioservice.RadioServiceFmPty;
import org.omri.radioservice.RadioServiceType;

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

public class RadioServiceFmImpl extends RadioServiceImpl implements RadioServiceFm, Serializable {

	private static final long serialVersionUID = -3679409912025169646L;
	
	private int mFrequency = -1;
	private int mPiCode = -1;
	private RadioServiceFmPty mPty = null;
	
	@Override
	public RadioServiceType getRadioServiceType() {
		return RadioServiceType.RADIOSERVICE_TYPE_FM;
	}

	@Override
	public int getFrequency() {
		return mFrequency;
	}

	@Override
	public int getRdsPiCode() {
		return mPiCode;
	}

	@Override
	public RadioServiceFmPty getRdsPty() {
		return mPty;
	}

	@Override
	public String getServiceLabel() {
		return "";
	}

	void setFrequency(int frequency) {
		mFrequency = frequency;
	}
	
	void setPiCode(int piCode) {
		mPiCode = piCode;
	}

	void setPty(RadioServiceFmPty pty) {
		mPty = pty;
	}

	@Override
	public boolean equalsRadioService(RadioService otherSrv) {
		return false;
	}
}
