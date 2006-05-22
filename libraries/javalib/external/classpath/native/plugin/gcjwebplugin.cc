/* gcjwebplugin.cc -- web browser plugin to execute Java applets
   Copyright (C) 2003, 2004, 2006  Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

// System includes.
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Netscape plugin API includes.
#include <npapi.h>
#include <npupp.h>

// GLib includes.
#include <glib.h>

// gcjwebplugin includes.
#include "config.h"

// Documentbase retrieval includes.
#include <nsIPluginInstance.h>
#include <nsIPluginInstancePeer.h>
#include <nsIPluginTagInfo2.h>

// Debugging macros.
#define PLUGIN_DEBUG(message)                                           \
  g_print ("GCJ PLUGIN: thread %p: %s\n", g_thread_self (), message)

#define PLUGIN_DEBUG_TWO(first, second)                                 \
  g_print ("GCJ PLUGIN: thread %p: %s %s\n", g_thread_self (),          \
           first, second)

// Error reporting macros.
#define PLUGIN_ERROR(message)                                       \
  g_printerr ("%s:%d: thread %p: Error: %s\n", __FILE__, __LINE__,  \
              g_thread_self (), message)

#define PLUGIN_ERROR_TWO(first, second)                                 \
  g_printerr ("%s:%d: thread %p: Error: %s: %s\n", __FILE__, __LINE__,  \
              g_thread_self (), first, second)

// Plugin information passed to about:plugins.
#define PLUGIN_NAME "GCJ Web Browser Plugin"
#define PLUGIN_DESC "The " PLUGIN_NAME " executes Java applets."
#define PLUGIN_MIME_DESC                                           \
  "application/x-java-vm:class,jar:GCJ;"                           \
  "application/x-java-applet:class,jar:GCJ;"                       \
  "application/x-java-applet;version=1.1:class,jar:GCJ;"           \
  "application/x-java-applet;version=1.1.1:class,jar:GCJ;"         \
  "application/x-java-applet;version=1.1.2:class,jar:GCJ;"         \
  "application/x-java-applet;version=1.1.3:class,jar:GCJ;"         \
  "application/x-java-applet;version=1.2:class,jar:GCJ;"           \
  "application/x-java-applet;version=1.2.1:class,jar:GCJ;"         \
  "application/x-java-applet;version=1.2.2:class,jar:GCJ;"         \
  "application/x-java-applet;version=1.3:class,jar:GCJ;"           \
  "application/x-java-applet;version=1.3.1:class,jar:GCJ;"         \
  "application/x-java-applet;version=1.4:class,jar:GCJ;"           \
  "application/x-java-applet;version=1.4.1:class,jar:GCJ;"         \
  "application/x-java-applet;version=1.4.2:class,jar:GCJ;"         \
  "application/x-java-applet;jpi-version=1.4.2_01:class,jar:GCJ;"  \
  "application/x-java-bean:class,jar:GCJ;"                         \
  "application/x-java-bean;version=1.1:class,jar:GCJ;"             \
  "application/x-java-bean;version=1.1.1:class,jar:GCJ;"           \
  "application/x-java-bean;version=1.1.2:class,jar:GCJ;"           \
  "application/x-java-bean;version=1.1.3:class,jar:GCJ;"           \
  "application/x-java-bean;version=1.2:class,jar:GCJ;"             \
  "application/x-java-bean;version=1.2.1:class,jar:GCJ;"           \
  "application/x-java-bean;version=1.2.2:class,jar:GCJ;"           \
  "application/x-java-bean;version=1.3:class,jar:GCJ;"             \
  "application/x-java-bean;version=1.3.1:class,jar:GCJ;"           \
  "application/x-java-bean;version=1.4:class,jar:GCJ;"             \
  "application/x-java-bean;version=1.4.1:class,jar:GCJ;"           \
  "application/x-java-bean;version=1.4.2:class,jar:GCJ;"           \
  "application/x-java-bean;jpi-version=1.4.2_01:class,jar:GCJ;"
#define PLUGIN_URL NS_INLINE_PLUGIN_CONTRACTID_PREFIX NS_JVM_MIME_TYPE
#define PLUGIN_MIME_TYPE "application/x-java-vm"
#define PLUGIN_FILE_EXTS "class,jar,zip"
#define PLUGIN_MIME_COUNT 1

// Directory in which named pipes are created.
#define PIPE_DIRECTORY "/tmp"

// Documentbase retrieval required definition.
static NS_DEFINE_IID (kIPluginTagInfo2IID, NS_IPLUGINTAGINFO2_IID);

// Browser function table.
static NPNetscapeFuncs browserFunctions;

// GCJPluginData stores all the data associated with a single plugin
// instance.  A separate plugin instance is created for each <APPLET>
// tag.  For now, each plugin instance spawns its own applet viewer
// process but this may need to change if we find pages containing
// multiple applets that expect to be running in the same VM.
struct GCJPluginData
{
  // A unique identifier for this plugin window.
  gchar* instance_string;
  // Applet viewer input pipe name.
  gchar* in_pipe_name;
  // Applet viewer input channel.
  GIOChannel* in_from_appletviewer;
  // Applet viewer input watch source.
  gint in_watch_source;
  // Applet viewer output pipe name.
  gchar* out_pipe_name;
  // Applet viewer output channel.
  GIOChannel* out_to_appletviewer;
  // Applet viewer output watch source.
  gint out_watch_source;
  // Mutex to protect appletviewer_alive.
  GMutex* appletviewer_mutex;
  // Back-pointer to the plugin instance to which this data belongs.
  // This should not be freed but instead simply set to NULL.
  NPP owner;
  // FALSE if the applet viewer process has died.  All code
  // communicating with the applet viewer should check this flag
  // before attempting to read from/write to the applet viewer pipes.
  gboolean appletviewer_alive;
  // The address of the plugin window.  This should not be freed but
  // instead simply set to NULL.
  gpointer window_handle;
  // The last plugin window width sent to us by the browser.
  guint32 window_width;
  // The last plugin window height sent to us by the browser.
  guint32 window_height;
};

// Documentbase retrieval type-punning union.
typedef union
{
  void** void_field;
  nsIPluginTagInfo2** info_field;
} info_union;

// Static instance helper functions.
// Have the browser allocate a new GCJPluginData structure.
static void plugin_data_new (GCJPluginData** data);
// Documentbase retrieval.
static gchar* plugin_get_documentbase (NPP instance);
// Callback used to monitor input pipe status.
static gboolean plugin_in_pipe_callback (GIOChannel* source,
                                         GIOCondition condition,
                                         gpointer plugin_data);
// Callback used to monitor output pipe status.
static gboolean plugin_out_pipe_callback (GIOChannel* source,
                                          GIOCondition condition,
                                          gpointer plugin_data);
static void plugin_start_appletviewer (GCJPluginData* data);
static gchar* plugin_create_applet_tag (int16 argc, char* argn[],
                                        char* argv[]);
static void plugin_send_message_to_appletviewer (GCJPluginData* data,
                                                 gchar const* message);
static void plugin_stop_appletviewer (GCJPluginData* data);
// Uninitialize GCJPluginData structure and delete pipes.
static void plugin_data_destroy (GCJPluginData** data);

// Global instance counter.
// Mutex to protect plugin_instance_counter.
static GMutex* plugin_instance_mutex = NULL;
// A counter used to create uniquely named pipes.
static gulong plugin_instance_counter = 0;

// Functions prefixed by GCJ_ are instance functions.  They are called
// by the browser and operate on instances of GCJPluginData.
// Functions prefixed by plugin_ are static helper functions.
// Functions prefixed by NP_ are factory functions.  They are called
// by the browser and provide functionality needed to create plugin
// instances.

// INSTANCE FUNCTIONS

// Creates a new gcjwebplugin instance.  This function creates a
// GCJPluginData* and stores it in instance->pdata.  The following
// GCJPluginData fiels are initialized: instance_string, in_pipe_name,
// in_from_appletviewer, in_watch_source, out_pipe_name,
// out_to_appletviewer, out_watch_source, appletviewer_mutex, owner,
// appletviewer_alive.  In addition two pipe files are created.  All
// of those fields must be properly destroyed, and the pipes deleted,
// by GCJ_Destroy.  If an error occurs during initialization then this
// function will free anything that's been allocated so far, set
// instance->pdata to NULL and return an error code.
NPError
GCJ_New (NPMIMEType pluginType, NPP instance, uint16 mode,
         int16 argc, char* argn[], char* argv[],
         NPSavedData* saved)
{
  PLUGIN_DEBUG ("GCJ_New");

  NPError np_error = NPERR_NO_ERROR;
  GCJPluginData* data = NULL;
  GError* channel_error = NULL;
  gchar* documentbase = NULL;
  gchar* read_message = NULL;
  gchar* applet_tag = NULL;
  gchar* tag_message = NULL;

  if (!instance)
    {
      PLUGIN_ERROR ("Browser-provided instance pointer is NULL.");
      np_error = NPERR_INVALID_INSTANCE_ERROR;
      goto cleanup_done;
    }

  // Initialize threads (needed for mutexes).
  if (!g_thread_supported ())
    g_thread_init (NULL);

  // data
  plugin_data_new (&data);
  if (data == NULL)
    {
      PLUGIN_ERROR ("Failed to allocate plugin data.");
      np_error = NPERR_OUT_OF_MEMORY_ERROR;
      goto cleanup_done;
    }

  // Initialize data->instance_string.
  //
  // instance_string should be unique for this process so we use a
  // combination of getpid and plugin_instance_counter.
  //
  // Critical region.  Reference and increment plugin_instance_counter
  // global.
  g_mutex_lock (plugin_instance_mutex);

  // data->instance_string
  data->instance_string = g_strdup_printf ("instance-%d-%ld",
                                           getpid (),
                                           plugin_instance_counter++);

  g_mutex_unlock (plugin_instance_mutex);

  // data->appletviewer_mutex
  data->appletviewer_mutex = g_mutex_new ();

  // Documentbase retrieval.
  documentbase = plugin_get_documentbase (instance);
  if (!documentbase)
    {
      PLUGIN_ERROR ("Documentbase retrieval failed."
                    " Browser not Mozilla-based?");
      goto cleanup_appletviewer_mutex;
    }

  // Create appletviewer-to-plugin pipe which we refer to as the input
  // pipe.

  // data->in_pipe_name
  data->in_pipe_name = g_strdup_printf (PIPE_DIRECTORY
                                        "/gcj-%s-appletviewer-to-plugin",
                                        data->instance_string);
  if (!data->in_pipe_name)
    {
      PLUGIN_ERROR ("Failed to create input pipe name.");
      np_error = NPERR_OUT_OF_MEMORY_ERROR;
      // If data->in_pipe_name is NULL then the g_free at
      // cleanup_in_pipe_name will simply return.
      goto cleanup_in_pipe_name;
    }

  if (mkfifo (data->in_pipe_name, 0700) == -1 && errno != EEXIST)
    {
      PLUGIN_ERROR_TWO ("Failed to create input pipe", strerror (errno));
      np_error = NPERR_GENERIC_ERROR;
      goto cleanup_in_pipe_name;
    }

  // Create plugin-to-appletviewer pipe which we refer to as the
  // output pipe.

  // data->out_pipe_name
  data->out_pipe_name = g_strdup_printf (PIPE_DIRECTORY
                                         "/gcj-%s-plugin-to-appletviewer",
                                         data->instance_string);

  if (!data->out_pipe_name)
    {
      PLUGIN_ERROR ("Failed to create output pipe name.");
      np_error = NPERR_OUT_OF_MEMORY_ERROR;
      goto cleanup_out_pipe_name;
    }

  if (mkfifo (data->out_pipe_name, 0700) == -1 && errno != EEXIST)
    {
      PLUGIN_ERROR_TWO ("Failed to create output pipe", strerror (errno));
      np_error = NPERR_GENERIC_ERROR;
      goto cleanup_out_pipe_name;
    }

  // Start a separate appletviewer process for each applet, even if
  // there are multiple applets in the same page.  We may need to
  // change this behaviour if we find pages with multiple applets that
  // rely on being run in the same VM.

  // Critical region.  Hold appletviewer_mutex while we start the
  // appletviewer, create the IO channels and install the channel
  // watch callbacks.
  g_mutex_lock (data->appletviewer_mutex);

  plugin_start_appletviewer (data);

  // Create plugin-to-appletviewer channel.  The default encoding for
  // the file is UTF-8.
  // data->out_to_appletviewer
  data->out_to_appletviewer = g_io_channel_new_file (data->out_pipe_name,
                                                     "w", &channel_error);
  if (!data->out_to_appletviewer)
    {
      PLUGIN_ERROR_TWO ("Failed to create output channel",
                        channel_error->message);
      np_error = NPERR_GENERIC_ERROR;
      goto cleanup_out_to_appletviewer;
    }
  if (channel_error)
    {
      g_error_free (channel_error);
      channel_error = NULL;
    }

  // Watch for hangup and error signals on the output pipe.
  data->out_watch_source =
    g_io_add_watch (data->out_to_appletviewer,
                    (GIOCondition) (G_IO_ERR | G_IO_HUP),
                    plugin_out_pipe_callback, (gpointer) data);

  // Create appletviewer-to-plugin channel.  The default encoding for
  // the file is UTF-8.
  // data->in_from_appletviewer
  data->in_from_appletviewer = g_io_channel_new_file (data->in_pipe_name,
                                                      "r", &channel_error);
  if (!data->in_from_appletviewer)
    {
      PLUGIN_ERROR_TWO ("Failed to create input channel",
                        channel_error->message);
      np_error = NPERR_GENERIC_ERROR;
      goto cleanup_in_from_appletviewer;
    }
  if (channel_error)
    {
      g_error_free (channel_error);
      channel_error = NULL;
    }

  // Watch for hangup and error signals on the input pipe.
  data->in_watch_source =
    g_io_add_watch (data->in_from_appletviewer,
                    (GIOCondition) (G_IO_IN | G_IO_ERR | G_IO_HUP),
                    plugin_in_pipe_callback, (gpointer) data);

  // Wait until we receive confirmation that the appletviewer has
  // started.
  if (g_io_channel_read_line (data->in_from_appletviewer,
                              &read_message, NULL, NULL,
                              &channel_error)
      != G_IO_STATUS_NORMAL)
    {
      PLUGIN_ERROR_TWO ("Receiving confirmation from appletviewer failed",
                        channel_error->message);
      np_error = NPERR_GENERIC_ERROR;
      goto cleanup_in_watch_source;
    }
  if (channel_error)
    {
      g_error_free (channel_error);
      channel_error = NULL;
    }

  PLUGIN_DEBUG ("GCJ_New: got confirmation that appletviewer is running.");
  data->appletviewer_alive = TRUE;

  // Send applet tag message to appletviewer.
  applet_tag = plugin_create_applet_tag (argc, argn, argv);
  tag_message = g_strconcat ("tag ", documentbase, " ", applet_tag, NULL);

  plugin_send_message_to_appletviewer (data, data->instance_string);
  plugin_send_message_to_appletviewer (data, tag_message);

  g_mutex_unlock (data->appletviewer_mutex);

  // If initialization succeeded entirely then we store the plugin
  // data in the instance structure and return.  Otherwise we free the
  // data we've allocated so far and set instance->pdata to NULL.

  // Set back-pointer to owner instance.
  data->owner = instance;
  instance->pdata = data;
  goto cleanup_done;

  // An error occurred while initializing the plugin data or spawning
  // the appletviewer so we free the data we've already allocated.

 cleanup_in_watch_source:
  // Removing a source is harmless if it fails since it just means the
  // source has already been removed.
  g_source_remove (data->in_watch_source);
  data->in_watch_source = 0;

 cleanup_in_from_appletviewer:
  if (data->in_from_appletviewer)
    g_io_channel_unref (data->in_from_appletviewer);
  data->in_from_appletviewer = NULL;

  // cleanup_out_watch_source:
  g_source_remove (data->out_watch_source);
  data->out_watch_source = 0;

 cleanup_out_to_appletviewer:
  if (data->out_to_appletviewer)
    g_io_channel_unref (data->out_to_appletviewer);
  data->out_to_appletviewer = NULL;

  // cleanup_out_pipe:
  // Delete output pipe.
  unlink (data->out_pipe_name);

 cleanup_out_pipe_name:
  g_free (data->out_pipe_name);
  data->out_pipe_name = NULL;

  // cleanup_in_pipe:
  // Delete input pipe.
  unlink (data->in_pipe_name);

 cleanup_in_pipe_name:
  g_free (data->in_pipe_name);
  data->in_pipe_name = NULL;

 cleanup_appletviewer_mutex:
  g_free (data->appletviewer_mutex);
  data->appletviewer_mutex = NULL;

  // cleanup_instance_string:
  g_free (data->instance_string);
  data->instance_string = NULL;

  // cleanup_data:
  // Eliminate back-pointer to plugin instance.
  data->owner = NULL;
  (*browserFunctions.memfree) (data);
  data = NULL;

  // Initialization failed so return a NULL pointer for the browser
  // data.
  instance->pdata = NULL;

 cleanup_done:

  g_free (tag_message);
  tag_message = NULL;
  g_free (applet_tag);
  applet_tag = NULL;
  g_free (read_message);
  read_message = NULL;
  g_free (documentbase);
  documentbase = NULL;

  PLUGIN_DEBUG ("GCJ_New return");

  return np_error;
}

NPError
GCJ_GetValue (NPP instance, NPPVariable variable, void* value)
{
  PLUGIN_DEBUG ("GCJ_GetValue");

  NPError np_error = NPERR_NO_ERROR;

  switch (variable)
    {
    // This plugin needs XEmbed support.
    case NPPVpluginNeedsXEmbed:
      {
        PLUGIN_DEBUG ("GCJ_GetValue: returning TRUE for NeedsXEmbed.");
        PRBool* bool_value = (PRBool*) value;
        *bool_value = PR_TRUE;
      }
      break;

    default:
      PLUGIN_ERROR ("Unknown plugin value requested.");
      np_error = NPERR_GENERIC_ERROR;
      break;
    }

  PLUGIN_DEBUG ("GCJ_GetValue return");

  return np_error;
}

NPError
GCJ_Destroy (NPP instance, NPSavedData** save)
{
  PLUGIN_DEBUG ("GCJ_Destroy");

  GCJPluginData* data = (GCJPluginData*) instance->pdata;

  if (data)
    {
      // Critical region.  Stop the appletviewer.
      g_mutex_lock (data->appletviewer_mutex);

      // Tell the appletviewer to destroy its embedded plugin window.
      plugin_send_message_to_appletviewer (data, "destroy");
      // Shut down the appletviewer.
      plugin_stop_appletviewer (data);

      g_mutex_unlock (data->appletviewer_mutex);

      // Free plugin data.
      plugin_data_destroy (&data);
    }

  PLUGIN_DEBUG ("GCJ_Destroy return");

  return NPERR_NO_ERROR;
}

NPError
GCJ_SetWindow (NPP instance, NPWindow* window)
{
  PLUGIN_DEBUG ("GCJ_SetWindow");

  if (instance == NULL)
    {
      PLUGIN_ERROR ("Invalid instance.");

      return NPERR_INVALID_INSTANCE_ERROR;
    }

  GCJPluginData* data = (GCJPluginData*) instance->pdata;

  // Simply return if we receive a NULL window.
  if ((window == NULL) || (window->window == NULL))
    {
      PLUGIN_DEBUG ("GCJ_SetWindow: got NULL window.");

      return NPERR_NO_ERROR;
    }

  if (data->window_handle)
    {
      // The window already exists.
      if (data->window_handle == window->window)
	{
          // The parent window is the same as in previous calls.
          PLUGIN_DEBUG ("GCJ_SetWindow: window already exists.");

          // Critical region.  Read data->appletviewer_mutex and send
          // a message to the appletviewer.
          g_mutex_lock (data->appletviewer_mutex);

	  if (data->appletviewer_alive)
	    {
	      // The window is the same as it was for the last
	      // SetWindow call.
	      if (window->width != data->window_width)
		{
                  PLUGIN_DEBUG ("GCJ_SetWindow: window width changed.");
		  // The width of the plugin window has changed.

                  // Send the new width to the appletviewer.
		  plugin_send_message_to_appletviewer (data,
                                                       data->instance_string);
                  gchar* width_message = g_strdup_printf ("width %d",
                                                          window->width);
		  plugin_send_message_to_appletviewer (data, width_message);
                  g_free (width_message);

                  // Store the new width.
                  data->window_width = window->width;
		}

	      if (window->height != data->window_height)
		{
                  PLUGIN_DEBUG ("GCJ_SetWindow: window height changed.");
		  // The height of the plugin window has changed.

                  // Send the new height to the appletviewer.
		  plugin_send_message_to_appletviewer (data,
                                                       data->instance_string);
                  gchar* height_message = g_strdup_printf ("height %d",
                                                           window->height);
		  plugin_send_message_to_appletviewer (data, height_message);
                  g_free (height_message);

                  // Store the new height.
                  data->window_height = window->height;
		}
	    }
	  else
	    {
              // The appletviewer is not running.
	      PLUGIN_DEBUG ("GCJ_SetWindow: appletviewer is not running.");
	    }

          g_mutex_unlock (data->appletviewer_mutex);
	}
      else
	{
	  // The parent window has changed.  This branch does run but
	  // doing nothing in response seems to be sufficient.
	  PLUGIN_DEBUG ("GCJ_SetWindow: parent window changed.");
	}
    }
  else
    {
      PLUGIN_DEBUG ("GCJ_SetWindow: setting window.");

      // Critical region.  Send messages to appletviewer.
      g_mutex_lock (data->appletviewer_mutex);

      plugin_send_message_to_appletviewer (data, data->instance_string);
      gchar *window_message = g_strdup_printf ("handle %ld",
                                               (gulong) window->window);
      plugin_send_message_to_appletviewer (data, window_message);
      g_free (window_message);

      g_mutex_unlock (data->appletviewer_mutex);

      // Store the window handle.
      data->window_handle = window->window;
    }

  PLUGIN_DEBUG ("GCJ_SetWindow return");

  return NPERR_NO_ERROR;
}

NPError
GCJ_NewStream (NPP instance, NPMIMEType type, NPStream* stream,
               NPBool seekable, uint16* stype)
{
  PLUGIN_DEBUG ("GCJ_NewStream");

  PLUGIN_DEBUG ("GCJ_NewStream return");

  return NPERR_NO_ERROR;
}

void
GCJ_StreamAsFile (NPP instance, NPStream* stream, const char* filename)
{
  PLUGIN_DEBUG ("GCJ_StreamAsFile");

  PLUGIN_DEBUG ("GCJ_StreamAsFile return");
}

NPError
GCJ_DestroyStream (NPP instance, NPStream* stream, NPReason reason)
{
  PLUGIN_DEBUG ("GCJ_DestroyStream");

  PLUGIN_DEBUG ("GCJ_DestroyStream return");

  return NPERR_NO_ERROR;
}

int32
GCJ_WriteReady (NPP instance, NPStream* stream)
{
  PLUGIN_DEBUG ("GCJ_WriteReady");

  PLUGIN_DEBUG ("GCJ_WriteReady return");

  return 0;
}

int32
GCJ_Write (NPP instance, NPStream* stream, int32 offset, int32 len,
           void* buffer)
{
  PLUGIN_DEBUG ("GCJ_Write");

  PLUGIN_DEBUG ("GCJ_Write return");

  return 0;
}

void
GCJ_Print (NPP instance, NPPrint* platformPrint)
{
  PLUGIN_DEBUG ("GCJ_Print");

  PLUGIN_DEBUG ("GCJ_Print return");
}

int16
GCJ_HandleEvent (NPP instance, void* event)
{
  PLUGIN_DEBUG ("GCJ_HandleEvent");

  PLUGIN_DEBUG ("GCJ_HandleEvent return");

  return 0;
}

void
GCJ_URLNotify (NPP instance, const char* url, NPReason reason,
               void* notifyData)
{
  PLUGIN_DEBUG ("GCJ_URLNotify");

  PLUGIN_DEBUG ("GCJ_URLNotify return");
}

jref
GCJ_GetJavaClass (void)
{
  PLUGIN_DEBUG ("GCJ_GetJavaClass");

  PLUGIN_DEBUG ("GCJ_GetJavaClass return");

  return 0;
}

// HELPER FUNCTIONS

static void
plugin_data_new (GCJPluginData** data)
{
  PLUGIN_DEBUG ("plugin_data_new");

  *data = (GCJPluginData*)
    (*browserFunctions.memalloc) (sizeof (struct GCJPluginData));

  // appletviewer_alive is false until the applet viewer is spawned.
  if (*data)
    memset (*data, 0, sizeof (struct GCJPluginData));

  PLUGIN_DEBUG ("plugin_data_new return");
}

// Documentbase retrieval.  This function gets the current document's
// documentbase.  This function relies on browser-private data so it
// will only work when the plugin is loaded in a Mozilla-based
// browser.  We could not find a way to retrieve the documentbase
// using the original Netscape plugin API so we use the XPCOM API
// instead.
static gchar*
plugin_get_documentbase (NPP instance)
{
  PLUGIN_DEBUG ("plugin_get_documentbase");

  nsIPluginInstance* xpcom_instance = NULL;
  nsIPluginInstancePeer* peer = NULL;
  nsresult result = 0;
  nsIPluginTagInfo2* pluginTagInfo2 = NULL;
  info_union u = { NULL };
  char const* documentbase = NULL;
  gchar* documentbase_copy = NULL;

  xpcom_instance = (nsIPluginInstance*) (instance->ndata);
  if (!xpcom_instance)
    {
      PLUGIN_ERROR ("xpcom_instance is NULL.");
      goto cleanup_done;
    }

  xpcom_instance->GetPeer (&peer);
  if (!peer)
    {
      PLUGIN_ERROR ("peer is NULL.");
      goto cleanup_done;
    }

  u.info_field = &pluginTagInfo2;

  result = peer->QueryInterface (kIPluginTagInfo2IID,
                                 u.void_field);
  if (result || !pluginTagInfo2)
    {
      PLUGIN_ERROR ("pluginTagInfo2 retrieval failed.");
      goto cleanup_peer;
    }

  pluginTagInfo2->GetDocumentBase (&documentbase);

  if (!documentbase)
    {
      PLUGIN_ERROR ("documentbase is NULL.");
      goto cleanup_plugintaginfo2;
    }

  documentbase_copy = g_strdup (documentbase);

  // Release references.
 cleanup_plugintaginfo2:
  NS_RELEASE (pluginTagInfo2);

 cleanup_peer:
  NS_RELEASE (peer);

 cleanup_done:
  PLUGIN_DEBUG ("plugin_get_documentbase return");

  return documentbase_copy;
}

// plugin_in_pipe_callback is called when data is available on the
// input pipe, or when the appletviewer crashes or is killed.  It may
// be called after data has been destroyed in which case it simply
// returns FALSE to remove itself from the glib main loop.
static gboolean
plugin_in_pipe_callback (GIOChannel* source,
                         GIOCondition condition,
                         gpointer plugin_data)
{
  PLUGIN_DEBUG ("plugin_in_pipe_callback");

  GCJPluginData* data = (GCJPluginData*) plugin_data;
  gboolean keep_installed = TRUE;

  // If data is NULL then GCJ_Destroy has already been called and
  // plugin_in_pipe_callback is being called after plugin
  // destruction.  In that case all we need to do is return FALSE so
  // that the plugin_in_pipe_callback watch is removed.
  if (data)
    {
      // Critical region. Set or clear data->appletviewer_alive.
      g_mutex_lock (data->appletviewer_mutex);

      if (condition & G_IO_IN)
        {
          GError* channel_error = NULL;
          gchar* message = NULL;

          if (g_io_channel_read_line (data->in_from_appletviewer,
                                      &message, NULL, NULL,
                                      &channel_error)
              != G_IO_STATUS_NORMAL)
            {
              PLUGIN_ERROR_TWO ("Failed to read line from input channel",
                                channel_error->message);
            }
          else
            {
              if (g_str_has_prefix (message, "url "))
                {
                  gchar** parts = g_strsplit (message, " ", 3);
                  PLUGIN_DEBUG_TWO ("plugin_in_pipe_callback:"
                                    " opening URL", parts[1]);
                  PLUGIN_DEBUG_TWO ("plugin_in_pipe_callback:"
                                    " URL target", parts[2]);
                  // Open the URL in a new browser window.
                  NPError np_error =
                    (*browserFunctions.geturl) (data->owner, parts[1], parts[2]);
                  if (np_error != NPERR_NO_ERROR)
                    PLUGIN_ERROR ("Failed to load URL.");
                  g_strfreev (parts);
                  parts = NULL;
                }
              else if (g_str_has_prefix (message, "status "))
                {
                  gchar** parts = g_strsplit (message, " ", 2);

                  PLUGIN_DEBUG_TWO ("plugin_in_pipe_callback:"
                                    " setting status", parts[1]);
                  (*browserFunctions.status) (data->owner, parts[1]);
                  g_strfreev (parts);
                  parts = NULL;
                }
              g_print ("  PIPE: plugin read %s\n", message);
            }

          if (channel_error)
            {
              g_error_free (channel_error);
              channel_error = NULL;
            }

          g_free (message);
          message = NULL;

          keep_installed = TRUE;
        }

      if (condition & (G_IO_ERR | G_IO_HUP))
        {
          PLUGIN_DEBUG ("appletviewer has stopped.");
          data->appletviewer_alive = FALSE;
          keep_installed = FALSE;
        }
      g_mutex_unlock (data->appletviewer_mutex);
    }

  PLUGIN_DEBUG ("plugin_in_pipe_callback return");

  return keep_installed;
}

// plugin_out_pipe_callback is called when the appletviewer crashes or
// is killed.  It may be called after data has been destroyed in which
// case it simply returns FALSE to remove itself from the glib main
// loop.
static gboolean
plugin_out_pipe_callback (GIOChannel* source,
                          GIOCondition condition,
                          gpointer plugin_data)
{
  PLUGIN_DEBUG ("plugin_out_pipe_callback");

  GCJPluginData* data = (GCJPluginData*) plugin_data;

  // If data is NULL then GCJ_Destroy has already been called and
  // plugin_out_pipe_callback is being called after plugin
  // destruction.  In that case all we need to do is return FALSE so
  // that the plugin_out_pipe_callback watch is removed.
  if (data)
    {
      // Critical region.  Clear data->appletviewer_alive.
      g_mutex_lock (data->appletviewer_mutex);

      PLUGIN_DEBUG ("plugin_out_pipe_callback: appletviewer has stopped.");
      data->appletviewer_alive = FALSE;

      g_mutex_unlock (data->appletviewer_mutex);
    }

  PLUGIN_DEBUG ("plugin_out_pipe_callback return");

  return FALSE;
}

static void
plugin_start_appletviewer (GCJPluginData* data)
{
  PLUGIN_DEBUG ("plugin_start_appletviewer");

  if (!data->appletviewer_alive)
    {
      GError* spawn_error = NULL;
      gchar* command_line[3] = { NULL, NULL, NULL };

      command_line[0] = g_strdup (APPLETVIEWER_EXECUTABLE);
      // Output from plugin's perspective is appletviewer's input.
      // Input from plugin's perspective is appletviewer's output.
      command_line[1] = g_strdup_printf ("--plugin=%s,%s",
                                         data->out_pipe_name,
                                         data->in_pipe_name);
      command_line[2] = NULL;

      if (!g_spawn_async (NULL, command_line, NULL, (GSpawnFlags) 0,
                          NULL, NULL, NULL, &spawn_error))
        {
          PLUGIN_ERROR_TWO ("Failed to spawn applet viewer",
                            spawn_error->message);
          goto cleanup;
        }

    cleanup:
      g_free (command_line[0]);
      g_free (command_line[1]);
      g_free (command_line[2]);
      if (spawn_error)
        {
          g_error_free (spawn_error);
          spawn_error = NULL;
        }
    }

  PLUGIN_DEBUG ("plugin_start_appletviewer return");
}

// Build up the applet tag string that we'll send to the applet
// viewer.
static gchar*
plugin_create_applet_tag (int16 argc, char* argn[], char* argv[])
{
  PLUGIN_DEBUG ("plugin_create_applet_tag");

  gchar* applet_tag = g_strdup ("<EMBED ");
  gchar* parameters = g_strdup ("");

  for (int16 i = 0; i < argc; i++)
    {
      if (!g_ascii_strcasecmp (argn[i], "code"))
        {
          gchar* code = g_strdup_printf ("CODE=\"%s\" ", argv[i]);
	  applet_tag = g_strconcat (applet_tag, code, NULL);
          g_free (code);
	}
      else if (!g_ascii_strcasecmp (argn[i], "codebase"))
	{
          gchar* codebase = g_strdup_printf ("CODEBASE=\"%s\" ", argv[i]);
	  applet_tag = g_strconcat (applet_tag, codebase, NULL);
          g_free (codebase);
	}
      else if (!g_ascii_strcasecmp (argn[i], "archive"))
	{
          gchar* archive = g_strdup_printf ("ARCHIVE=\"%s\" ", argv[i]);
	  applet_tag = g_strconcat (applet_tag, archive, NULL);
          g_free (archive);
	}
      else if (!g_ascii_strcasecmp (argn[i], "width"))
	{
          gchar* width = g_strdup_printf ("WIDTH=\"%s\" ", argv[i]);
	  applet_tag = g_strconcat (applet_tag, width, NULL);
          g_free (width);
	}
      else if (!g_ascii_strcasecmp (argn[i], "height"))
	{
          gchar* height = g_strdup_printf ("HEIGHT=\"%s\" ", argv[i]);
	  applet_tag = g_strconcat (applet_tag, height, NULL);
          g_free (height);
	}
      else
        {
          // Escape the parameter value so that line termination
          // characters will pass through the pipe.
          if (argv[i] != '\0')
            {
              gchar* escaped = g_strescape (argv[i], NULL);
              parameters = g_strconcat (parameters, "<PARAM NAME=\"", argn[i],
                                        "\" VALUE=\"", escaped, "\">", NULL);
              g_free (escaped);
            }
        }
    }

  applet_tag = g_strconcat (applet_tag, ">", parameters, "</EMBED>", NULL);

  g_free (parameters);
  parameters = NULL;

  PLUGIN_DEBUG ("plugin_create_applet_tag return");

  return applet_tag;
}

// plugin_send_message_to_appletviewer must be called while holding
// data->appletviewer_mutex.
static void
plugin_send_message_to_appletviewer (GCJPluginData* data, gchar const* message)
{
  PLUGIN_DEBUG ("plugin_send_message_to_appletviewer");

  if (data->appletviewer_alive)
    {
      GError* channel_error = NULL;
      gchar* newline_message = NULL;
      gsize bytes_written = 0;

      // Send message to appletviewer.
      newline_message = g_strdup_printf ("%s\n", message);

      // g_io_channel_write_chars will return something other than
      // G_IO_STATUS_NORMAL if not all the data is written.  In that
      // case we fail rather than retrying.
      if (g_io_channel_write_chars (data->out_to_appletviewer,
                                    newline_message, -1, &bytes_written,
                                    &channel_error)
          != G_IO_STATUS_NORMAL)
        PLUGIN_ERROR_TWO ("Failed to write bytes to output channel",
                          channel_error->message);

      if (channel_error)
        {
          g_error_free (channel_error);
          channel_error = NULL;
        }

      if (g_io_channel_flush (data->out_to_appletviewer, &channel_error)
          != G_IO_STATUS_NORMAL)
        PLUGIN_ERROR_TWO ("Failed to flush bytes to output channel",
                          channel_error->message);

      if (channel_error)
        {
          g_error_free (channel_error);
          channel_error = NULL;
        }
      g_free (newline_message);

      g_print ("  PIPE: plugin wrote %s\n", message);
    }

  PLUGIN_DEBUG ("plugin_send_message_to_appletviewer return");
}

// Stop the appletviewer process.  When this is called the
// appletviewer can be in any of three states: running, crashed or
// hung.  If the appletviewer is running then sending it "shutdown"
// will cause it to exit.  This will cause
// plugin_out_pipe_callback/plugin_in_pipe_callback to be called and
// the input and output channels to be shut down.  If the appletviewer
// has crashed then plugin_out_pipe_callback/plugin_in_pipe_callback
// would already have been called and data->appletviewer_alive cleared
// in which case this function simply returns.  If the appletviewer is
// hung then this function will be successful and the input and output
// watches will be removed by plugin_data_destroy.
// plugin_stop_appletviewer must be called with
// data->appletviewer_mutex held.
static void
plugin_stop_appletviewer (GCJPluginData* data)
{
  PLUGIN_DEBUG ("plugin_stop_appletviewer");

  if (data->appletviewer_alive)
    {
      // Shut down the appletviewer.
      GError* channel_error = NULL;
      gsize bytes_written = 0;

      if (data->out_to_appletviewer)
        {
          if (g_io_channel_write_chars (data->out_to_appletviewer, "shutdown",
                                        -1, &bytes_written, &channel_error)
              != G_IO_STATUS_NORMAL)
            PLUGIN_ERROR_TWO ("Failed to write shutdown message to"
                              " appletviewer", channel_error->message);

          if (channel_error)
            {
              g_error_free (channel_error);
              channel_error = NULL;
            }

          if (g_io_channel_flush (data->out_to_appletviewer, &channel_error)
              != G_IO_STATUS_NORMAL)
            PLUGIN_ERROR_TWO ("Failed to write shutdown message to"
                              " appletviewer", channel_error->message);

          if (channel_error)
            {
              g_error_free (channel_error);
              channel_error = NULL;
            }

          if (g_io_channel_shutdown (data->out_to_appletviewer,
                                     TRUE, &channel_error)
              != G_IO_STATUS_NORMAL)
            PLUGIN_ERROR_TWO ("Failed to shut down appletviewer"
                              " output channel", channel_error->message);

          if (channel_error)
            {
              g_error_free (channel_error);
              channel_error = NULL;
            }
        }

      if (data->in_from_appletviewer)
        {
          if (g_io_channel_shutdown (data->in_from_appletviewer,
                                     TRUE, &channel_error)
              != G_IO_STATUS_NORMAL)
            PLUGIN_ERROR_TWO ("Failed to shut down appletviewer"
                              " input channel", channel_error->message);

          if (channel_error)
            {
              g_error_free (channel_error);
              channel_error = NULL;
            }
        }
    }

  PLUGIN_DEBUG ("plugin_stop_appletviewer return");
}

static void
plugin_data_destroy (GCJPluginData** data)
{
  PLUGIN_DEBUG ("plugin_data_destroy");

  GCJPluginData* tofree = *data;

  tofree->window_handle = NULL;
  tofree->window_height = 0;
  tofree->window_width = 0;

  // Copied from GCJ_New.

  // cleanup_in_watch_source:
  // Removing a source is harmless if it fails since it just means the
  // source has already been removed.
  g_source_remove (tofree->in_watch_source);
  tofree->in_watch_source = 0;

  // cleanup_in_from_appletviewer:
  if (tofree->in_from_appletviewer)
    g_io_channel_unref (tofree->in_from_appletviewer);
  tofree->in_from_appletviewer = NULL;

  // cleanup_out_watch_source:
  g_source_remove (tofree->out_watch_source);
  tofree->out_watch_source = 0;

  // cleanup_out_to_appletviewer:
  if (tofree->out_to_appletviewer)
    g_io_channel_unref (tofree->out_to_appletviewer);
  tofree->out_to_appletviewer = NULL;

  // cleanup_out_pipe:
  // Delete output pipe.
  unlink (tofree->out_pipe_name);

  // cleanup_out_pipe_name:
  g_free (tofree->out_pipe_name);
  tofree->out_pipe_name = NULL;

  // cleanup_in_pipe:
  // Delete input pipe.
  unlink (tofree->in_pipe_name);

  // cleanup_in_pipe_name:
  g_free (tofree->in_pipe_name);
  tofree->in_pipe_name = NULL;

  // cleanup_appletviewer_mutex:
  g_free (tofree->appletviewer_mutex);
  tofree->appletviewer_mutex = NULL;

  // cleanup_instance_string:
  g_free (tofree->instance_string);
  tofree->instance_string = NULL;

  // cleanup_data:
  // Eliminate back-pointer to plugin instance.
  tofree->owner = NULL;
  (*browserFunctions.memfree) (tofree);
  tofree = NULL;

  PLUGIN_DEBUG ("plugin_data_destroy return");
}

// FACTORY FUNCTIONS

// Provides the browser with pointers to the plugin functions that we
// implement and initializes a local table with browser functions that
// we may wish to call.  Called once, after browser startup and before
// the first plugin instance is created.
NPError
NP_Initialize (NPNetscapeFuncs* browserTable, NPPluginFuncs* pluginTable)
{
  PLUGIN_DEBUG ("NP_Initialize");

  if ((browserTable == NULL) || (pluginTable == NULL))
    {
      PLUGIN_ERROR ("Browser or plugin function table is NULL.");

      return NPERR_INVALID_FUNCTABLE_ERROR;
    }

  // Ensure that the major version of the plugin API that the browser
  // expects is not more recent than the major version of the API that
  // we've implemented.
  if ((browserTable->version >> 8) > NP_VERSION_MAJOR)
    {
      PLUGIN_ERROR ("Incompatible version.");

      return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }

  // Ensure that the plugin function table we've received is large
  // enough to store the number of functions that we may provide.
  if (pluginTable->size < sizeof (NPPluginFuncs))      
    {
      PLUGIN_ERROR ("Invalid plugin function table.");

      return NPERR_INVALID_FUNCTABLE_ERROR;
    }

  // Ensure that the browser function table is large enough to store
  // the number of browser functions that we may use.
  if (browserTable->size < sizeof (NPNetscapeFuncs))
    {
      PLUGIN_ERROR ("Invalid browser function table.");

      return NPERR_INVALID_FUNCTABLE_ERROR;
    }

  // Store in a local table the browser functions that we may use.
  browserFunctions.version = browserTable->version;
  browserFunctions.size = browserTable->size;
  browserFunctions.posturl = browserTable->posturl;
  browserFunctions.geturl = browserTable->geturl;
  browserFunctions.geturlnotify = browserTable->geturlnotify;
  browserFunctions.requestread = browserTable->requestread;
  browserFunctions.newstream = browserTable->newstream;
  browserFunctions.write = browserTable->write;
  browserFunctions.destroystream = browserTable->destroystream;
  browserFunctions.status = browserTable->status;
  browserFunctions.uagent = browserTable->uagent;
  browserFunctions.memalloc = browserTable->memalloc;
  browserFunctions.memfree = browserTable->memfree;
  browserFunctions.memflush = browserTable->memflush;
  browserFunctions.reloadplugins = browserTable->reloadplugins;
  browserFunctions.getvalue = browserTable->getvalue;

  // Return to the browser the plugin functions that we implement.
  pluginTable->version = (NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR;
  pluginTable->size = sizeof (NPPluginFuncs);
  pluginTable->newp = NewNPP_NewProc (GCJ_New);
  pluginTable->destroy = NewNPP_DestroyProc (GCJ_Destroy);
  pluginTable->setwindow = NewNPP_SetWindowProc (GCJ_SetWindow);
  pluginTable->newstream = NewNPP_NewStreamProc (GCJ_NewStream);
  pluginTable->destroystream = NewNPP_DestroyStreamProc (GCJ_DestroyStream);
  pluginTable->asfile = NewNPP_StreamAsFileProc (GCJ_StreamAsFile);
  pluginTable->writeready = NewNPP_WriteReadyProc (GCJ_WriteReady);
  pluginTable->write = NewNPP_WriteProc (GCJ_Write);
  pluginTable->print = NewNPP_PrintProc (GCJ_Print);
  pluginTable->urlnotify = NewNPP_URLNotifyProc (GCJ_URLNotify);
  pluginTable->getvalue = NewNPP_GetValueProc (GCJ_GetValue);

  plugin_instance_mutex = g_mutex_new ();

  PLUGIN_DEBUG ("NP_Initialize: using " APPLETVIEWER_EXECUTABLE ".");

  PLUGIN_DEBUG ("NP_Initialize return");

  return NPERR_NO_ERROR;
}

// Returns a string describing the MIME type that this plugin
// handles.
char*
NP_GetMIMEDescription (void)
{
  PLUGIN_DEBUG ("NP_GetMIMEDescription");

  PLUGIN_DEBUG ("NP_GetMIMEDescription return");

  return (char*) PLUGIN_MIME_DESC;
}

// Returns a value relevant to the plugin as a whole.  The browser
// calls this function to obtain information about the plugin.
NPError
NP_GetValue (void* future, NPPVariable variable, void* value)
{
  PLUGIN_DEBUG ("NP_GetValue");

  NPError result = NPERR_NO_ERROR;
  gchar** char_value = (gchar**) value;

  switch (variable)
    {
    case NPPVpluginNameString:
      PLUGIN_DEBUG ("NP_GetValue: returning plugin name.");
      *char_value = g_strdup (PLUGIN_NAME " " PACKAGE_VERSION);
      break;

    case NPPVpluginDescriptionString:
      PLUGIN_DEBUG ("NP_GetValue: returning plugin description.");
      *char_value = g_strdup (PLUGIN_DESC);
      break;

    default:
      PLUGIN_ERROR ("Unknown plugin value requested.");
      result = NPERR_GENERIC_ERROR;
      break;
    }

  PLUGIN_DEBUG ("NP_GetValue return");

  return result;
}

// Shuts down the plugin.  Called after the last plugin instance is
// destroyed.
NPError
NP_Shutdown (void)
{
  PLUGIN_DEBUG ("NP_Shutdown");

  // Free mutex.
  g_mutex_free (plugin_instance_mutex);
  plugin_instance_mutex = NULL;

  PLUGIN_DEBUG ("NP_Shutdown return");

  return NPERR_NO_ERROR;
}
