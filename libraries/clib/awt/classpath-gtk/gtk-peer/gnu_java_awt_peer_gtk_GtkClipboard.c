/* gtkclipboard.c
   Copyright (C) 1998, 1999, 2005 Free Software Foundation, Inc.

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


#include "jcl.h"
#include "gtkpeer.h"
#include "gnu_java_awt_peer_gtk_GtkClipboard.h"
#include "gnu_java_awt_peer_gtk_GtkSelection.h"

#define OBJECT_TARGET 1
#define TEXT_TARGET   2
#define IMAGE_TARGET  3
#define URI_TARGET    4

static GtkClipboard *clipboard;

/* Simple id to keep track of the selection we are currently managing. */
static int current_selection = 0;

/* Whether we "own" the clipboard. And may clear it. */
static int owner = 0;

static jclass gtk_clipboard_class;
static jmethodID setSystemContentsID;

static jobject gtk_clipboard_instance = NULL;
static jmethodID provideContentID;
static jmethodID provideTextID;
static jmethodID provideImageID;
static jmethodID provideURIsID;

static jstring stringTarget;
static jstring imageTarget;
static jstring filesTarget;

static void
cp_gtk_clipboard_owner_change (GtkClipboard *clipboard __attribute__((unused)),
			       GdkEvent *event __attribute__((unused)),
			       gpointer user_data __attribute__((unused)))
{
  /* These are only interesting when we are not the owner. Otherwise
     we will have the set and clear functions doing the updating. */
  JNIEnv *env = cp_gtk_gdk_env ();
  if (!owner)
    (*env)->CallStaticVoidMethod (env, gtk_clipboard_class,
				  setSystemContentsID);
}

JNIEXPORT jboolean JNICALL 
Java_gnu_java_awt_peer_gtk_GtkClipboard_initNativeState (JNIEnv *env,
							 jclass gtkclipboard,
							 jstring string,
							 jstring image,
							 jstring files)
{
  GdkDisplay* display;
  jboolean can_cache;

  gtk_clipboard_class = gtkclipboard;
  setSystemContentsID = (*env)->GetStaticMethodID (env, gtk_clipboard_class,
						   "setSystemContents",
						   "()V");
  if (setSystemContentsID == NULL)
    return JNI_FALSE;

  stringTarget = (*env)->NewGlobalRef(env, string);
  imageTarget = (*env)->NewGlobalRef(env, image);
  filesTarget = (*env)->NewGlobalRef(env, files);

  gdk_threads_enter ();
  clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);

  display = gtk_clipboard_get_display (clipboard);
  if (gdk_display_supports_selection_notification (display))
    {
      g_signal_connect (clipboard, "owner-change",
			G_CALLBACK (cp_gtk_clipboard_owner_change), NULL);
      gdk_display_request_selection_notification (display,
						  GDK_SELECTION_CLIPBOARD);
      can_cache = JNI_TRUE;
    }
  else
    can_cache = JNI_FALSE;
  gdk_threads_leave ();

  return can_cache;
}

