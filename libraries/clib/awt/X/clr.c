/**
 * clr.c - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "toolkit.h"
#include <limits.h>
#include <math.h>


#define CLR_DIST 8.0

/********************************************************************************
 * auxiliary functions
 */

__inline__ int
xclrEquals ( XColor* pc, int r, int g, int b )
{
  return ((pc->red == r) && (pc->green == g) && (pc->blue == b));
}


/*
 * this is a rather crude color difference - a HSV difference might be
 * more ergonomical, but also much more expensive
 */
__inline__ double
colorDiff ( int r1, int g1, int b1, int r2, int g2, int b2 )
{
  int dr,dg, db;

  dr = r1 - r2;  dr *= dr;
  dg = g1 - g2;  dg *= dg;
  db = b1 - b2;  db *= db;

  return sqrt( dr + dg + db);
}


/********************************************************************************
 * 256 PseudoColor visual
 *
 * This is a real source of trouble, since we just can mitigate a serious lack
 * of resources. There are just 256 different colors available, and if this has to
 * be mapped to a equally distributed RGB model (used by Java), a 3/3/2 visual is
 * usually used, which is computed by a right shift of the Java color values (x>>5,
 * x>>6). This is wrong, twice.

 * First, it would give us just two gray values (not counting black and white),
 * because we just have 4 different blues. We therefore have to use a more fine
 * grained color cube to map Java rgbs to pixel values. A (virtual) 3/3/3 visual will
 * do the job. But this requires that we have a strategy for filling up cube cells
 * which don't have a direct system colormap equivalence (done by computing the
 * "nearest" initialized cell). Of course, this higher "color resolution" is just a
 * fake (we still have only 256 colors), and therefore we also need a mechanism to
 * ask for some "exact" color matches. If we have writable color cells, we first
 * satisfy them (if not already in the system colormap), if there still are colors
 * left, we allocate the worst RGB cube-to-colormap mismatches.

 * Second, we can't just truncate a Java RGB to get our color cube indices. This
 * might be tolerable with TrueColor visuals, but here we have just 8 distinct
 * values [0, 36.4, ..., 255], i.e. each cube point/cell represents a rather large
 * interval (36x36x36), and we have to round requests to the nearest cube cell index

 * But all this effort can easily be rendered almost useless by a bad system
 * colormap (i.e. if we don't have enough "different" colors, or if there are
 * no writable color cells at all). This is usually caused by rather careless
 * window managers, and the only way to get rid of this is to direct the window
 * manager to use one of the standard colormaps (if feasible)

 * An alternative strategy could be to defer color allocation until Color objects
 * with not-yet allocated cells are created. But since Java uses Color arithmetic
 * frequently (brighter(), darker(), HSV conversion etc.), this might lead to very
 * uneven results.
 */


/* this is a linear [0..255] / 8 rgb mapping (255/7 = 36.4) */
unsigned char rgb8 [] = { 0, 36, 73, 109, 146, 182, 219, 255 };


/*
 * Note that this implementation of nearestColor works "geometrically", i.e.
 * it just computes the best match for equal (index) distance color cells, not the
 * absolute best value (regardless of cell distance). We therefore shouldn't use
 * its result to set cells in m->pix while other empty cells are still waiting to
 * be served.
 */
unsigned char
nearestColor ( Rgb2Pseudo* m, int pi, int pj, int pk, int dist )
{
  int i, j, k, i0, i1, j0, j1, k0, k1;
  int pix, idx = -1;
  double drgb, d = 1e6;

  if ( (i0 = pi-dist) < 0 ) i0 = 0;
  if ( (i1 = pi+dist) > 7 ) i1 = 7;

  if ( (j0 = pj-dist) < 0 ) j0 = 0;
  if ( (j1 = pj+dist) > 7 ) j1 = 7;

  if ( (k0 = pk-dist) < 0 ) k0 = 0;
  if ( (k1 = pk+dist) > 7 ) k1 = 7;

  for ( i=i0; i <= i1; i++ ) {
	for ( j=j0; j <= j1; j++ ) {
	  for ( k=k0; k <= k1; k++ ) {
		if ( (pix = m->pix[i][j][k]) ) {
		  drgb = colorDiff( m->rgb[pix].r, m->rgb[pix].g, m->rgb[pix].b,
							rgb8[pi], rgb8[pj], rgb8[pk]);
		  if ( drgb < d ){
			d = drgb;
			idx = pix;
		  }
		}
	  }
	}
  }

  if ( idx >= 0 )
	return idx;
  else {
	if ( (i0 == 0) && (i1 == 7) ) /* should never happen, backup */
	  return 0;
	else
	  return nearestColor( m, pi, pj, pk, dist+1);
  }
}


