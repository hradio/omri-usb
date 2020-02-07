package org.omri.radio.impl;

import android.support.annotation.NonNull;
import android.util.Base64;
import android.util.Log;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.omri.radio.Radio;
import org.omri.radioservice.RadioService;
import org.omri.radioservice.RadioServiceDabComponent;
import org.omri.radioservice.RadioServiceDabUserApplication;
import org.omri.radioservice.RadioServiceIpStream;
import org.omri.radioservice.RadioServiceMimeType;
import org.omri.radioservice.RadioServiceType;
import org.omri.radioservice.metadata.TermId;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;

import eu.hradio.core.radiodns.radioepg.radiodns.RadioDns;

import static org.omri.BuildConfig.DEBUG;

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

class RadioServiceManager implements org.omri.radio.RadioServiceManager {

	private final static String TAG = "RadioServiceManager";

	private final static RadioServiceManager INSTANCE = new RadioServiceManager();

	private ConcurrentHashMap<RadioServiceType, CopyOnWriteArrayList<RadioService>> mServicesMap = new ConcurrentHashMap<>();
	private ConcurrentHashMap<RadioServiceType, Boolean> mServicesDeSerializingInProgress = new ConcurrentHashMap<>();

	private final String SERVICES_DIR;
	private final String SERVICES_JSON_DAB;
	private final String SERVICES_JSON_IP;
	private final String SERVICES_JSON_EDI;

	private boolean mFirstInitDab = true;
	private boolean mFirstInitEdi = true;
	private boolean mFirstInitIp = true;

