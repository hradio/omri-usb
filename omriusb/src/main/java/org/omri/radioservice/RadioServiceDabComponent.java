package org.omri.radioservice;

import java.util.List;

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
 * Abstract class containing informations for a service component (SC) for a {@link RadioServiceDab}
 * @author Fabian Sattler, IRT GmbH
 */
public interface RadioServiceDabComponent {

	/**
	 * Returns the bitrate in kbit/s for this {@link RadioServiceDabComponent}
	 * @return the bitrate in kbit/s for this {@link RadioServiceDabComponent}
	 */
	public int getBitrate();
	
	/**
	 * Indicates if the CA (Conditional Access) flag is set
	 * @return indication for conditional access
	 */
	public boolean isCaApplied();
	
	/**
	 * Returns the {@link RadioServiceDabComponent}'s service ID
	 * @return the {@link RadioServiceDabComponent}'s service ID
	 */
	public int getServiceId();
	
	/**
	 * Returns the {@link RadioServiceDabComponent} subchannel ID
	 * @return the {@link RadioServiceDabComponent} subchannel ID
	 */
	public int getSubchannelId();
	
	/**
	 * Returns the label for this {@link RadioServiceDabComponent}
	 * @return the label for this {@link RadioServiceDabComponent}
	 */
	public String getLabel();
	
	/**
	 * Returns the packet address or -1 if it's not a packetmode service.
	 * @return the packet address or -1 if it's not a packetmode service.
	 */
	public int getPacketAddress();
	
	/**
	 * Indicates if this {@link RadioServiceDabComponent} is the primary component of this {@link RadioServiceDab}
	 * @return if this {@link RadioServiceDabComponent} is the primary component of this {@link RadioServiceDab}
	 */
	public boolean isPrimary();
	
	/**
	 * Returns the service component ID within service (ScIDs)
	 * @return the service component ID within service (ScIDs)
	 */
	public int getServiceComponentIdWithinService();

	/**
	 * Returns the start address of this {@link RadioServiceDabComponent} in the MSC
	 * @return the start address of this {@link RadioServiceDabComponent} in the MSC
	 */
	public int getMscStartAddress();

	/**
	 * Returns the size of this {@link RadioServiceDabComponent} in capacity units
	 * @return the size of this {@link RadioServiceDabComponent} in capacity units
	 */
	public int getSubchannelSize();

	/**
	 * Return the protectionlevel of this {@link RadioServiceDabComponent}
	 * @return the protectionlevel of this {@link RadioServiceDabComponent}
	 */
	public int getProtectionLevel();

	/**
	 * Return the protectiontype of this {@link RadioServiceDabComponent}
	 * @return the protectiontype of this {@link RadioServiceDabComponent}
	 */
	public int getProtectionType();

	/**
	 * Returns the unequal error protection table index of this {@link RadioServiceDabComponent}
	 * @return the unequal error protection table index of this {@link RadioServiceDabComponent}
	 */
	public int getUepTableIndex();

	/**
	 * Indicates if a forward error correction (FEC) scheme is applied to this {@link RadioServiceDabComponent}
	 * @return a boolean indicating if a forward error correction (FEC) scheme is applied to this {@link RadioServiceDabComponent}
	 */
	public boolean isFecSchemeApplied();

	/**
	 * Returns the Transport Mode (TM) ID
	 * @return the Transport Mode (TM) ID
	 */
	public int getTmId();
	
	/**
	 * Returns the service component type. Audioservice Component Type (ASCTy) for TmId{0}; Dataservice Component Type (DSCTy) for TmId{1} and TmId{3}.
	 * @return the service component type.
	 */
	public int getServiceComponentType();

	/**
	 * Indicates if MSC datagroup transport is used for this component. Only applicable for dataservices TmId{1} and TmId{3}.
	 * @return if MSC datagroup transport is used for this component.
	 */
	public boolean isDatagroupTransportUsed();
	
	/**
	 * Returns a list with {@link RadioServiceDabUserApplication}s for this SC
	 * @return a list with {@link RadioServiceDabUserApplication}s for this SC
	 */
	public List<RadioServiceDabUserApplication> getUserApplications();
	
	/**
	 * Subscribe a {@link RadioServiceDabComponentListener} to receive updates from this {@link RadioServiceDabComponent}
	 * @param dabComponentListener the {@link RadioServiceDabComponentListener} to subscribe
	 */
	public void subscribe(RadioServiceDabComponentListener dabComponentListener);
	
	/**
	 * Unsubscribe a {@link RadioServiceDabComponentListener}
	 * @param dabComponentListener the {@link RadioServiceDabComponentListener} to unsubscribe
	 */
	public void unsubscribe(RadioServiceDabComponentListener dabComponentListener);
}
