package org.omri.radioservice;

/**
 * Copyright (C) 2016 Open Mobile Radio Interface (OMRI) Group
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
 * {@link RadioServiceDabDataServiceComponentType} type definitions
 * @author Fabian Sattler, IRT GmbH
 */
public enum RadioServiceDabDataServiceComponentType {
	UNSPECIFIED_DATA(0, "Unspecified"),
	TRAFFIC_MESSAGE_CHANNEL(1, "Traffic Message Channel (TMC)"),
	EMERGENCY_WARNING_SYSTEM(2, "Emergency Warning System (EWS)"),
	INTERACTIVE_TEXT_TRANSMISSION_SYSTEM(3, "Interactive Text Transmission System (ITTS)"),
	PAGING(4, "Paging"),
	TRANSPARENT_DATA_CHANNEL(5, "Transparent Data Channel (TDC)"),

	MPEG_2_TRANSPORT_STREAM(24, "MPEG-2 Transportstream"),

	EMBEDDED_IP_PACKETS(59, "Embedded IP-Packets"),
	MULTIMEDIA_OBJECT_TRANSFER(60, "Multimedia Object Transfer (MOT)"),
	PROPRIETARY_SERVICE(61, "Proprietary"),
	NOT_USED_62(62, "Not used"),
	NOT_USED_63(63, "Not used");

	private final int mType;
	private final String mName;

	private RadioServiceDabDataServiceComponentType(int type, String name) {
		mType = type;
		mName = name;
	}

	public int getType() {
		return mType;
	}

	public String getName() {
		return mName;
	}

	public static RadioServiceDabDataServiceComponentType getDSCTyByType(int type) {
		for(RadioServiceDabDataServiceComponentType dscty : RadioServiceDabDataServiceComponentType.values()) {
			if(dscty.getType() == type) {
				return dscty;
			}
		}

		return RadioServiceDabDataServiceComponentType.UNSPECIFIED_DATA;
	}
}
