/**
 * tlkprops.h - native layer property defines
 *
 * Here are the flags describing the properties of our native layer. Keep them
 * in sync with the Toolkit class finals!
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2002, 2003, 2004
 *	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

/*
 * 'failed' means we don't have a working native layer at all
 */
#define TLK_FAILED                 -1

/*
 * 'isBlocking' means that we suspend the dispatcher thread in
 * evtGetNextEvent() until the next Java event becomes available
 */
#define TLK_IS_BLOCKING             1

/*
 * 'isDispatchExclusive' means that we have a native window system requiring
 * us to create, event-dispatch and destroy windows exclusivly from one thread
 * (the 'dispatcher' thread). The OS/2 presentation manager is an example of such
 * a beast
 */
#define TLK_IS_DISPATCH_EXCLUSIVE   2

/*
 * 'needsFlush' means we don't execute graphics requests directly, they are
 * queued by the graphics lib in a way that requires frequent
 * queue flushing (e.g. for 'isBlocking' and Xlib). Without this, we have
 * to make sure that a blocked AWT IO doesn't defer auto-flush (e.g. by XNextEvent)
 * until doomsday. The XFLUSH macro (to be called in graphics ops) is a hook for an
 * alternative mechanism (e.g. by "elapsed time since last flush")
 */
#define TLK_NEEDS_FLUSH             4

/*
 * 'nativeDispatcherLoop' means Java doesn't run the native event aquisition loop,
 * it is inside of either the native layer or a native toolkit in use
 */
#define TLK_NATIVE_DISPATCHER_LOOP   8


/*
 * 'externalDeco' means we don't really own the space for toplevel decorations
 * (like size borders, titlebars), we just have to fake Java about it so that we can
 * simulate being in control of toplevel sizing (which we actually aren't for native
 * windowing systems like X)
 */
#define TLK_EXTERNAL_DECO            16
