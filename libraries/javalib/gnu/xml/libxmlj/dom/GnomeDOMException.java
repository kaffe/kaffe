/*
 * GnomeDOMException.java
 * Copyright (C) 2004 The Free Software Foundation
 * 
 * This file is part of GNU JAXP, a library.
 * 
 * GNU JAXP is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * GNU JAXP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License.
 */
package gnu.xml.libxmlj.dom;

import org.w3c.dom.DOMException;

class GnomeDOMException
extends DOMException
{

  GnomeDOMException (short code, String message)
  {
    super (code, createMessage (code, message));
  }

  private static String createMessage (int code, String message)
  {
    if (message != null)
      {
        return message;
      }
    switch (code)
      {
      case INDEX_SIZE_ERR:
        return "INDEX_SIZE_ERR";
      case DOMSTRING_SIZE_ERR:
        return "DOMSTRING_SIZE_ERR";
      case HIERARCHY_REQUEST_ERR:
        return "HIERARCHY_REQUEST_ERR";
      case WRONG_DOCUMENT_ERR:
        return "WRONG_DOCUMENT_ERR";
      case INVALID_CHARACTER_ERR:
        return "INVALID_CHARACTER_ERR";
      case NO_DATA_ALLOWED_ERR:
        return "NO_DATA_ALLOWED_ERR";
      case NO_MODIFICATION_ALLOWED_ERR:
        return "NO_MODIFICATION_ALLOWED_ERR";
      case NOT_FOUND_ERR:
        return "NOT_FOUND_ERR";
      case NOT_SUPPORTED_ERR:
        return "NOT_SUPPORTED_ERR";
      case INUSE_ATTRIBUTE_ERR:
        return "INUSE_ATTRIBUTE_ERR";
      case INVALID_STATE_ERR:
        return "INVALID_STATE_ERR";
      case SYNTAX_ERR:
        return "SYNTAX_ERR";
      case INVALID_MODIFICATION_ERR:
        return "INVALID_MODIFICATION_ERR";
      case NAMESPACE_ERR:
        return "NAMESPACE_ERR";
      case INVALID_ACCESS_ERR:
        return "INVALID_ACCESS_ERR";
      case VALIDATION_ERR:
        return "VALIDATION_ERR";
      case TYPE_MISMATCH_ERR:
        return "TYPE_MISMATCH_ERR";
      default:
        return null;
      }
  }
  
}
