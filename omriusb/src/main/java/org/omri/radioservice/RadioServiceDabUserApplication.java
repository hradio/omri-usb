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
 * Abstract class containing informations about one user application for a {@link RadioServiceDabComponent}
 * @author Fabian Sattler, IRT GmbH
 */
public interface RadioServiceDabUserApplication {
	
	/**
	 * Returns the {@link RadioServiceDabUserApplicationType} user application type
	 * @return the {@link RadioServiceDabUserApplicationType} user application type
	 */
	public RadioServiceDabUserApplicationType getType();

	/**
	 * Indicates if this {@link RadioServiceDabUserApplication} is protected by conditional access
	 * @return a boolean indicating if this {@link RadioServiceDabUserApplication} is protected by conditional access
	 */
	public boolean isCaProtected();

	/**
	 * Returns the Conditional Access Organization ID. Only applicable if CaProtected.
	 * @return the Conditional Access Organization ID or {@code -1} if no CA is applied.
	 */
	public int getCaOrganization();

	/**
	 * Indicates if this {@link RadioServiceDabUserApplication} is transported in the XPAD data field
	 * @return a boolean indicating if this {@link RadioServiceDabUserApplication} is transported in the XPAD data field
	 */
	public boolean isXpadApptype();

	/**
	 * Returns the XPAD application type. Only applicable if this {@link RadioServiceDabUserApplication} isXpadApptype();
	 * @return the XPAD application type.
	 */
	public int getXpadAppType();

	/**
	 * Indicates if this {@link RadioServiceDabUserApplication} uses MSC datagroups for transportation.
	 * @return a boolean indicating if this {@link RadioServiceDabUserApplication} uses MSC datagroups for transportation.
	 */
	public boolean isDatagroupTransportUsed();

	/**
	 * Returns the {@link RadioServiceDabDataServiceComponentType} of this {@link RadioServiceDabUserApplication}.
	 * @return the {@link RadioServiceDabDataServiceComponentType} of this {@link RadioServiceDabUserApplication}.
	 */
	public RadioServiceDabDataServiceComponentType getDataServiceComponentType();

	/**
	 * Returns a byte array with data specific to some userapplications.
	 * @return a byte array with data specific to some userapplications or {@code null}.
	 */
	public byte[] getUserApplicationData();
}