typedef struct _Mismatch {
  unsigned char d;
  unsigned char i, j, k;
} Mismatch;


void
fillUpColorCube ( Rgb2Pseudo* map, Colormap cm, int nAvail, unsigned long *pixels,
				  unsigned char (*mp) [8][8][8] )
{
  int            i, j, k, pix, l;
  int            nMis = 0, maxMis = nAvail;
  int            r, g, b;
  Mismatch       *mm = alloca( maxMis* sizeof( Mismatch));
  unsigned char  d;
  XColor         xclr;

  memset( *mp, 0, 8*8*8);

  /*
   * Find the nearest values for not yet initialized cells. Note that we
   * cannot set these values directly in map->pix because it would cause
   * interference with other unset cells (the way nearestColor works), and
   * pixel values should just be computed from direct match cells
   */
  for ( i=0; i<8; i++ ){
	for ( j=0; j<8; j++ ) {
	  for ( k=0; k<8; k++ ){
		if ( (map->pix[i][j][k] == 0) && (i | j | k) ){
		  pix = nearestColor( map, i, j, k, 1);
		  (*mp)[i][j][k] = pix;

		  /*
		   * If we still have available color cells, build a sorted list of the
		   * worst mismatches (but skip dark values)
		   */
		  if ( (nAvail > 0) && (i|j|k) > 2) {
			if ( (d = (unsigned char) colorDiff( rgb8[i], rgb8[j], rgb8[k],
								map->rgb[pix].r, map->rgb[pix].g, map->rgb[pix].b)) > 50 ){
			  for ( l=0; l<nMis && mm[l].d > d ; l++ );
			  if ( l < nMis )
				memmove( mm + l+1, mm+l, (nMis - l)*sizeof( Mismatch));
			  mm[l].d = d; mm[l].i = i; mm[l].j = j; mm[l].k = k;
			  if ( nMis < maxMis )
				nMis++;
			}
		  }
		}
	  }
	}
  }

  /* if there is a mismatch list, resolve it */
  for ( l=0; l< nMis-1; l++ ) {
	r = rgb8[ mm[l].i ];
	g = rgb8[ mm[l].j ];
	b = rgb8[ mm[l].k ];

	xclr.pixel = pixels[l];
	xclr.flags = DoRed | DoGreen | DoBlue;
	xclr.red   = r << 8;
	xclr.green = g << 8;
	xclr.blue  = b << 8;
    XStoreColor( X->dsp, cm, &xclr);

	map->pix [mm[l].i] [mm[l].j] [mm[l].k] = xclr.pixel;
	map->rgb[xclr.pixel].r = r;
	map->rgb[xclr.pixel].g = g;
	map->rgb[xclr.pixel].b = b;

	/* mark this cell as satisifed */
	(*mp) [mm[l].i] [mm[l].j] [mm[l].k] = 0;
  }

  /* store all still uninitialized cube cells from our temp cube of nearest values */
  for ( i=0; i<8; i++ ){
	for ( j=0; j<8; j++ ) {
	  for ( k=0; k<8; k++ ){
		if ( (pix = (*mp) [i][j][k]) )
		  map->pix[i][j][k] = pix;
	  }
	}
  }
}


#define MAX_REQUESTS 16

