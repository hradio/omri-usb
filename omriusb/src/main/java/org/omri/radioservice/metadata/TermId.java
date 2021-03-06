package org.omri.radioservice.metadata;

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
*/
public interface TermId {
	
	/**
	 * Returns the genre href of this termID
	 * @return the genre href of this termID
	 */
	public String getGenreHref();

	/**
	 * Returns the TermID as String representation
	 * @return the TermId as String representation
	 */
	public String getTermId();

	/**
	 * Returns text of this termID.
	 * @return text of this termID
	 */
	public String getText();
}
