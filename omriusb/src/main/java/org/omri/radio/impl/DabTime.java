package org.omri.radio.impl;

import java.util.Calendar;
import java.util.TimeZone;

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
class DabTime {

	private final int mYear;
	private final int mMonth;
	private final int mDay;
	private final int mHour;
	private final int mMinute;
	private final int mSecond;
	private final int mMilliSeconds;

	private final long mPosixMillis;

	public DabTime(int year, int month, int day, int hour, int minute, int second, int milliSeconds) {
		mYear = year;
		mMonth = month;
		mDay = day;
		mHour = hour;
		mMinute = minute;
		mSecond = second;
		mMilliSeconds = milliSeconds;

		Calendar dabTimeCal = Calendar.getInstance(TimeZone.getTimeZone("UTC"));
		dabTimeCal.set(Calendar.YEAR, mYear);
		dabTimeCal.set(Calendar.MONTH, mMonth-1);
		dabTimeCal.set(Calendar.DAY_OF_MONTH, mDay);
		dabTimeCal.set(Calendar.HOUR_OF_DAY, mHour);
		dabTimeCal.set(Calendar.MINUTE, mMinute);
		dabTimeCal.set(Calendar.SECOND, mSecond);
		dabTimeCal.set(Calendar.MILLISECOND, mMilliSeconds);

		mPosixMillis = dabTimeCal.getTimeInMillis();
	}

	long getPosixMillis() {
		return mPosixMillis;
	}
}
