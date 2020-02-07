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
 * Abstract class for a DAB {@link RadioService}
 * @author Fabian Sattler, IRT GmbH
 */

import org.omri.radioservice.metadata.SbtItem;

import java.util.List;

/**
 * Interface representing a DAB over IP service
 */
public interface RadioServiceDabEdi extends RadioServiceDab {

	/**
	 * Returns the URL of the EDI stream
	 * @return the URL of the EDI stream
	 */
	public String getUrl();

	/**
	 * Returns if the EDI stream has Server Based Timeshift functionality
	 * @return {@code true} if SBT is available, {@code false} otherwise
	 */
	public boolean sbtEnabled();

	/**
	 * Returns the maximum server based timeshift duration in milliseconds
	 * @return the maximum server based timeshift duration in milliseconds
	 */
	public long getSbtMax();

	/**
	 * Seeks the server based timeshift stream
	 * @param seekMs the milliseconds to seek to
	 */
	public void seekSbt(long seekMs);

	/**
	 *
	 * @param toggleId
	 */
	public void setToggleSbt(long toggleId);

	/**
	 * Pauses the stream on the server
	 * @param pause {@code true} to pause, {@code false} otherwise
	 */
	public void pauseSbt(boolean pause);

	/**
	 * Returns a list of available {@link SbtItem}s or an empty list
	 * @return a list of available {@link SbtItem}s
	 */
	public List<SbtItem> getSbtItems();

	/**
	 * Returns the unique server based timeshift-token
	 * @return the unique server based timeshift-token
	 */
	public String getSbtToken();

	/**
	 * Set a unique server based timeshift-token to initialize a connection with
	 * @param token a previously obtained unique server based timeshift-token
	 */
	public void setInitialSbtToken(String token);

	/**
	 * Set an offset in the server based timeshift buffer to initialize a connection with
	 * @param offset an offset in the server based timeshift buffer
	 */
	public void setInitialSbtOffset(long offset);

	/**
	 * Set a server based timeshift toggleId to start the stream at
	 * @param toggleId the toggleId to start the stream at
	 */
	public void setInitialSbtToggleId(long toggleId);

	public void setInitialTimePosix(long posixSeconds);

	/**
	 * Add a {@link SbtCallback}
	 * @param callback the callback to add
	 */
	public void addSbtCallback(SbtCallback callback);

	/**
	 * Remove a {@link SbtCallback}
	 * @param callback the callback to remove
	 */
	public void removeSbtCallback(SbtCallback callback);

	/**
	 * Returns the current real-world time in Posix milliseconds
	 * @return Posix milliseconds
	 */
	public long getRealtimePosixMs();

	public interface SbtCallback {

		void sbtEnabled();

		void streamDabTime(long posixMilliSeconds);

		void sbtItemAdded(SbtItem item);

		void sbtItemInvalid(SbtItem item);
	}
}
