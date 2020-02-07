package org.omri.radioservice.metadata;

import java.util.Calendar;

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
 * Abstract class for a IP delivered RadioDNS RadioVis {@link Visual} metadata
 * 
 * @author Fabian Sattler, IRT GmbH
 */
public interface VisualIpRdnsRadioVis extends Visual {

	/**
	 * The Triggertime
	 * @return a Calendar for the Triggertime or {@code 'null'} if the triggertime is {@code 'now'}
	 */
	public Calendar getTriggerTime();

	/**
	 * Contains the ClickThroughURL for the image
	 * @return a String representing the ClickThroughURL for the image or an empty String
	 */
	public String getLink();

	/**
	 * Returns the category id of this {@link VisualIpRdnsRadioVis}.
	 * @return the category id or '-1' if it's not available
	 */
	public int getCategoryId();

	/**
	 * The ID of this {@link VisualIpRdnsRadioVis}
	 * @return the ID
	 */
	public int getSlideId();

	/**
	 * The category title
	 * @return the category title or an empty string
	 */
	public String getCategoryTitle();
}
