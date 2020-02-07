package org.omri.radio.impl;

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

public abstract class RadioDnsEpgBearer implements Comparable<RadioDnsEpgBearer> {

	public abstract String getBearerId();

	public abstract RadioDnsEpgBearerType getBearerType();

	public abstract int getCost();

	public abstract int getBitrate();

	public abstract String getMimeValue();
}
