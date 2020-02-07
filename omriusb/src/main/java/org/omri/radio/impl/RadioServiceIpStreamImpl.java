package org.omri.radio.impl;

import org.omri.radioservice.RadioServiceIpStream;
import org.omri.radioservice.RadioServiceMimeType;

import java.io.Serializable;

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

public class RadioServiceIpStreamImpl implements RadioServiceIpStream, Serializable {

	private static final long serialVersionUID = 9074820190262630177L;

	private String mStreamUrl;
	private int mBitrate;
	private RadioServiceMimeType mMimeType;
	private int mCost = 0;
	private int mOffset = 0;

	@Override
	public String getUrl() {
		return mStreamUrl;
	}

	@Override
	public int getBitrate() {
		return mBitrate;
	}

	@Override
	public RadioServiceMimeType getMimeType() {
		return mMimeType;
	}

	@Override
	public int getCost() {
		return mCost;
	}

	@Override
	public int getOffset() {
		return mOffset;
	}

	void setStreamUrl(String streamUrl) {
		mStreamUrl = streamUrl;
	}

	void setBitrate(int bitrate) {
		mBitrate = bitrate;
	}

	void setMimeType(RadioServiceMimeType mime) {
		mMimeType = mime;
	}

	void setCost(int cost) {
		mCost = cost;
	}

	void setOffset(int off){
		mOffset = off;
	}
}