static void
cp_gtk_clipboard_get_func (GtkClipboard *clipboard __attribute__((unused)),
			   GtkSelectionData *selection,
			   guint info,
			   gpointer user_data __attribute__((unused)))
{
  JNIEnv *env = cp_gtk_gdk_env ();
  
  if (info == OBJECT_TARGET)
    {
      const gchar *target_name;
      jstring target_string;
      jbyteArray bytes;
      jint len;
      jbyte *barray;

      target_name = gdk_atom_name (selection->target);
      if (target_name == NULL)
	return;
      target_string = (*env)->NewStringUTF (env, target_name);
      if (target_string == NULL)
	return;
      bytes = (*env)->CallObjectMethod(env,
				       gtk_clipboard_instance,
				       provideContentID,
				       target_string);
      if (bytes == NULL)
	return;
      len = (*env)->GetArrayLength(env, bytes);
      if (len <= 0)
	return;
      barray = (*env)->GetByteArrayElements(env, bytes, NULL);
      if (barray == NULL)
	return;
      gtk_selection_data_set (selection, selection->target, 8,
			      (guchar *) barray, len);

      (*env)->ReleaseByteArrayElements(env, bytes, barray, 0);

    }
  else if (info == TEXT_TARGET)
    {
      jstring string;
      const gchar *text;
      int len;
      string = (*env)->CallObjectMethod(env,
					gtk_clipboard_instance,
					provideTextID);
      if (string == NULL)
	return;
      len = (*env)->GetStringUTFLength (env, string);
      if (len == -1)
	return;
      text = (*env)->GetStringUTFChars (env, string, NULL);
      if (text == NULL)
	return;

      gtk_selection_data_set_text (selection, text, len);
      (*env)->ReleaseStringUTFChars (env, string, text);
    }
  else if (info == IMAGE_TARGET)
    {
      jobject gtkimage;
      GdkPixbuf *pixbuf = NULL;
      
      gtkimage = (*env)->CallObjectMethod(env,
					  gtk_clipboard_instance,
					  provideImageID);
      if (gtkimage == NULL)
	return;
      
      pixbuf = cp_gtk_image_get_pixbuf (env, gtkimage);
      if (pixbuf != NULL)
	{
	  gtk_selection_data_set_pixbuf (selection, pixbuf);

	  /* if the GtkImage is offscreen, this is a temporary pixbuf
	     which should be thrown out. */
	  if(cp_gtk_image_is_offscreen (env, gtkimage) == JNI_TRUE)
	    gdk_pixbuf_unref (pixbuf);
	}
    }
  else if (info == URI_TARGET)
    {
      jobjectArray uris;
      jint count;
      int i;
      gchar **list;

      uris = (*env)->CallObjectMethod(env,
				      gtk_clipboard_instance,
				      provideURIsID);
      if (uris == NULL)
	return;
      count = (*env)->GetArrayLength (env, uris);
      if (count <= 0)
	return;

      list = (gchar **) JCL_malloc (env, (count + 1) * sizeof (gchar *));
      for (i = 0; i < count; i++)
	{
	  const char *text;
	  jstring uri;
	  
	  /* Mark NULL in so case of some error we can find the end. */
	  list[i] = NULL;
	  uri = (*env)->GetObjectArrayElement (env, uris, i);
	  if (uri == NULL)
	    break;
	  text = (*env)->GetStringUTFChars (env, uri, NULL);
	  if (text == NULL)
	    break;
	  list[i] = strdup (text);
	  (*env)->ReleaseStringUTFChars (env, uri, text);
	}

      if (i == count)
	{
	  list[count] = NULL;
	  gtk_selection_data_set_uris (selection, list);
	}

      for (i = 0; list[i] != NULL; i++)
	free (list[i]);
      JCL_free (env, list);
    }
}

static void
cp_gtk_clipboard_clear_func (GtkClipboard *clipboard __attribute__((unused)),
			     gpointer user_data)
{
  if (owner && (int) user_data == current_selection)
    {
      JNIEnv *env = cp_gtk_gdk_env();
      owner = 0;
      (*env)->CallStaticVoidMethod (env, gtk_clipboard_class,
				    setSystemContentsID);
    }
}