void
initColormap ( JNIEnv* env, Toolkit* X, Colormap cm, Rgb2Pseudo* map )
{
  jclass   clazz;
  jfieldID fid;
  jarray   rgbRequests = 0;
  jboolean isCopy;
  jint     *jrgbs = 0;
  int      nReq = 0;
  unsigned long  pixels[MAX_REQUESTS];
  jint     req[MAX_REQUESTS];
  unsigned long  planeMasks[1];
  int      n, i, j, k, l, m, pix;
  Visual   *v = DefaultVisualOfScreen( DefaultScreenOfDisplay( X->dsp));
  XColor   xclr;
  int      r, g, b;
  char     blackSeen = 0;
  unsigned char (*mp)[8][8][8] = alloca( 8*8*8 * sizeof( char));

  memset( *mp, 0, 8*8*8);

  /* get the java.awt.Defaults.RgbRequests field */
  if ( (clazz = (*env)->FindClass( env, "java/awt/Defaults")) ){
	if ( (fid = (*env)->GetStaticFieldID( env, clazz, "RgbRequests", "[I")) ){
	  if ( (rgbRequests = (*env)->GetStaticObjectField( env, clazz, fid)) ){
		jrgbs = (*env)->GetIntArrayElements( env, rgbRequests, &isCopy);
		nReq = (*env)->GetArrayLength( env, rgbRequests);

		if ( nReq > MAX_REQUESTS )
		  nReq = MAX_REQUESTS;

		memcpy( req, jrgbs, nReq * sizeof( jint));
		(*env)->ReleaseIntArrayElements( env, rgbRequests, jrgbs, JNI_ABORT);
	  }
	}
  }
  
  /*
   * Determine how many RW cells there are available. Don't try to grab
   * too many cells, since this might disturb other apps and could end up
   * in even worse results
   */
  for ( n= 10; n; n-- ) {
	if ( XAllocColorCells( X->dsp, cm, False, planeMasks, 0, pixels, n) )
	  break;
  }

  xclr.red = 0; xclr.green = 0; xclr.blue = 0;
  xclr.flags = DoRed | DoGreen | DoBlue;

  /* mark all of our cells (so that we don't rely on their current values) */
  for ( i=0; i<n; i++ ){
	xclr.pixel = pixels[i];
	XStoreColor( X->dsp, cm, &xclr);
  }

  /* check which of our rgb requests are already in the colormap */
  for ( l=0; l<v->map_entries; l++ ) {
	xclr.pixel = l;
	XQueryColor( X->dsp, cm, &xclr);
	r = xclr.red >> 8;
	g = xclr.green >> 8;
	b = xclr.blue >> 8;

	i = JI8(r);
	j = JI8(g);
	k = JI8(b);

	if ( r | g | b ) {
	  for ( m=0; m<nReq; m++ ) {
		if ( req[m] &&
			 colorDiff( JRED(req[m]), JGREEN(req[m]), JBLUE(req[m]), r, g, b) < CLR_DIST ) {
           req[m] = 0;                 /* mark color request as satisfied */
		   (*mp)[i][j][k] = 1;         /* mark cube cell (i,j,k) as requested */
		   break;
		}
	  }
	}

	/*
	 * we start to populate the color cube as we go (XQueryColor might be expensive),
	 * but we don't overwrite an already set cell with a worse match (if we don't
	 * have a standard system colormap, there is a good chance that several
	 * colormap cells will map to the same indices of our 3/3/3 color cube). We also
	 * shouldn't overwrite requested colors with better cube-cell matches (hence the
	 * 'mp' check)
	 */
	if ( !(i|j|k) && blackSeen++ ) /* don't overwrite real black with avail cell */
	  continue;
	if ( ((*mp)[i][j][k] < 2) && (!(pix = map->pix [i][j][k]) || 
						  (colorDiff( r, g, b, rgb8[i], rgb8[j], rgb8[k]) <
						   colorDiff( map->rgb[pix].r, map->rgb[pix].g, map->rgb[pix].b,
									  rgb8[i], rgb8[j], rgb8[k]))) ) {
	  if ( (*mp)[i][j][k] ) /* prevent cube cell from being overwritten, again */
		(*mp)[i][j][k] ++;

	  map->pix [i][j][k] = l;
	  map->rgb[l].r = r;
	  map->rgb[l].g = g;
	  map->rgb[l].b = b;
	}
  }

  /* set cells of not-yet satisfied rgb requests */
  for ( i=0, j=0; (i<nReq) && (i<n); i++ ) {
	if ( req[i] ){
	  r = JRED( req[i]);
	  g = JGREEN( req[i]);
	  b = JBLUE( req[i]);

	  xclr.pixel = pixels[j++];
	  xclr.red   = r << 8;
	  xclr.green = g << 8;
	  xclr.blue  = b << 8;
	  XStoreColor( X->dsp, cm, &xclr);

	  map->pix [JI8(r)] [JI8(g)] [JI8(b)] = xclr.pixel;
	  map->rgb[xclr.pixel].r = r;
	  map->rgb[xclr.pixel].g = g;
	  map->rgb[xclr.pixel].b = b;
	}
  }

  /*
   * initalize rest of cube cells by computing their nearest rgb value, optionally
   * allocating new cells for the worst mismatches (if there still are colors avail)
   */
  fillUpColorCube( map, cm, (n - j), pixels+j, mp );
}



