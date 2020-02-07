package org.omri.radio.impl;

import android.os.Build;
import android.util.Log;

import org.omri.BuildConfig;
import org.omri.radio.Radio;
import org.omri.radioservice.metadata.VisualType;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

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

public class VisualLogoImpl extends VisualImpl implements Serializable {

	private static final long serialVersionUID = -1520291958767885960L;

	private static final String TAG = "VisualLogoImpl";

	private String mLogoFileName = null;
	private String mLogoUrl = null;

	private List<RadioDnsEpgBearer> mBearers = new ArrayList<>();

	@Override
	public VisualType getVisualType() {
		return VisualType.METADATA_VISUAL_TYPE_RADIODNS_RADIOVIS;
	}

	void setFilePath(String logoFileName) {
		mLogoFileName = logoFileName;
	}

	String getFilePath() {
		return mLogoFileName;
	}

	void setLogoUrl(String logoUrl) {
		mLogoUrl = logoUrl;
	}

	String getLogoUrl() {
		return mLogoUrl;
	}

	boolean isAvailable() {
		return mLogoFileName != null && new File(((RadioImpl) Radio.getInstance()).mContext.getCacheDir().getAbsolutePath() + "/logofiles_cache/" + mLogoFileName).exists();
	}

	void addBearer(RadioDnsEpgBearer bearer) {
		if(bearer != null) {
			if(BuildConfig.DEBUG)Log.d(TAG, "Adding Bearer: " + bearer.getBearerId());
			mBearers.add(bearer);
		}
	}

	void addBearer(List<RadioDnsEpgBearer> bearerList) {
		if(BuildConfig.DEBUG) {
			for(RadioDnsEpgBearer bearer : bearerList) {
				Log.d(TAG, "Adding Bearer: " + bearer.getBearerId());
			}
		}
		mBearers.addAll(bearerList);
	}

	List<RadioDnsEpgBearer> getBearers() {
		return mBearers;
	}

	@Override
	public byte[] getVisualData() {
		if(mLogoFileName != null) {
			if(BuildConfig.DEBUG) Log.d(TAG, "Reading file to bytearray: " + (((RadioImpl) Radio.getInstance()).mContext.getCacheDir().getAbsolutePath() + "/logofiles_cache/" + mLogoFileName));
			File file = new File(((RadioImpl) Radio.getInstance()).mContext.getCacheDir().getAbsolutePath() + "/logofiles_cache/" + mLogoFileName);
			int size = (int) file.length();
			byte[] bytes = new byte[size];

			try {
				BufferedInputStream buf = new BufferedInputStream(new FileInputStream(file));
				buf.read(bytes, 0, bytes.length);
				buf.close();
			} catch (FileNotFoundException e) {
				e.printStackTrace();
				bytes = new byte[0];
			} catch (IOException e) {
				e.printStackTrace();
				bytes = new byte[0];
			}

			return bytes;
		}

		return new byte[0];
	}

	@Override
	public boolean equals(Object obj) {
		if(obj != null) {
			if(obj instanceof VisualLogoImpl) {
				VisualLogoImpl visObj = (VisualLogoImpl)obj;
				return this.mBearers.size() == visObj.getBearers().size() && this.mBearers.containsAll(visObj.getBearers()) && this.mLogoUrl.equals(visObj.getLogoUrl()) && this.getVisualWidth() == visObj.getVisualWidth() && this.getVisualHeight() == visObj.getVisualHeight() && this.getVisualMimeType() == visObj.getVisualMimeType();
			}
		}

		return false;
	}

	@Override
	public int hashCode() {
		if(android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
			return Objects.hash(this.mBearers, this.mLogoUrl, this.getVisualWidth(), this.getVisualHeight(), this.getVisualMimeType().getMimeTypeString());
		} else {
			int hash = 27;
			hash = 31 * hash + this.mBearers.hashCode();
			hash = 31 * hash + this.mLogoUrl.hashCode();
			hash = 31 * hash + (int)(this.getVisualWidth() ^ (this.getVisualWidth() >>> 32));
			hash = 31 * hash + (int)(this.getVisualHeight() ^ (this.getVisualHeight() >>> 32));
			hash = 31 * hash + this.getVisualMimeType().getMimeTypeString().hashCode();

			return hash;
		}
	}
}