JNIEXPORT void JNICALL
Java_gnu_java_awt_peer_gtk_GtkClipboard_advertiseContent
(JNIEnv *env,
 jobject instance,
 jobjectArray mime_array,
 jboolean add_text,
 jboolean add_images __attribute__((unused)),
 jboolean add_uris __attribute__((unused)))
{
  GtkTargetList *target_list;
  GList *list;
  GtkTargetEntry *targets;
  gint n, i;

  gdk_threads_enter ();
  target_list = gtk_target_list_new (NULL, 0);

  if (mime_array != NULL)
    {
      n = (*env)->GetArrayLength (env, mime_array);
      for (i = 0; i < n; i++)
	{
	  const char *text;
	  jstring target;
	  GdkAtom atom;

	  target = (*env)->GetObjectArrayElement (env, mime_array, i);
	  if (target == NULL)
	    break;
	  text = (*env)->GetStringUTFChars (env, target, NULL);
	  if (text == NULL)
	    break;

	  atom = gdk_atom_intern (text, FALSE);
	  gtk_target_list_add (target_list, atom, 0, OBJECT_TARGET);

	  (*env)->ReleaseStringUTFChars (env, target, text);
	}
    }

  /* Add extra targets that gtk+ can provide/translate for us. */
  if (add_text)
    gtk_target_list_add_text_targets (target_list, TEXT_TARGET);
  if (add_images)
    gtk_target_list_add_image_targets (target_list, IMAGE_TARGET, TRUE);
  if (add_uris)
    gtk_target_list_add_uri_targets (target_list, URI_TARGET);

  /* Turn list into a target table. */
  n = g_list_length (target_list->list);
  if (n > 0)
    {
      targets = g_new (GtkTargetEntry, n);
      for (list = target_list->list, i = 0;
	   list != NULL;
	   list = list->next, i++)
	{
	  GtkTargetPair *pair = (GtkTargetPair *) list->data;
	  targets[i].target = gdk_atom_name (pair->target);
	  targets[i].flags = pair->flags;
	  targets[i].info = pair->info;
	}

      /* Set the targets plus callback functions and ask for the clipboard
	 to be stored when the application exists. */
      current_selection++;
      if (gtk_clipboard_set_with_data (clipboard, targets, n,
				       cp_gtk_clipboard_get_func,
				       cp_gtk_clipboard_clear_func,
				       (gpointer) current_selection))
	{
	  owner = 1;
	  if (gtk_clipboard_instance == NULL)
	    {
	      JNIEnv *env = cp_gtk_gdk_env ();
	      gtk_clipboard_instance =  (*env)->NewGlobalRef(env, instance);

	      provideContentID
		= (*env)->GetMethodID (env, gtk_clipboard_class,
				       "provideContent",
				       "(Ljava/lang/String;)[B");
	      if (provideContentID == NULL)
		return;

	      provideTextID
		= (*env)->GetMethodID (env, gtk_clipboard_class,
				       "provideText", "()Ljava/lang/String;");
	      if (provideTextID == NULL)
		return;

	      provideImageID
		= (*env)->GetMethodID (env, gtk_clipboard_class,
				       "provideImage",
				       "()Lgnu/java/awt/peer/gtk/GtkImage;");
	      if (provideImageID == NULL)
		return;

	      provideURIsID
		= (*env)->GetMethodID (env, gtk_clipboard_class,
				       "provideURIs",
				       "()[Ljava/lang/String;");
	      if (provideURIsID == NULL)
		return;
	    }
	  gtk_clipboard_set_can_store (clipboard, NULL, 0);
	}
      else
	{
	  owner = 0;
	  (*env)->CallStaticVoidMethod (env, gtk_clipboard_class,
					setSystemContentsID);
	}

      for (i = 0; i < n; i++)
	g_free (targets[i].target);
      g_free (targets);
    }
  else if (owner)
    {
      gtk_clipboard_clear (clipboard);
      owner = 0;
    }

  gtk_target_list_unref (target_list);
  gdk_threads_leave ();
}


/* GtkSelection native methods. Put here for convenience since they
   need access to the current clipboard. */

static jmethodID mimeTypesAvailableID;

/* Note this is actually just a GtkClipboardReceivedFunc, not a real
   GtkClipboardTargetsReceivedFunc, see requestMimeTypes. */
