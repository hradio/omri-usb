package org.omri.radio.impl;

import com.google.android.exoplayer2.upstream.HttpDataSource;
import com.google.android.exoplayer2.upstream.HttpDataSource.BaseFactory;
import com.google.android.exoplayer2.util.Predicate;

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

public class IcyHttpDataSourceFactory extends BaseFactory {

	private final String mUserAgent;
	private final IcyStreamDataSource.IcyMetadataListener mListener;
	private final IcyStreamDataSource.RawStreamDataListener mRawListener;

	IcyHttpDataSourceFactory(String userAgent, IcyStreamDataSource.IcyMetadataListener listener, IcyStreamDataSource.RawStreamDataListener rawListener, Predicate<String> contentTypePredicate) {
		mUserAgent = userAgent;
		mListener = listener;

		mRawListener = rawListener;
	}

	@Override
	protected HttpDataSource createDataSourceInternal(HttpDataSource.RequestProperties defaultRequestProperties) {
		IcyStreamDataSource icyStream = new IcyStreamDataSource(mUserAgent, null);
		icyStream.addIcyMetaListener(mListener);
		if(mRawListener != null) {
			icyStream.addRawStreamDataListener(mRawListener);
		}
		return icyStream;
	}

}
