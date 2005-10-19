/* DialogPasswordHandler.java -- input passwords with a GUI dialog.
   Copyright (C) 2003  Casey Marshall <rsdio@metastatic.org>

This file is a part of Jessie.

Jessie is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

Jessie is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with Jessie; if not, write to the

   Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor,
   Boston, MA  02110-1301
   USA  */


package org.metastatic.jessie;

import java.awt.Button;
import java.awt.Frame;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Label;
import java.awt.TextField;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;

import javax.security.auth.callback.Callback;
import javax.security.auth.callback.CallbackHandler;
import javax.security.auth.callback.PasswordCallback;
import javax.security.auth.callback.UnsupportedCallbackException;

/**
 * This is a {@link CallbackHandler} for {@link PasswordCallback} objects,
 * which prompts the user for a password with an AWT dialog.
 */
public class DialogPasswordHandler implements CallbackHandler, ActionListener,
                                              WindowListener
{

  // Fields.
  // -------------------------------------------------------------------------

  private final Object lock = new Object();
  private final Button accept;
  private final Button cancel;
  private Frame dialog;
  private final Label label;
  private final TextField password;
  private PasswordCallback current;

  // Constructor.
  // -------------------------------------------------------------------------

  public DialogPasswordHandler()
  {
    label = new Label("");
    accept = new Button("Accept");
    cancel = new Button("Cancel");
    password = new TextField();
    dialog = new Frame();

    Insets i = new Insets(5, 5, 5, 5);
    dialog.setLayout(new GridBagLayout());
    dialog.add(label,
      new GridBagConstraints(0, 0, 2, 1, 1.0, 1.0, GridBagConstraints.WEST,
                             GridBagConstraints.HORIZONTAL, i, 0, 0));
    dialog.add(password,
      new GridBagConstraints(0, 1, 2, 1, 1.0, 1.0, GridBagConstraints.CENTER,
                             GridBagConstraints.HORIZONTAL, i, 0, 0));
    dialog.add(accept,
      new GridBagConstraints(0, 2, 1, 1, 1.0, 1.0, GridBagConstraints.EAST,
                             GridBagConstraints.NONE, i, 0, 0));
    dialog.add(cancel,
      new GridBagConstraints(1, 2, 1, 1, 1.0, 1.0, GridBagConstraints.WEST,
                             GridBagConstraints.NONE, i, 0, 0));

    accept.addActionListener(this);
    cancel.addActionListener(this);
    password.addActionListener(this);
    dialog.addWindowListener(this);
    dialog.pack();
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  protected void finalize()
  {
    dialog.dispose();
    dialog = null;
  }

  // CallbackHandler implementation.
  // -------------------------------------------------------------------------

  public void handle(Callback[] callbacks) throws UnsupportedCallbackException
  {
    if (callbacks == null || callbacks.length == 0)
      throw new IllegalArgumentException();
    for (int i = 0; i < callbacks.length; i++)
      {
        if (!(callbacks[i] instanceof PasswordCallback))
          throw new UnsupportedCallbackException(callbacks[i]);
      }
    for (int i = 0; i < callbacks.length; i++)
      {
        synchronized (lock)
          {
            current = (PasswordCallback) callbacks[i];
            label.setText(current.getPrompt());
            password.setEchoChar(current.isEchoOn() ? 0 : '*');
            dialog.show();
            try { lock.wait(300000); }
            catch (InterruptedException x) { }
            dialog.hide();
          }
      }
  }

  // ActionListener implementation.
  // -------------------------------------------------------------------------

  public void actionPerformed(ActionEvent event)
  {
    synchronized (lock)
      {
        System.err.println("source = " + event.getSource().getClass().getName());
        if (event.getSource() == accept || event.getSource() == password)
          {
            current.setPassword(password.getText().toCharArray());
          }
        dialog.hide();
        lock.notifyAll();
      }
  }

  // WindowListener implementation.
  // -------------------------------------------------------------------------

  public void windowClosing(WindowEvent event)
  {
    synchronized (lock)
      {
        dialog.hide();
        lock.notifyAll();
      }
  }

  public void windowClosed(WindowEvent event)
  {
    synchronized (lock)
      {
        dialog.hide();
        lock.notifyAll();
      }
  }

  public void windowOpened(WindowEvent e) { }
  public void windowIconified(WindowEvent e) { }
  public void windowDeiconified(WindowEvent e) { }
  public void windowActivated(WindowEvent e) { }
  public void windowDeactivated(WindowEvent e) { }
}