static void
cp_gtk_clipboard_targets_received (GtkClipboard *clipboard
				   __attribute__((unused)),
				   GtkSelectionData *target_data,
				   gpointer selection)
{
  GdkAtom *targets = NULL;
  gint targets_len = 0;
  gchar **target_strings = NULL;
  jobjectArray strings = NULL;
  int strings_len = 0;
  gboolean include_text = FALSE;
  gboolean include_image = FALSE;
  gboolean include_uris = FALSE;
  jobject selection_obj = (jobject) selection;
  JNIEnv *env = cp_gtk_gdk_env ();

  if (target_data != NULL && target_data->length > 0)
    {
      include_text = gtk_selection_data_targets_include_text (target_data);
      include_image = gtk_selection_data_targets_include_image (target_data,
								TRUE);
      if (gtk_selection_data_get_targets (target_data, &targets, &targets_len))
	{
	  int i;
	  GdkAtom uri_list_atom = gdk_atom_intern ("text/uri-list", FALSE);
	  target_strings = g_new (gchar*, targets_len);
	  if (target_strings != NULL)
	    for (i = 0; i < targets_len; i++)
	      {
		gchar *name =  gdk_atom_name (targets[i]);
		if (strchr (name, '/') != NULL)
		  {
		    target_strings[i] = name;
		    strings_len++;
		    if (! include_uris && targets[i] == uri_list_atom)
		      include_uris = TRUE;
		  }
		else
		  target_strings[i] = NULL;
	      }
	}

      if (target_strings != NULL)
	{
	  int i = 0, j = 0;
	  jclass stringClass;
	  
	  if (include_text)
	    strings_len++;
	  if (include_image)
	    strings_len++;
	  if (include_uris)
	    strings_len++;
	  
	  stringClass = (*env)->FindClass (env, "java/lang/String");
	  strings = (*env)->NewObjectArray (env, strings_len, stringClass,
					    NULL);
	  if (strings != NULL)
	    {
	      if (include_text)
		(*env)->SetObjectArrayElement (env, strings, i++,
					       stringTarget);
	      if (include_image)
		(*env)->SetObjectArrayElement (env, strings, i++,
					       imageTarget);
	      if (include_uris)
		(*env)->SetObjectArrayElement (env, strings, i++,
					       filesTarget);
	      
	      while(i < strings_len)
		{
		  if (target_strings[j] == NULL)
		    j++;
		  else
		    {
		      jstring string;
		      string = (*env)->NewStringUTF (env,
						     target_strings[j++]);
		      if (string == NULL)
			break;
		      (*env)->SetObjectArrayElement (env, strings, i++,
						     string);
		    }
		}
	    }

	  for (i = 0; i < targets_len; i++)
	    g_free (target_strings[i]);
	  g_free (target_strings);
	}
    }

  (*env)->CallVoidMethod (env, selection_obj,
			  mimeTypesAvailableID,
			  strings);
  (*env)->DeleteGlobalRef (env, selection_obj);
}

JNIEXPORT void JNICALL 
Java_gnu_java_awt_peer_gtk_GtkSelection_requestMimeTypes
(JNIEnv *env, jobject selection)
{
  jobject selection_obj;
  selection_obj = (*env)->NewGlobalRef(env, selection);
  if (selection_obj == NULL)
    return;

  if (mimeTypesAvailableID == NULL)
    {
      jclass gtk_selection_class;
      gtk_selection_class = (*env)->GetObjectClass (env, selection_obj);
      mimeTypesAvailableID = (*env)->GetMethodID (env, gtk_selection_class,
						"mimeTypesAvailable",
						"([Ljava/lang/String;)V");
      if (mimeTypesAvailableID == NULL)
	return;
    }

  /* We would have liked to call gtk_clipboard_request_targets ()
     since that is more general. But the result of that, an array of
     GdkAtoms, cannot be used with the
     gtk_selection_data_targets_include_<x> functions (despite what
     the name suggests). */
  gdk_threads_enter ();
  gtk_clipboard_request_contents (clipboard,
				  gdk_atom_intern ("TARGETS", FALSE),
				  cp_gtk_clipboard_targets_received,
				  (gpointer) selection_obj);
  gdk_threads_leave ();
}


