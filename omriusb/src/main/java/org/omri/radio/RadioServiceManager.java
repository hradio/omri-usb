package org.omri.radio;

import org.omri.radioservice.RadioService;
import org.omri.radioservice.RadioServiceType;

import java.util.List;

//TODO add to OMRI_BASE
public interface RadioServiceManager {

	/**
	 * Returns a list of all known {@link RadioService}s or an empty list
	 * @param type the wanted {@link RadioServiceType} or {@code null} to get all types
	 * @return a list of all known {@link RadioService}s or an empty list
	 */
	public List<RadioService> getRadioServices(RadioServiceType type);

	/**
	 * Removes the given {@link RadioService} permanently
	 * @param service the {@link RadioService} to remove
	 * @return {@code true} on success, {@code false} if the {@link RadioService} was not found in the list
	 */
	public boolean removeRadioService(RadioService service);

	/**
	 * Adds the given {@link RadioService} to the list
	 * @param addSrv the {@link RadioService} to add
	 */
	public void addRadioService(RadioService addSrv);

	public boolean addRadioServiceFromParams();
}
