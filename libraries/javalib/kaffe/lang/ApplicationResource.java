/*
 * ApplicationResource -
 *  If an resource used by an application needs tidying up when the
 *  application terminates, it should implement this interface which will
 *  call invoke 'freeResource' when the application exits.
 */

package kaffe.lang;

public interface ApplicationResource {

void freeResource();

}
