package org.omri.radio.impl;

import org.omri.radioservice.metadata.Visual;
import org.omri.radioservice.metadata.VisualMimeType;

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

public abstract class VisualImpl implements Visual, Serializable {

	private static final long serialVersionUID = -6792542766032172959L;

	private VisualMimeType mMimeType = VisualMimeType.METADATA_VISUAL_MIMETYPE_UNKNOWN;
	private int mWidth = -1;
	private int mHeight = -1;
	private byte[] mImageData;

	@Override
	public VisualMimeType getVisualMimeType() {
		return mMimeType;
	}
	
	void setVisualMimeType(VisualMimeType mimeType) {
		this.mMimeType = mimeType;
	}
	
	public void setVisualMimeType(int mimeTypeId) {
		if(mimeTypeId <= VisualMimeType.values().length) {
			this.mMimeType = VisualMimeType.values()[mimeTypeId];
		}
	}

	@Override
	public byte[] getVisualData() {
		return mImageData;
	}

	void setVisualData(byte[] visData) {
		this.mImageData = visData;
	}

	@Override
	public int getVisualHeight() {
		return mHeight;
	}

	void setHeight(int height) {
		mHeight = height;
	}

	@Override
	public int getVisualWidth() {
		return mWidth;
	}

	void setWidth(int width) {
		mWidth = width;
	}
}
