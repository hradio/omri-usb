package org.omri.radioservice.metadata;

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
 * Abstract class for a DAB Dynamic Label (Plus) {@link Textual} metadata
 * 
 * @author Fabian Sattler, IRT GmbH
 */
public interface TextualDabDynamicLabel extends Textual {
	
	/**
	 * Indicates if this {@link TextualDabDynamicLabel} has DL+ tags
	 * @return indication of DL+ tags
	 */
	public boolean hasTags();
	
	/**
	 * Returns the number of tags. Only applicable for DL+. Check 'hasTags()'
	 * @return the number of tags
	 */
	public int getTagCount();
	
	/**
	 * Returns a list of {@link TextualDabDynamicLabelPlusItem}s or an empty list
	 * @return a list of {@link TextualDabDynamicLabelPlusItem}s or an empty list
	 */
	public List<TextualDabDynamicLabelPlusItem> getDlPlusItems();

	public boolean itemRunning();

	public boolean itemToggled();
}