Rgb2Pseudo*
initRgb2Pseudo ( JNIEnv* env, Toolkit* X )
{
  Colormap dcm;
  int i, j, k;
  XColor xclr;
  Rgb2Pseudo *map;

  dcm = DefaultColormapOfScreen( DefaultScreenOfDisplay( X->dsp));
  map = (Rgb2Pseudo*) malloc( sizeof(Rgb2Pseudo));
  xclr.flags = DoRed | DoGreen | DoBlue;

  for ( i=0; i<8; i++ ){
	for ( j=0; j<8; j++ ) {
	  for ( k=0; k<8; k++ )
		map->pix[i][j][k] = 0;
	}
  }

  initColormap( env, X, dcm, map);

  return map;
}


/********************************************************************************
 * DirectColor (RGB) visual
 */

Rgb2True*
initRgb2True ( Toolkit* X )
{
  Visual *v = DefaultVisualOfScreen( DefaultScreenOfDisplay( X->dsp));
  unsigned int n, m;
  int      nRed, nGreen, nBlue;

  Rgb2True *map = (Rgb2True*) malloc( sizeof( Rgb2True));

  for ( nBlue=0, m=v->blue_mask; m; nBlue++, m >>= 1 );
  for ( nGreen=0, m=v->green_mask >> nBlue; m; nGreen++, m >>= 1 );
  for ( nRed=0, m=v->red_mask >> (nBlue+nGreen); m; nRed++, m >>= 1 );

  n = 8 - nBlue;
  map->blueMask = (0xff >> n) << n;
  map->blueShift = n;

  n = 16 - nGreen;
  map->greenMask = (0xff00 >> n) << n;
  map->greenShift = 16 - (nBlue + nGreen);

  n = 24 - nRed;
  map->redMask = (0xff0000 >> n) << n;
  map->redShift = 24 - (nBlue + nGreen + nRed);

  return map;
}


/********************************************************************************
 * common funcs
 */

void
initColorMapping ( JNIEnv* env, Toolkit* X )
{
  Visual *v = DefaultVisualOfScreen( DefaultScreenOfDisplay( X->dsp));
  
  X->visualClass = v->class;

#ifdef DEBUG
  fprintf( stderr, "visual:\n");
  fprintf( stderr, "  id:          %d\n", (int)v->visualid);
  fprintf( stderr, "  class:       %d\n", v->class);
  fprintf( stderr, "  red_mask     %x\n", (int)v->red_mask);
  fprintf( stderr, "  green_mask   %x\n", (int)v->green_mask);
  fprintf( stderr, "  blue_mask    %x\n", (int)v->blue_mask);
  fprintf( stderr, "  bits_per_rgb %x\n", v->bits_per_rgb);
  fprintf( stderr, "  map_entries  %d\n", v->map_entries);
#endif

  switch ( v->class ) {
  case DirectColor:
  case TrueColor:	 X->tclr = initRgb2True( X); break;
  case PseudoColor:  X->pclr = initRgb2Pseudo( env, X); break;
  }
}


/********************************************************************************
 * exported functions
 */