static jmethodID textAvailableID;

static void
cp_gtk_clipboard_text_received (GtkClipboard *clipboard
				__attribute__((unused)),
				const gchar *text,
				gpointer selection)
{
  jstring string;
  jobject selection_obj = (jobject) selection;

  JNIEnv *env = cp_gtk_gdk_env ();
  if (text != NULL)
    string = (*env)->NewStringUTF (env, text);
  else
    string = NULL;

  (*env)->CallVoidMethod (env, selection_obj,
                          textAvailableID,
                          string);
  (*env)->DeleteGlobalRef (env, selection_obj);
}

JNIEXPORT void JNICALL
Java_gnu_java_awt_peer_gtk_GtkSelection_requestText
(JNIEnv *env, jobject selection)
{
  jobject selection_obj;
  selection_obj = (*env)->NewGlobalRef(env, selection);
  if (selection_obj == NULL)
    return;

  if (textAvailableID == NULL)
    {
      jclass gtk_selection_class;
      gtk_selection_class = (*env)->GetObjectClass (env, selection_obj);
      textAvailableID = (*env)->GetMethodID (env, gtk_selection_class,
					     "textAvailable",
					     "(Ljava/lang/String;)V");
      if (textAvailableID == NULL)
        return;
    }

  gdk_threads_enter ();
  gtk_clipboard_request_text (clipboard,
			      cp_gtk_clipboard_text_received,
			      (gpointer) selection_obj);
  gdk_threads_leave ();
}

static jmethodID imageAvailableID;

static void
cp_gtk_clipboard_image_received (GtkClipboard *clipboard
				 __attribute__((unused)),
				 GdkPixbuf *pixbuf,
				 gpointer selection)
{
  jobject pointer = NULL;
  jobject selection_obj = (jobject) selection;
  JNIEnv *env = cp_gtk_gdk_env ();

  if (pixbuf != NULL)
    {
      g_object_ref (pixbuf);
      pointer = JCL_NewRawDataObject (env, (void *) pixbuf);
    }

  (*env)->CallVoidMethod (env, selection_obj,
			  imageAvailableID,
                          pointer);
  (*env)->DeleteGlobalRef (env, selection_obj);
}

JNIEXPORT void JNICALL
Java_gnu_java_awt_peer_gtk_GtkSelection_requestImage (JNIEnv *env, jobject obj)
{
  jobject selection_obj;
  selection_obj = (*env)->NewGlobalRef(env, obj);
  if (selection_obj == NULL)
    return;

  if (imageAvailableID == NULL)
    {
      jclass gtk_selection_class;
      gtk_selection_class = (*env)->GetObjectClass (env, selection_obj);
      imageAvailableID = (*env)->GetMethodID (env, gtk_selection_class,
					     "imageAvailable",
					     "(Lgnu/classpath/Pointer;)V");
      if (imageAvailableID == NULL)
        return;
    }

  gdk_threads_enter ();
  gtk_clipboard_request_image (clipboard,
			       cp_gtk_clipboard_image_received,
			       (gpointer) selection_obj);
  gdk_threads_leave ();
}

static jmethodID urisAvailableID;

static void
cp_gtk_clipboard_uris_received (GtkClipboard *clipboard
				__attribute__((unused)),
				GtkSelectionData *uri_data,
				gpointer selection)
{
  gchar **uris = NULL;
  jobjectArray strings = NULL;
  jobject selection_obj = (jobject) selection;
  JNIEnv *env = cp_gtk_gdk_env ();

  if (uri_data != NULL)
    uris = gtk_selection_data_get_uris (uri_data);
  
  if (uris != NULL)
    {
      int len, i;
      gchar **count = uris;
      jclass stringClass = (*env)->FindClass (env, "java/lang/String");

      len = 0;
      while (count[len])
	len++;

      strings = (*env)->NewObjectArray (env, len, stringClass, NULL);
      if (strings != NULL)
	{
	  for (i = 0; i < len; i++)
	    {
	      jstring string = (*env)->NewStringUTF (env, uris[i]);
	      if (string == NULL)
		break;
	      (*env)->SetObjectArrayElement (env, strings, i, string);
	    }
	}
      g_strfreev (uris);
    }

  (*env)->CallVoidMethod (env, selection_obj,
                          urisAvailableID,
                          strings);
  (*env)->DeleteGlobalRef (env, selection_obj);
}

