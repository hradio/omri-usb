package org.omri.radio.impl;

import android.util.Log;

import org.omri.BuildConfig;

import eu.hradio.httprequestwrapper.dtos.service_search.Bearer;
import eu.hradio.httprequestwrapper.dtos.service_search.RankedStandaloneService;
import eu.hradio.httprequestwrapper.dtos.service_search.ServiceList;
import eu.hradio.httprequestwrapper.exception.HRadioSearchClientException;
import eu.hradio.httprequestwrapper.listener.OnErrorListener;
import eu.hradio.httprequestwrapper.listener.OnSearchResultListener;
import eu.hradio.httprequestwrapper.service.ServiceSearchClient;
import eu.hradio.httprequestwrapper.service.ServiceSearchClientImpl;

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
class HradioServiceScanner {

	private final static String TAG = "HradioServiceScanner";

	private static final HradioServiceScanner mScannerInstance = new HradioServiceScanner();

	private final ServiceSearchClient mSearchClient;

	private boolean mIsScanning = false;

	private HradioServiceScanner() {
		mSearchClient = new ServiceSearchClientImpl();
	}

	static HradioServiceScanner getInstance() {
		return mScannerInstance;
	}

	void scanServices() {
		if(!mIsScanning) {
			mIsScanning = true;

			mSearchClient.asyncGetAllServices(new OnSearchResultListener<ServiceList>() {
				@Override
				public void onResult(ServiceList serviceList) {
					if(BuildConfig.DEBUG) Log.d(TAG, "HRadioHttpClient onResult, " + serviceList.getSize() + " Services found");
					for(RankedStandaloneService rankedSrv : serviceList.getContent()) {

						if(BuildConfig.DEBUG) Log.d(TAG, "HRadioHttpClient Service: " + rankedSrv.getContent().getName() + ", " + rankedSrv.getContent().getId() + ", Source: " + rankedSrv.getSource() + ", Score: " + rankedSrv.getScore());
						for(Bearer bearer : rankedSrv.getContent().getBearers()) {
							if(BuildConfig.DEBUG) Log.d(TAG, "HRadioHttpClient Bearer Type: " + bearer.getType().toString() + ", Address: " + bearer.getAddress() + ", MIME: " + bearer.getMimeType() + ", Bitrate: " + bearer.getBitrate());
						}
					}

					mIsScanning = false;
				}
			}, new OnErrorListener() {
				@Override
				public void onError(HRadioSearchClientException e) {
					if(BuildConfig.DEBUG) Log.e(TAG, "HRadioHttpClient onError: " + e.getMessage()); e.printStackTrace();

					mIsScanning = false;
				}
			});
		}
	}
}