jint
Java_java_awt_Toolkit_clrGetPixelValue ( JNIEnv* env, jclass clazz, jint rgb )
{
  return pixelValue( X, rgb);
}

void
Java_java_awt_Toolkit_clrSetSystemColors ( JNIEnv* env, jclass clazz, jarray sysClrs )
{
#ifdef NEVER /* maybe this could be initialized via X resources */
  jboolean isCopy;
  jint *rgbs = (*env)->GetIntArrayElements( env, sysClrs, &isCopy);
  int  len = (*env)->GetArrayLength( env, sysClrs);

  rgbs[ 0] = 0;  /* desktop */
  rgbs[ 1] = 0;  /* active_caption */
  rgbs[ 2] = 0;  /* active_caption_text */
  rgbs[ 3] = 0;  /* active_caption_border */
  rgbs[ 4] = 0;  /* inactive_caption */
  rgbs[ 5] = 0;  /* inactive_caption_text */
  rgbs[ 6] = 0;  /* inactive_caption_border */
  rgbs[ 7] = 0;  /* window */
  rgbs[ 8] = 0;  /* window_border */
  rgbs[ 9] = 0;  /* window_text */
  rgbs[10] = 0;  /* menu */
  rgbs[11] = 0;  /* menu_text */
  rgbs[12] = 0;  /* text */
  rgbs[13] = 0;  /* text_text */
  rgbs[14] = 0;  /* text_highlight */
  rgbs[15] = 0;  /* text_highlight_text */
  rgbs[16] = 0;  /* text_inactive_text */
  rgbs[17] = 0;  /* control */
  rgbs[18] = 0;  /* control_text */
  rgbs[19] = 0;  /* control_highlight */
  rgbs[20] = 0;  /* control_lt_highlight */
  rgbs[21] = 0;  /* control_shadow */
  rgbs[22] = 0;  /* control_dk_shadow */
  rgbs[23] = 0;  /* scrollbar */
  rgbs[24] = 0;  /* info */
  rgbs[25] = 0;  /* info_text */

  (*env)->ReleaseIntArrayElements( env, sysClrs, rgbs, 0);
#endif
}

/*
 * we need to implement brighter / darker in the native layer because the usual
 * arithmetic Rgb brightening/darkening isn't really useful if it comes to
 * PseudoColor visuals (e.g. a primitive 16 color VGA wouldn't produce any
 * usable results). Even 256 colormaps suffer from 
 */
jlong
Java_java_awt_Toolkit_clrBright ( JNIEnv* env, jclass clazz, jint rgb )
{
  unsigned r, g, b;
  jint     modRgb, modPix;

  r = JRED( rgb);
  g = JGREEN( rgb);
  b = JBLUE( rgb);

  switch ( X->visualClass ) {
  case TrueColor:
  case DirectColor:
  case PseudoColor:
	r = (r * 4) / 3;
	g = (g * 4) / 3;
	b = (b * 4) / 3;
	
	if ( r > 0xff ) r = 0xff;
	if ( g > 0xff ) g = 0xff;
	if ( b > 0xff ) b = 0xff;

	modRgb = JRGB( r, g, b);
	modPix = pixelValue( X, modRgb);

	break;

  default:
	modRgb = 0xffffffff;
	modPix = WhitePixel( X->dsp, DefaultScreen( X->dsp));
  }

  return (((jlong)modPix << 32) | modRgb);
}

jlong
Java_java_awt_Toolkit_clrDark ( JNIEnv* env, jclass clazz, jint rgb )
{
  unsigned r, g, b;
  jint     modRgb, modPix;

  r = JRED( rgb);
  g = JGREEN( rgb);
  b = JBLUE( rgb);

  switch ( X->visualClass ) {
  case TrueColor:
  case DirectColor:
  case PseudoColor:
	r = (r * 2) / 3;
	g = (g * 2) / 3;
	b = (b * 2) / 3;
	
	modRgb = JRGB( r, g, b);
	modPix = pixelValue( X, modRgb);

	break;

  default:
	modRgb = 0xff000000;
	modPix = BlackPixel( X->dsp, DefaultScreen( X->dsp));
  }

  return (((jlong)modPix << 32) | modRgb);
}
