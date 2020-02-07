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
 * Abstract class for a DAB {@link RadioService}
 * @author Fabian Sattler, IRT GmbH
 */
public interface RadioServiceDab extends RadioService {
	
	/**
	 * Returns the extended country code (ECC) for the DAB Ensemble this {@link RadioServiceDab} belongs to
	 * @return the extended country code (ECC) for the DAB Ensemble this {@link RadioServiceDab} belongs to
	 */
	public int getEnsembleEcc();
	
	/**
	 * Returns the DAB Ensemble ID, this {@link RadioServiceDab} belongs to.
	 * @return the DAB Ensemble ID, this {@link RadioServiceDab} belongs to.
	 */
	public int getEnsembleId();
	
	/**
	 * Returns the label of the DAB Ensemble, this {@link RadioServiceDab} belongs to
	 * @return the label of the DAB Ensemble, this {@link RadioServiceDab} belongs to
	 */
	public String getEnsembleLabel();

	/**
	 * Returns the short label of the DAB Ensemble, this {@link RadioServiceDab} belongs to
	 * @return the short label of the DAB Ensemble, this {@link RadioServiceDab} belongs to
	 */
	public String getEnsembleShortLabel();
	
	/**
	 * Returns the frequency in kHz of the DAB Ensemble, this {@link RadioServiceDab} belongs to
	 * @return the frequency in kHz of the DAB Ensemble, this {@link RadioServiceDab} belongs to
	 */
	public int getEnsembleFrequency();

	/**
	 * Indicates if this {@link RadioServiceDab} is conditional access protected
	 * @return a boolean indicating if this {@link RadioServiceDab} is conditional access protected
	 */
	public boolean isCaProtected();

	/**
	 * Returns the conditional access id of this {@link RadioServiceDab} if conditional access is applied
	 * @return the conditional access id of this {@link RadioServiceDab} if conditional access is applied
	 */
	public int getCaId();

	/**
	 * Returns the short label of this {@link RadioServiceDab}
	 * @return the short label of this {@link RadioServiceDab}
	 */
	public String getShortLabel();

	/**
	 * Returns the service id. The length depends on the type of service. 16 bit unsigned for programme and 32 bit unsigned for data service.
	 * @return the service id.
	 */
	public int getServiceId();
	
	/**
	 * Indicates if this {@link RadioServiceDab} is a DAB programme or a DAB data service  
	 * @return indication for programme or data service
	 */
	public boolean isProgrammeService();
	
	/**
	 * Returns a list with the {@link RadioServiceDabComponent}s associated with this {@link RadioServiceDab}
	 * @return a list with the {@link RadioServiceDabComponent}s associated with this {@link RadioServiceDab}
	 */
	public List<RadioServiceDabComponent> getServiceComponents();
	

}