JNIEXPORT void JNICALL
Java_gnu_java_awt_peer_gtk_GtkSelection_requestURIs (JNIEnv *env, jobject obj)
{
  GdkAtom uri_atom;
  jobject selection_obj;
  selection_obj = (*env)->NewGlobalRef(env, obj);
  if (selection_obj == NULL)
    return;

  if (urisAvailableID == NULL)
    {
      jclass gtk_selection_class;
      gtk_selection_class = (*env)->GetObjectClass (env, selection_obj);
      urisAvailableID = (*env)->GetMethodID (env, gtk_selection_class,
					     "urisAvailable",
                                             "([Ljava/lang/String;)V");
      if (urisAvailableID == NULL)
        return;
    }

  /* There is no real request_uris so we have to make one ourselves. */
  gdk_threads_enter ();
  uri_atom = gdk_atom_intern ("text/uri-list", FALSE);
  gtk_clipboard_request_contents (clipboard,
				  uri_atom,
				  cp_gtk_clipboard_uris_received,
				  (gpointer) selection_obj);
  gdk_threads_leave ();
}

static jmethodID bytesAvailableID;

static void
cp_gtk_clipboard_bytes_received (GtkClipboard *clipboard
				 __attribute__((unused)),
				 GtkSelectionData *selection_data,
				 gpointer selection)
{
  jbyteArray bytes = NULL;
  jobject selection_obj = (jobject) selection;
  JNIEnv *env = cp_gtk_gdk_env ();

   if (selection_data != NULL && selection_data->length > 0)
    {
      bytes = (*env)->NewByteArray (env, selection_data->length);
      if (bytes != NULL)
	(*env)->SetByteArrayRegion(env, bytes, 0, selection_data->length,
				   (jbyte *) selection_data->data);
    }

  (*env)->CallVoidMethod (env, selection_obj,
                          bytesAvailableID,
                          bytes);
  (*env)->DeleteGlobalRef (env, selection_obj);
}

JNIEXPORT void JNICALL
Java_gnu_java_awt_peer_gtk_GtkSelection_requestBytes (JNIEnv *env,
						      jobject obj,
						      jstring target_string)
{
  int len;
  const gchar *target_text;
  GdkAtom target_atom;
  jobject selection_obj;
  selection_obj = (*env)->NewGlobalRef(env, obj);
  if (selection_obj == NULL)
    return;

  if (bytesAvailableID == NULL)
    {
      jclass gtk_selection_class;
      gtk_selection_class = (*env)->GetObjectClass (env, selection_obj);
      bytesAvailableID = (*env)->GetMethodID (env, gtk_selection_class,
					      "bytesAvailable",
					      "([B)V");
      if (bytesAvailableID == NULL)
        return;
    }

  len = (*env)->GetStringUTFLength (env, target_string);
  if (len == -1)
    return;
  target_text = (*env)->GetStringUTFChars (env, target_string, NULL);
  if (target_text == NULL)
    return;

  gdk_threads_enter ();
  target_atom = gdk_atom_intern (target_text, FALSE);
  gtk_clipboard_request_contents (clipboard,
                                  target_atom,
                                  cp_gtk_clipboard_bytes_received,
                                  (gpointer) selection_obj);
  gdk_threads_leave ();

  (*env)->ReleaseStringUTFChars (env, target_string, target_text);
}
