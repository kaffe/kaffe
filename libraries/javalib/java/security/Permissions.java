
package java.security;

import java.util.Enumeration;
import java.util.Iterator;
import java.util.HashMap;
import java.util.HashSet;

public class Permissions extends PermissionCollection {

	private HashMap permissionCollections;
	
	public Permissions() {
		permissionCollections = new HashMap();
	}

	public void add(Permission permission) {

		PermissionCollection pc = (PermissionCollection)permissionCollections.get(permission.getClass());

		if (pc == null) {
			pc = permission.newPermissionCollection();
			
			if (pc == null) {
				pc = new DefaultPermissionCollection();
			}
			
			permissionCollections.put(permission.getClass(), pc);
		}

		pc.add(permission);
	}

	public boolean implies(Permission permission) {
	
		if (permissionCollections.get(AllPermission.class) != null)
			return true;

		PermissionCollection pc = (PermissionCollection)permissionCollections.get(permission.getClass());
	
		if (pc == null)
			return false;

		return pc.implies(permission);	
	}

	public Enumeration elements() {
		return new Enumeration () {
			Iterator i = permissionCollections.values().iterator();
		
			public boolean hasMoreElements() {
				return i.hasNext();
			}

			public Object nextElement() {
				return i.next();
			}
		};
	}

	public String toString() {
		return super.toString();
	}
	
	private static class DefaultPermissionCollection extends PermissionCollection {
		private HashSet permissions;

		public DefaultPermissionCollection() {
			permissions = new HashSet();
		}

		public void add(Permission p) {
			permissions.add(p);	
		}

		public boolean implies(Permission p) {
			boolean ret = true;
			
			for (Iterator i=permissions.iterator();i.hasNext() && ret;) {
				ret &= ((Permission)i.next()).implies(p);
			}
			
			return ret;	
		}
	
		public Enumeration elements() {
			return new Enumeration () {
				Iterator i = permissions.iterator();
				
				public boolean hasMoreElements() {
					return i.hasNext();
				}

				public Object nextElement() {
					return i.next();
				}
			};
		}
	}
}