	private RadioServiceManager() {
		if (((RadioImpl) Radio.getInstance()).mContext != null) {
			SERVICES_DIR = ((RadioImpl) Radio.getInstance()).mContext.getFilesDir() + "/services/";
			SERVICES_JSON_DAB = SERVICES_DIR + "dabservices.json";
			SERVICES_JSON_IP = SERVICES_DIR + "ipservices.json";
			SERVICES_JSON_EDI = SERVICES_DIR + "ediservices.json";
		} else {
			SERVICES_DIR = null;
			SERVICES_JSON_DAB = null;
			SERVICES_JSON_IP = null;
			SERVICES_JSON_EDI = null;
		}

		File servicesDir = new File(SERVICES_DIR);
		if (!servicesDir.exists()) {
			boolean dirCreated = servicesDir.mkdirs();
			if (DEBUG) Log.d(TAG, "Services dir created: " + dirCreated);
		}

		mServicesMap.put(RadioServiceType.RADIOSERVICE_TYPE_DAB, new CopyOnWriteArrayList<RadioService>());
		mServicesMap.put(RadioServiceType.RADIOSERVICE_TYPE_IP, new CopyOnWriteArrayList<RadioService>());
		mServicesMap.put(RadioServiceType.RADIOSERVICE_TYPE_EDI, new CopyOnWriteArrayList<RadioService>());

		mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_DAB, true);
		mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_IP, true);
		mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_EDI, true);

		new Thread(new Runnable() {
			@Override
			public void run() {
				deSerializeIpServices();
				deserializeDabServices();
				deSerializeEdiServices();
			}
		}).start();
	}

	static RadioServiceManager getInstance() {
		return INSTANCE;
	}

	final boolean isServiceListReady(RadioServiceType type) {
		Boolean listInProgress = mServicesDeSerializingInProgress.get(type);
		if (listInProgress != null) {
			return !listInProgress;
		}

		return false;
	}

	void addService(@NonNull RadioService addSrv) {
		CopyOnWriteArrayList<RadioService> addList = mServicesMap.get(addSrv.getRadioServiceType());
		if (addList != null) {
			//TODO remove service and add new one for update?
			boolean oldSrvRemoved = addList.remove(addSrv);
			if (DEBUG && oldSrvRemoved) {
				Log.d(TAG, "Removed old version of service: " + addSrv.getServiceLabel() + " : " + addSrv.getRadioServiceType().toString());
			}
			boolean addedSrv = addList.add(addSrv);
			if (DEBUG && addedSrv) { Log.d(TAG, "Added new service: " + addSrv.getServiceLabel() + " : " + addSrv.getRadioServiceType().toString()); }
		} else {
			if (DEBUG) Log.d(TAG, "Adding unknown ServiceType: " + addSrv.getRadioServiceType().toString());
		}
	}

	boolean addRadioservice(RadioService addSrv) {
		CopyOnWriteArrayList<RadioService> addList = mServicesMap.get(addSrv.getRadioServiceType());
		if (addList != null) {
			//TODO remove service and add new one for update?
			boolean oldSrvRemoved = addList.remove(addSrv);
			if (DEBUG && oldSrvRemoved) {
				Log.d(TAG, "Removed old version of service: " + addSrv.getServiceLabel() + " : " + addSrv.getRadioServiceType().toString());
			}

			if (DEBUG) { Log.d(TAG, "Added new service: " + addSrv.getServiceLabel() + " : " + addSrv.getRadioServiceType().toString()); }

			return addList.add(addSrv);
		}
		return false;
	}

	void clearServiceList(@NonNull RadioServiceType type) {
		CopyOnWriteArrayList<RadioService> addList = mServicesMap.get(type);
		if (addList != null) {
			addList.clear();
		}
	}

	void serializeServices(@NonNull RadioServiceType type) {
		switch (type) {
			case RADIOSERVICE_TYPE_DAB:
				mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_DAB, true);
				serializeDabServices();
				break;
			case RADIOSERVICE_TYPE_IP:
				mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_IP, true);
				serializeIpServices();
				break;
			case RADIOSERVICE_TYPE_EDI:
				mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_EDI, true);
				serializeEdiServices();
				break;
			case RADIOSERVICE_TYPE_FM:
			case RADIOSERVICE_TYPE_SIRIUS:
			case RADIOSERVICE_TYPE_HDRADIO:
			default:
				break;
		}
	}

	private void writeServicesFile(String fileNamePath, String servicesJson) throws IOException {
		BufferedWriter srvListWriter = null;
		try {
			//save to file
			if (DEBUG) Log.d(TAG, "Serializing SrvListJson writing to file: " + fileNamePath);

			File srvListFile = new File(fileNamePath);
			srvListWriter = new BufferedWriter(new FileWriter(srvListFile));
			srvListWriter.write(servicesJson);
			srvListWriter.close();
		} catch (IOException ioExc) {
			if (DEBUG) ioExc.printStackTrace();
			throw ioExc;
		} finally {
			if (srvListWriter != null) {
				try {
					srvListWriter.close();
				} catch (IOException ioExc) {
					if (DEBUG) ioExc.printStackTrace();
				}
			}
		}
	}

	private String readServiceFile(String filePathName) throws FileNotFoundException, IOException {
		File savedSrvFile = new File(filePathName);
		if (savedSrvFile.exists()) {

			FileInputStream srvJsonInputStream = null;
			BufferedReader srvJsonReader = null;
			try {
				srvJsonInputStream = new FileInputStream(savedSrvFile);
				srvJsonReader = new BufferedReader(new InputStreamReader(srvJsonInputStream));
				StringBuilder srvJsonBuilder = new StringBuilder();

				char[] readBuf = new char[4096];
				int bytesRead = 0;
				while ((bytesRead = srvJsonReader.read(readBuf)) != -1) {
					srvJsonBuilder.append(readBuf, 0, bytesRead);
				}

				return srvJsonBuilder.toString();
			} finally {
				try {
					if (srvJsonInputStream != null) {
						srvJsonInputStream.close();
					}
					if (srvJsonReader != null) {
						srvJsonReader.close();
					}
				} catch (IOException ioExc) {
					if (DEBUG) ioExc.printStackTrace();
				}
			}
		}

		return null;
	}

	private void serializeIpServices() {
		if (SERVICES_JSON_IP != null) {
			mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_IP, true);

			CopyOnWriteArrayList<RadioService> serialisingServices = mServicesMap.get(RadioServiceType.RADIOSERVICE_TYPE_IP);
			if (serialisingServices != null) {
				JSONArray srvFileArr = new JSONArray();
				for (RadioService saveSrv : serialisingServices) {
					RadioServiceIpImpl ipSaveSrv = (RadioServiceIpImpl) saveSrv;
					JSONObject saveSrvObj = new JSONObject();
					try {
						saveSrvObj.put("serviceLabel", ipSaveSrv.getServiceLabel());

						JSONArray bearersArr = new JSONArray();
						for (RadioDnsEpgBearer ipSrvBearer : ipSaveSrv.getBearers()) {
							JSONObject ipSrvBearerObj = new JSONObject();

							ipSrvBearerObj.put("mimeValue", ipSrvBearer.getMimeValue());
							ipSrvBearerObj.put("bearerId", ipSrvBearer.getBearerId());
							ipSrvBearerObj.put("bearerType", ipSrvBearer.getBearerType().toString());
							ipSrvBearerObj.put("bitrate", ipSrvBearer.getBitrate());
							ipSrvBearerObj.put("cost", ipSrvBearer.getCost());

							bearersArr.put(ipSrvBearerObj);
						}
						saveSrvObj.put("bearers", bearersArr);

						JSONArray ipStreamsArr = new JSONArray();
						for (RadioServiceIpStream ipStream : ipSaveSrv.getIpStreams()) {
							JSONObject ipStreamObj = new JSONObject();

							ipStreamObj.put("bitrate", ipStream.getBitrate());
							ipStreamObj.put("cost", ipStream.getCost());
							ipStreamObj.put("mimeType", ipStream.getMimeType().getMimeTypeString());
							ipStreamObj.put("offset", ipStream.getOffset());
							ipStreamObj.put("url", ipStream.getUrl());

							ipStreamsArr.put(ipStreamObj);
						}
						saveSrvObj.put("ipStreams", ipStreamsArr);

						if (ipSaveSrv.getRadioDns() != null) {
							JSONObject ipSrvRdnsObj = new JSONObject();
							ipSrvRdnsObj.put("fqdn", ipSaveSrv.getRadioDns().getFqdn());
							ipSrvRdnsObj.put("srvId", ipSaveSrv.getRadioDns().getServiceIdentifier());
							saveSrvObj.put("radiodns", ipSrvRdnsObj);
						}

						JSONArray genreArr = new JSONArray();
						for (TermId termId : ipSaveSrv.getGenres()) {
							JSONObject termIdObj = new JSONObject();
							termIdObj.put("termId", termId.getTermId());
							termIdObj.put("termHref", termId.getGenreHref());
							termIdObj.put("termText", termId.getText());

							genreArr.put(termIdObj);
						}
						saveSrvObj.put("genres", genreArr);

						JSONArray keyWordsArr = new JSONArray();
						for (String keyWord : ipSaveSrv.getKeywords()) {
							keyWordsArr.put(keyWord);
						}
						saveSrvObj.put("keywords", keyWordsArr);

						JSONArray linkArr = new JSONArray();
						for (String link : ipSaveSrv.getLinks()) {
							linkArr.put(link);
						}
						saveSrvObj.put("links", linkArr);

						saveSrvObj.put("longDescription", ipSaveSrv.getLongDescription());
						saveSrvObj.put("shortDescription", ipSaveSrv.getShortDescription());

						srvFileArr.put(saveSrvObj);
					} catch (JSONException jsonExc) {
						if (DEBUG) jsonExc.printStackTrace();
					}
				}

				try {
					writeServicesFile(SERVICES_JSON_IP, srvFileArr.toString(2));
				} catch (JSONException jsonExc) {
					if (DEBUG) jsonExc.printStackTrace();
				} catch (IOException ioExc) {
					if (DEBUG) ioExc.printStackTrace();
				} finally {
					mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_IP, false);
				}

				if (DEBUG) Log.d(TAG, "Serializing IP SrvListJson done!");
			}
		}
	}

	private void deSerializeIpServices() {
		if (SERVICES_JSON_IP != null) {
			Boolean deserInProgress = mServicesDeSerializingInProgress.get(RadioServiceType.RADIOSERVICE_TYPE_IP);
			if (deserInProgress != null && deserInProgress && !mFirstInitIp) {
				if (DEBUG) Log.w(TAG, "Deserializing IP services already in progress");
				return;
			}

			mFirstInitIp = false;

			try {
				String jsonArrString = readServiceFile(SERVICES_JSON_IP);
				if (jsonArrString != null) {
					mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_IP, true);

					ArrayList<RadioService> tempAddList = new ArrayList<>();

					JSONArray srvListArr = new JSONArray(jsonArrString);
					if (DEBUG) Log.d(TAG, "Read IpSrvListJson length: " + srvListArr.length());
					for (int i = 0; i < srvListArr.length(); i++) {
						JSONObject srvObj = srvListArr.getJSONObject(i);

						RadioServiceIpImpl ipSrv = new RadioServiceIpImpl();
						ipSrv.setServiceLabel(srvObj.getString("serviceLabel"));

						JSONArray bearersArr = srvObj.getJSONArray("bearers");
						for (int j = 0; j < bearersArr.length(); j++) {
							JSONObject bearerObj = bearersArr.getJSONObject(j);

							RadioDnsEpgBearerType bearerType = RadioDnsEpgBearerType.valueOf(bearerObj.getString("bearerType"));
							String bearerId = bearerObj.getString("bearerId");
							String mimeValue = bearerObj.getString("mimeValue");
							int bitrate = bearerObj.getInt("bitrate");
							int cost = bearerObj.getInt("cost");
							switch (bearerType) {
								case DAB:
									ipSrv.addBearer(new RadioDnsEpgBearerDab(bearerId, cost, mimeValue, bitrate));
									break;
								case IP_HTTP:
									ipSrv.addBearer(new RadioDnsEpgBearerIpHttp(bearerId, cost, mimeValue, bitrate));
									break;
								default:
									break;
							}
						}

						JSONArray ipStreamsArr = srvObj.getJSONArray("ipStreams");
						for (int j = 0; j < ipStreamsArr.length(); j++) {
							JSONObject ipStreamObj = ipStreamsArr.getJSONObject(j);

							RadioServiceIpStreamImpl ipStream = new RadioServiceIpStreamImpl();
							ipStream.setBitrate(ipStreamObj.getInt("bitrate"));
							ipStream.setCost(ipStreamObj.getInt("cost"));
							ipStream.setMimeType(RadioServiceMimeType.fromMimeValue(ipStreamObj.getString("mimeType")));
							ipStream.setOffset(ipStreamObj.getInt("offset"));
							ipStream.setStreamUrl(ipStreamObj.getString("url"));

							ipSrv.addStream(ipStream);
						}

						JSONObject rdnsOptObj = srvObj.optJSONObject("radiodns");
						if (rdnsOptObj != null) {
							String fqdn = rdnsOptObj.getString("fqdn");
							String sid = rdnsOptObj.getString("srvId");
							RadioDns rdns = new RadioDns(fqdn, sid);

							ipSrv.setRadioDns(rdns);
						}

						JSONArray genreArr = srvObj.getJSONArray("genres");
						for (int l = 0; l < genreArr.length(); l++) {
							JSONObject genreObj = genreArr.getJSONObject(l);

							TermIdImpl termId = new TermIdImpl();
							termId.setTermId(genreObj.getString("termId"));
							termId.setGenreHref(genreObj.getString("termHref"));
							termId.setGenreText(genreObj.getString("termText"));

							ipSrv.addGenre(termId);
						}

						JSONArray keyWordsArr = srvObj.getJSONArray("keywords");
						for (int l = 0; l < keyWordsArr.length(); l++) {
							ipSrv.addKeyword(keyWordsArr.getString(l));
						}

						JSONArray linksArr = srvObj.getJSONArray("links");
						for (int l = 0; l < linksArr.length(); l++) {
							ipSrv.addLink(linksArr.getString(l));
						}

						ipSrv.setLongDescription(srvObj.getString("longDescription"));
						ipSrv.setShortDescription(srvObj.getString("shortDescription"));

						tempAddList.add(ipSrv);
					}

					mServicesMap.put(RadioServiceType.RADIOSERVICE_TYPE_IP, new CopyOnWriteArrayList<>(tempAddList));
					if (DEBUG) Log.d(TAG, "Restoring IPSrvListJson done with " + tempAddList.size() + " services");
				}
			} catch (JSONException jsonExc) {
				if (DEBUG) jsonExc.printStackTrace();
			} catch (FileNotFoundException fnfExc) {
				if (DEBUG) fnfExc.printStackTrace();
			} catch (IOException ioExc) {
				if (DEBUG) ioExc.printStackTrace();
			} finally {
				mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_IP, false);

				if(DEBUG)Log.d(TAG, "IP List unlocked: " + mServicesDeSerializingInProgress.get(RadioServiceType.RADIOSERVICE_TYPE_IP));
			}
		}

		mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_IP, false);
	}

	private JSONObject createDabServiceObject(RadioServiceDabImpl service) throws JSONException {
		JSONObject saveSrvObj = new JSONObject();

		saveSrvObj.put("caId", service.getCaId());
		saveSrvObj.put("ensembleEcc", service.getEnsembleEcc());
		saveSrvObj.put("ensembleFrequency", service.getEnsembleFrequency());
		saveSrvObj.put("ensembleId", service.getEnsembleId());
		saveSrvObj.put("ensembleLabel", service.getEnsembleLabel());
		saveSrvObj.put("ensembleShortLabel", service.getEnsembleShortLabel());
		saveSrvObj.put("radioServiceType", service.getRadioServiceType().toString());
		saveSrvObj.put("serviceId", service.getServiceId());
		saveSrvObj.put("serviceLabel", service.getServiceLabel());
		saveSrvObj.put("serviceShortLabel", service.getShortLabel());
		saveSrvObj.put("isCaProtected", service.isCaProtected());
		saveSrvObj.put("isProgrammeService", service.isProgrammeService());
		JSONArray serviceComponentsArr = new JSONArray();
		for (RadioServiceDabComponent dabSrvComp : service.getServiceComponents()) {
			JSONObject dabSrvCompObj = new JSONObject();
			dabSrvCompObj.put("bitrate", dabSrvComp.getBitrate());
			dabSrvCompObj.put("label", dabSrvComp.getLabel());
			dabSrvCompObj.put("mscStartAddress", dabSrvComp.getMscStartAddress());
			dabSrvCompObj.put("packetAddress", dabSrvComp.getPacketAddress());
			dabSrvCompObj.put("protectionLevel", dabSrvComp.getProtectionLevel());
			dabSrvCompObj.put("protectionType", dabSrvComp.getProtectionType());
			dabSrvCompObj.put("scids", dabSrvComp.getServiceComponentIdWithinService());
			dabSrvCompObj.put("compType", dabSrvComp.getServiceComponentType());
			dabSrvCompObj.put("compServiceId", dabSrvComp.getServiceId());
			dabSrvCompObj.put("subChannelId", dabSrvComp.getSubchannelId());
			dabSrvCompObj.put("subChannelSize", dabSrvComp.getSubchannelSize());
			dabSrvCompObj.put("tmId", dabSrvComp.getTmId());
			dabSrvCompObj.put("uepTblIdx", dabSrvComp.getUepTableIndex());
			dabSrvCompObj.put("caApplied", dabSrvComp.isCaApplied());
			dabSrvCompObj.put("dgUsed", dabSrvComp.isDatagroupTransportUsed());
			dabSrvCompObj.put("fecApplied", dabSrvComp.isFecSchemeApplied());
			dabSrvCompObj.put("primary", dabSrvComp.isPrimary());

			JSONArray userAppsArr = new JSONArray();
			for (RadioServiceDabUserApplication uapp : dabSrvComp.getUserApplications()) {
				JSONObject uappObj = new JSONObject();
				uappObj.put("caOrg", uapp.getCaOrganization());
				uappObj.put("dscty", uapp.getDataServiceComponentType().getType());
				uappObj.put("uappType", uapp.getType().getType());
				uappObj.put("uappData", uapp.getUserApplicationData() != null ? new String(Base64.encode(uapp.getUserApplicationData(), Base64.NO_WRAP)) : "");
				uappObj.put("xpadAppType", uapp.getXpadAppType());
				uappObj.put("caProtected", uapp.isCaProtected());
				uappObj.put("dgUsed", uapp.isDatagroupTransportUsed());
				uappObj.put("isXpadApp", uapp.isXpadApptype());

				userAppsArr.put(uappObj);
			}
			dabSrvCompObj.put("userApplications", userAppsArr);

			serviceComponentsArr.put(dabSrvCompObj);
		}
		saveSrvObj.put("serviceComponents", serviceComponentsArr);

		JSONArray genreArr = new JSONArray();
		for (TermId termId : service.getGenres()) {
			JSONObject termIdObj = new JSONObject();
			termIdObj.put("termId", termId.getTermId());
			termIdObj.put("termHref", termId.getGenreHref());
			termIdObj.put("termText", termId.getText());

			genreArr.put(termIdObj);
		}
		saveSrvObj.put("genres", genreArr);

		JSONArray keyWordsArr = new JSONArray();
		for (String keyWord : service.getKeywords()) {
			keyWordsArr.put(keyWord);
		}
		saveSrvObj.put("keywords", keyWordsArr);

		JSONArray linkArr = new JSONArray();
		for (String link : service.getLinks()) {
			linkArr.put(link);
		}
		saveSrvObj.put("links", linkArr);

		//TODO
				/*
				JSONArray locationArr = new JSONArray();
				for(Location location : dabSaveSrv.getLocations()) {

				}
				*/

		//TODO
				/*
				dabSaveSrv.getMemberships()
				*/

		saveSrvObj.put("longDescription", service.getLongDescription());
		saveSrvObj.put("shortDescription", service.getShortDescription());

		return saveSrvObj;
	}

	private void serializeEdiServices() {
		if (SERVICES_JSON_EDI != null) {
			mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_EDI, true);

			CopyOnWriteArrayList<RadioService> serialisingServices = mServicesMap.get(RadioServiceType.RADIOSERVICE_TYPE_EDI);
			if (serialisingServices != null) {
				JSONArray srvFileArr = new JSONArray();
				for (RadioService saveSrv : serialisingServices) {
					RadioServiceDabEdiImpl ediSaveSrv = (RadioServiceDabEdiImpl) saveSrv;
					try {
						JSONObject dabJsonObj = createDabServiceObject(ediSaveSrv);
						dabJsonObj.put("ediurl", ediSaveSrv.getUrl());
						srvFileArr.put(dabJsonObj);
					} catch (JSONException jsonExc) {
						if (DEBUG) jsonExc.printStackTrace();
					}
				}

				try {
					writeServicesFile(SERVICES_JSON_EDI, srvFileArr.toString(2));
				} catch (JSONException jsonExc) {
					if (DEBUG) jsonExc.printStackTrace();
				} catch (IOException ioExc) {
					if (DEBUG) ioExc.printStackTrace();
				} finally {
					mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_EDI, false);
				}

				if (DEBUG) Log.d(TAG, "Serializing EDI SrvListJson done!");
			}
		}
	}

	private void serializeDabServices() {
		if (SERVICES_JSON_DAB != null) {
			mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_DAB, true);

			CopyOnWriteArrayList<RadioService> serialisingServices = mServicesMap.get(RadioServiceType.RADIOSERVICE_TYPE_DAB);
			if (serialisingServices != null) {
				JSONArray srvFileArr = new JSONArray();
				for (RadioService saveSrv : serialisingServices) {
					RadioServiceDabImpl dabSaveSrv = (RadioServiceDabImpl) saveSrv;
					try {
						srvFileArr.put(createDabServiceObject(dabSaveSrv));
					} catch (JSONException jsonExc) {
						if (DEBUG) jsonExc.printStackTrace();
					}
				}

				try {
					writeServicesFile(SERVICES_JSON_DAB, srvFileArr.toString(2));
				} catch (JSONException jsonExc) {
					if (DEBUG) jsonExc.printStackTrace();
				} catch (IOException ioExc) {
					if (DEBUG) ioExc.printStackTrace();
				} finally {
					mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_DAB, false);
				}

				if (DEBUG) Log.d(TAG, "Serializing DAB SrvListJson done!");
			}
		}
	}

	private void recreateDabService(JSONObject srvObj, RadioServiceDabImpl dabSrv) throws JSONException {
		dabSrv.setCaId(srvObj.getInt("caId"));
		dabSrv.setEnsembleEcc(srvObj.getInt("ensembleEcc"));
		dabSrv.setEnsembleFrequency(srvObj.getInt("ensembleFrequency"));
		dabSrv.setEnsembleId(srvObj.getInt("ensembleId"));
		dabSrv.setEnsembleLabel(srvObj.getString("ensembleLabel"));
		dabSrv.setEnsembleShortLabel(srvObj.getString("ensembleShortLabel"));
		dabSrv.setServiceId(srvObj.getInt("serviceId"));
		dabSrv.setServiceLabel(srvObj.getString("serviceLabel"));
		dabSrv.setShortLabel(srvObj.getString("serviceShortLabel"));
		dabSrv.setIsCaProtected(srvObj.getBoolean("isCaProtected"));
		dabSrv.setIsProgrammeService(srvObj.getBoolean("isProgrammeService"));

		JSONArray srvCompArr = srvObj.getJSONArray("serviceComponents");
		for (int j = 0; j < srvCompArr.length(); j++) {
			JSONObject dabSrvCompObj = srvCompArr.getJSONObject(j);

			RadioServiceDabComponentImpl dabSrvComp = new RadioServiceDabComponentImpl();
			dabSrvComp.setScBitrate(dabSrvCompObj.getInt("bitrate"));
			dabSrvComp.setScLabel(dabSrvCompObj.getString("label"));
			dabSrvComp.setMscStartAddress(dabSrvCompObj.getInt("mscStartAddress"));
			dabSrvComp.setPacketAddress(dabSrvCompObj.getInt("packetAddress"));
			dabSrvComp.setProtectionLevel(dabSrvCompObj.getInt("protectionLevel"));
			dabSrvComp.setProtectionType(dabSrvCompObj.getInt("protectionType"));
			dabSrvComp.setServiceComponentIdWithinService(dabSrvCompObj.getInt("scids"));
			dabSrvComp.setServiceComponentType(dabSrvCompObj.getInt("compType"));
			dabSrvComp.setServiceId(dabSrvCompObj.getInt("compServiceId"));
			dabSrvComp.setSubchannelId(dabSrvCompObj.getInt("subChannelId"));
			dabSrvComp.setSubchannelSize(dabSrvCompObj.getInt("subChannelSize"));
			dabSrvComp.setTmId(dabSrvCompObj.getInt("tmId"));
			dabSrvComp.setUepTableIndex(dabSrvCompObj.getInt("uepTblIdx"));
			dabSrvComp.setIsScCaFlagSet(dabSrvCompObj.getBoolean("caApplied"));
			dabSrvComp.setDatagroupTransportUsed(dabSrvCompObj.getBoolean("dgUsed"));
			dabSrvComp.setIsFecSchemeApplied(dabSrvCompObj.getBoolean("fecApplied"));
			dabSrvComp.setIsScPrimary(dabSrvCompObj.getBoolean("primary"));

			JSONArray uappsArr = dabSrvCompObj.getJSONArray("userApplications");
			for (int k = 0; k < uappsArr.length(); k++) {
				JSONObject uappObj = uappsArr.getJSONObject(k);

				RadioServiceDabUserApplicationImpl uapp = new RadioServiceDabUserApplicationImpl();
				uapp.setCaOrganization(uappObj.getInt("caOrg"));
				uapp.setDSCTy(uappObj.getInt("dscty"));
				uapp.setUserApplicationType(uappObj.getInt("uappType"));

				String uappDataString = uappObj.getString("uappData");
				if (!uappDataString.isEmpty()) {
					uapp.setUappdata(Base64.decode(uappDataString, Base64.NO_WRAP));
				}

				uapp.setXpadApptype(uappObj.getInt("xpadAppType"));
				uapp.setIsCaProtected(uappObj.getBoolean("caProtected"));
				uapp.setIsDatagroupsUsed(uappObj.getBoolean("dgUsed"));
				uapp.setIsXpadApptype(uappObj.getBoolean("isXpadApp"));

				dabSrvComp.addScUserApplication(uapp);
			}

			dabSrvComp.setPacketAddress(dabSrvCompObj.getInt("packetAddress"));
			dabSrvComp.setPacketAddress(dabSrvCompObj.getInt("packetAddress"));
			dabSrvComp.setPacketAddress(dabSrvCompObj.getInt("packetAddress"));

			dabSrv.addServiceComponent(dabSrvComp);
		}

		JSONArray genreArr = srvObj.getJSONArray("genres");
		for (int l = 0; l < genreArr.length(); l++) {
			JSONObject genreObj = genreArr.getJSONObject(l);

			TermIdImpl termId = new TermIdImpl();
			termId.setTermId(genreObj.getString("termId"));
			termId.setGenreHref(genreObj.getString("termHref"));
			termId.setGenreText(genreObj.getString("termText"));

			dabSrv.addGenre(termId);
		}

		JSONArray keyWordsArr = srvObj.getJSONArray("keywords");
		for (int l = 0; l < keyWordsArr.length(); l++) {
			dabSrv.addKeyword(keyWordsArr.getString(l));
		}

		JSONArray linksArr = srvObj.getJSONArray("links");
		for (int l = 0; l < linksArr.length(); l++) {
			dabSrv.addLink(linksArr.getString(l));
		}

		dabSrv.setLongDescription(srvObj.getString("longDescription"));
		dabSrv.setShortDescription(srvObj.getString("shortDescription"));
	}

	private void deSerializeEdiServices() {
		if (SERVICES_JSON_EDI != null) {
			Boolean deserInProgress = mServicesDeSerializingInProgress.get(RadioServiceType.RADIOSERVICE_TYPE_EDI);
			if (deserInProgress != null && deserInProgress && !mFirstInitEdi) {
				if (DEBUG) Log.w(TAG, "Deserializing EDI services already in progress");
				return;
			}

			mFirstInitEdi = false;

			try {
				String savedSrvFileString = readServiceFile(SERVICES_JSON_EDI);
				if (savedSrvFileString != null) {
					mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_EDI, true);

					ArrayList<RadioService> tempAddList = new ArrayList<>();

					JSONArray srvListArr = new JSONArray(savedSrvFileString);
					if (DEBUG) Log.d(TAG, "Read EDISrvListJson length: " + srvListArr.length());
					for (int i = 0; i < srvListArr.length(); i++) {
						JSONObject srvObj = srvListArr.getJSONObject(i);

						String ediurl = srvObj.getString("ediurl");
						if (ediurl == null) {
							continue;
						}

						RadioServiceDabEdiImpl ediSrv = new RadioServiceDabEdiImpl(ediurl);

						recreateDabService(srvObj, ediSrv);

						tempAddList.add(ediSrv);
					}

					mServicesMap.put(RadioServiceType.RADIOSERVICE_TYPE_EDI, new CopyOnWriteArrayList<>(tempAddList));

					if (DEBUG) Log.d(TAG, "Restoring EdiSrvListJson done!");
				}
			} catch (JSONException jsonExc) {
				if (DEBUG) jsonExc.printStackTrace();
			} catch (FileNotFoundException fnfExc) {
				if (DEBUG) fnfExc.printStackTrace();
			} catch (IOException ioExc) {
				if (DEBUG) ioExc.printStackTrace();
			} finally {
				if (DEBUG) Log.d(TAG, "Unlocking list for: " + RadioServiceType.RADIOSERVICE_TYPE_EDI.toString());
				mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_EDI, false);

				if(DEBUG)Log.d(TAG, "EDI List unlocked: " + mServicesDeSerializingInProgress.get(RadioServiceType.RADIOSERVICE_TYPE_EDI));
			}
		} else {
			if (DEBUG) Log.d(TAG, "Restoring EdiSrvListJson does not exist");
		}

		mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_EDI, false);
	}

	private void deserializeDabServices() {
		if (SERVICES_JSON_DAB != null) {
			Boolean deserInProgress = mServicesDeSerializingInProgress.get(RadioServiceType.RADIOSERVICE_TYPE_DAB);
			if (deserInProgress != null && deserInProgress && !mFirstInitDab) {
				if (DEBUG) Log.w(TAG, "Deserializing DAB services already in progress");
				return;
			}

			mFirstInitDab = false;

			try {
				String savedSrvFileString = readServiceFile(SERVICES_JSON_DAB);
				if (savedSrvFileString != null) {
					mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_DAB, true);

					ArrayList<RadioService> tempAddList = new ArrayList<>();

					JSONArray srvListArr = new JSONArray(savedSrvFileString);
					if (DEBUG) Log.d(TAG, "Read DABSrvListJson length: " + srvListArr.length());
					for (int i = 0; i < srvListArr.length(); i++) {
						JSONObject srvObj = srvListArr.getJSONObject(i);

						RadioServiceDabImpl dabSrv = new RadioServiceDabImpl();

						recreateDabService(srvObj, dabSrv);
						tempAddList.add(dabSrv);
					}

					mServicesMap.put(RadioServiceType.RADIOSERVICE_TYPE_DAB, new CopyOnWriteArrayList<>(tempAddList));

					if (DEBUG) Log.d(TAG, "Restoring DabSrvListJson done!");
				}
			} catch (JSONException jsonExc) {
				if (DEBUG) jsonExc.printStackTrace();
			} catch (FileNotFoundException fnfExc) {
				if (DEBUG) fnfExc.printStackTrace();
			} catch (IOException ioExc) {
				if (DEBUG) ioExc.printStackTrace();
			} finally {
				if (DEBUG) Log.d(TAG, "Unlocking list for: " + RadioServiceType.RADIOSERVICE_TYPE_DAB.toString());
				mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_DAB, false);

				if(DEBUG)Log.d(TAG, "DAB List unlocked: " + mServicesDeSerializingInProgress.get(RadioServiceType.RADIOSERVICE_TYPE_DAB));
			}
		} else {
			if (DEBUG) Log.d(TAG, "Restoring DabSrvListJson does not exist");
		}

		mServicesDeSerializingInProgress.put(RadioServiceType.RADIOSERVICE_TYPE_DAB, false);
	}

	/* experimental and dangerous */
	boolean deleteService(RadioService delSrv) {
		if(DEBUG)Log.d(TAG, "Trying to delete: " + delSrv.getServiceLabel() + " : " + delSrv.getRadioServiceType());

		boolean delSuccess = false;
		CopyOnWriteArrayList<RadioService> delList = mServicesMap.get(delSrv.getRadioServiceType());
		if(delList != null) {
			delSuccess = delList.remove(delSrv);

			if(DEBUG)Log.d(TAG, "Delete service success: " + delSuccess);
			if(delSuccess) {
				scheduleSaveServices(delSrv.getRadioServiceType());
			}
		}

		return delSuccess;
	}

	private ConcurrentHashMap<RadioServiceType, Timer> mSaveDelServicesMap = new ConcurrentHashMap<>();
	private void scheduleSaveServices(RadioServiceType type) {
		if(DEBUG)Log.d(TAG, "Scheduling DelSaveServices task");

		Timer delSaveTimer = mSaveDelServicesMap.get(type);
		if(delSaveTimer != null) {
			if(DEBUG)Log.d(TAG, "Canceling previous " + type.toString() + " DelSaveServices task");
			delSaveTimer.cancel();
		}

		Timer delTimer = new Timer();
		mSaveDelServicesMap.put(type, delTimer);
		delTimer.schedule(new TimerTask() {
			@Override
			public void run() {
				if(DEBUG)Log.d(TAG, "Executing DelSaveServices task for: " + type.toString());

				switch (type) {
					case RADIOSERVICE_TYPE_DAB:
						serializeDabServices();
						break;
					case RADIOSERVICE_TYPE_EDI:
						serializeEdiServices();
						break;
					case RADIOSERVICE_TYPE_IP:
						serializeIpServices();
						break;
					default:
						break;
				}
			}
		}, 5000);
	}

	/* RadioServiceManager interface implementation */
	@Override
	public List<RadioService> getRadioServices(RadioServiceType type) {
		if(type != null) {
			Boolean listInProgress = mServicesDeSerializingInProgress.get(type);
			if (listInProgress != null && !listInProgress) {
				CopyOnWriteArrayList<RadioService> retList = mServicesMap.get(type);
				if (retList != null) {
					return retList;
				}
			} else {
				if (DEBUG) Log.w(TAG, "ServiceList for " + type.toString() + " is not ready yet");
			}
		}

		return new ArrayList<>();
	}

	@Override
	public boolean removeRadioService(RadioService service) {
		if(service != null) {
			if(DEBUG)Log.d(TAG, "Removing service: " + service.getServiceLabel() + " : " + service.getRadioServiceType().toString());
			return deleteService(service);
		}

		return false;
	}

	@Override
	public void addRadioService(RadioService addSrv) {
		if(addSrv != null) {
			if(DEBUG)Log.d(TAG, "Adding service: " + addSrv.getServiceLabel() + " : " + addSrv.getRadioServiceType().toString());
			addService(addSrv);
			scheduleSaveServices(addSrv.getRadioServiceType());
		}
	}

	@Override
	public boolean addRadioServiceFromParams() {
		return false;
	}
}


