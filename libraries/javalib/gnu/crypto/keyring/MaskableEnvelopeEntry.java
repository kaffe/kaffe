package gnu.crypto.keyring;  // -*- c-basic-offset: 3 -*-

// ---------------------------------------------------------------------------
// $Id: MaskableEnvelopeEntry.java,v 1.2 2005/07/04 00:04:55 robilad Exp $
//
// Copyright (C) 2003 Free Software Foundation, Inc.
//
// This file is part of GNU Crypto.
//
// GNU Crypto is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// GNU Crypto is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to the
//
//    Free Software Foundation Inc.,
//    51 Franklin Street, Fifth Floor,
//    Boston, MA 02110-1301
//    USA
//
// Linking this library statically or dynamically with other modules is
// making a combined work based on this library.  Thus, the terms and
// conditions of the GNU General Public License cover the whole
// combination.
//
// As a special exception, the copyright holders of this library give
// you permission to link this library with independent modules to
// produce an executable, regardless of the license terms of these
// independent modules, and to copy and distribute the resulting
// executable under terms of your choice, provided that you also meet,
// for each linked independent module, the terms and conditions of the
// license of that module.  An independent module is a module which is
// not derived from or based on this library.  If you modify this
// library, you may extend this exception to your version of the
// library, but you are not obligated to do so.  If you do not wish to
// do so, delete this exception statement from your version.
//
// ---------------------------------------------------------------------------

import java.util.ArrayList;
import java.util.List;

/**
 * An envelope entry that can be "masked" -- placed in a state where the
 * envelope's contents cannot be accessed, due to the envelope not being
 * fully decoded, for example.
 */
public abstract class MaskableEnvelopeEntry extends EnvelopeEntry {

   // Fields.
   // ------------------------------------------------------------------------

   /** The masked state. */
   protected boolean masked;

   // Constructors.
   // ------------------------------------------------------------------------

   public MaskableEnvelopeEntry(int type, Properties properties) {
      super(type, properties);
   }

   protected MaskableEnvelopeEntry(int type) {
      super (type);
   }

   // Instance methods.
   // ------------------------------------------------------------------------

   /**
    * Sets the masked state to the specified value.
    *
    * @param masked The new masked state.
    */
   protected final void setMasked(boolean masked) {
      this.masked = masked;
   }

   /**
    * Gets the masked state of this object. Certain operations on this object
    * will fail if it is masked.
    *
    * @return The current masked state.
    */
   public boolean isMasked() {
      return masked;
   }

   public void add(Entry entry) {
      if (isMasked()) {
         throw new IllegalStateException("masked envelope");
      }
      super.add(entry);
   }

   public boolean containsEntry(Entry entry) {
      if (isMasked()) {
         throw new IllegalStateException("masked envelope");
      }
      return super.containsEntry(entry);
   }

   public List getEntries() {
      if (isMasked()) {
         throw new IllegalStateException("masked envelope");
      }
      return new ArrayList(entries);
   }

   public List get(String alias) {
      if (isMasked()) {
         throw new IllegalStateException("masked envelope");
      }
      return super.get(alias);
   }

   public boolean remove(Entry entry) {
      if (isMasked()) {
         throw new IllegalStateException("masked envelope");
      }
      return super.remove(entry);
   }

   public void remove(String alias) {
      if (isMasked()) {
         throw new IllegalStateException("masked envelope");
      }
      super.remove(alias);
   }
}
