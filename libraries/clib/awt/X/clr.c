/**
 * clr.c - color management
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "toolkit.h"
#include <limits.h>
#include <math.h>


#define CLR_DIST 8.0

/********************************************************************************
 * auxiliary functions
 */

static inline int
xclrEquals ( XColor* pc, int r, int g, int b )
{
  return ((pc->red == r) && (pc->green == g) && (pc->blue == b));
}


/*
 * this is a rather crude color difference - a HSV difference might be
 * more ergonomical, but also much more expensive
 */
static inline double
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
static unsigned char
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


static void
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

static void
initColormap ( JNIEnv* env, Toolkit* tk, Colormap cm, Rgb2Pseudo* map )
{
  jclass   clazz;
  jfieldID fid;
  jintArray rgbRequests = 0;
  jboolean isCopy;
  jint     *jrgbs = 0;
  int      nReq = 0;
  unsigned long  pixels[MAX_REQUESTS];
  jint     req[MAX_REQUESTS];
  unsigned long  planeMasks[1];
  int      n, i, j, k, l, m, pix;
  Visual   *v = DefaultVisualOfScreen( DefaultScreenOfDisplay( tk->dsp));
  XColor   xclr;
  int      r, g, b;
  char     blackSeen = 0;
  unsigned char (*mp)[8][8][8] = alloca( 8*8*8 * sizeof( char));

  memset( *mp, 0, 8*8*8);

  /* get the java.awt.DefaultsRGB.RgbRequests field */
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
	if ( XAllocColorCells( tk->dsp, cm, False, planeMasks, 0, pixels, n) )
	  break;
  }

  xclr.red = 0; xclr.green = 0; xclr.blue = 0;
  xclr.flags = DoRed | DoGreen | DoBlue;

  /* mark all of our cells (so that we don't rely on their current values) */
  for ( i=0; i<n; i++ ){
	xclr.pixel = pixels[i];
	XStoreColor( tk->dsp, cm, &xclr);
  }

  /* check which of our rgb requests are already in the colormap */
  for ( l=0; l<v->map_entries; l++ ) {
	xclr.pixel = l;
	XQueryColor( tk->dsp, cm, &xclr);
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



static Rgb2Pseudo*
initRgb2Pseudo ( JNIEnv* env, jclass clazz UNUSED, Toolkit* tk )
{
  Colormap dcm;
  int i, j, k;
  XColor xclr;
  Rgb2Pseudo *map;

  dcm = DefaultColormapOfScreen( DefaultScreenOfDisplay( tk->dsp));
  map = (Rgb2Pseudo*) AWT_MALLOC( sizeof(Rgb2Pseudo));
  xclr.flags = DoRed | DoGreen | DoBlue;

  for ( i=0; i<8; i++ ){
	for ( j=0; j<8; j++ ) {
	  for ( k=0; k<8; k++ )
		map->pix[i][j][k] = 0;
	}
  }

  initColormap( env, tk, dcm, map);
  tk->colorMode = CM_PSEUDO_256;

  return map;
}


/********************************************************************************
 * TrueColor (RGB) visual
 */

static Rgb2True*
initRgb2True (JNIEnv* env UNUSED, jclass clazz UNUSED,  Toolkit* tk )
{
  Visual *v = DefaultVisualOfScreen( DefaultScreenOfDisplay( tk->dsp));
  unsigned int m;
  int      nRed, nGreen, nBlue;
  int      iRed, iGreen, iBlue;
  int      n;
  Rgb2True *map = 0;

  if ( (v->blue_mask == 0xff) && (v->green_mask == 0xff00) && (v->red_mask == 0xff0000) ){
	/*
	 * This is our favourite case - a direct 8-8-8 native rgb. It could be handled as
	 * a 0,0 TrueColor conversion, but (esp. for image manipulations) we can save a lot
	 * of computation by a special TrueColor mode
	 */
	DBG( AWT_CLR, printf("AWT color mode: CM_TRUE_888\n"));
	X->colorMode = CM_TRUE_888;
  }
  else {
	/*
	 * There is either a rearrangement or a non-8 bit color component involved,
	 * get start index and length of each color component. Note that the Rgb2True
	 * struct is used to compute pixelvalues from Java rgbs, i.e. the mask and shift
	 * values are relative to the Java 8-8-8 RGB.
	 */
	map = (Rgb2True*) AWT_MALLOC( sizeof( Rgb2True));

	for ( iBlue=0, m=v->blue_mask; (m & 1) == 0; iBlue++, m >>= 1);
	for ( nBlue=0; m; nBlue++, m >>= 1 );

	for ( iGreen=0, m=v->green_mask; (m & 1) == 0; iGreen++, m >>= 1);
	for ( nGreen=0; m; nGreen++, m >>= 1 );

	for ( iRed=0, m=v->red_mask; (m & 1) == 0; iRed++, m >>= 1);
	for ( nRed=0; m; nRed++, m >>= 1 );

	map->blueShift = 8 - (iBlue + nBlue);
	if ( nBlue < 8 ){  /* color reduction */
	  n = 8 - nBlue;
	  map->blueMask = (0xff >> n) << n;
	}
	else {             /* color expansion */
	  map->blueMask = 0xff;
	}

	map->greenShift = 16 - (iGreen + nGreen);
	if ( nGreen < 8 ){ /* color reduction */
	  n = 8 + (8 - nGreen);
	  map->greenMask = (0xff00 >> n) << n;
	}
	else {             /* color expansion */
	  map->greenMask = 0xff00;
	}

	map->redShift = 24 - (iRed + nRed);
	if ( nRed < 8 ){  /* color reduction */
	  n = 16 + (8 - nRed);
	  map->redMask = (0xff0000 >> n) << n;
	}
	else {            /* color expansion */
	  map->redMask = 0xff0000;
	}

	X->colorMode = CM_TRUE;

	DBG( AWT_CLR, printf("AWT color mode: CM_TRUE\n"));
	DBG( AWT_CLR, printf("    red:   %8x, %d\n", map->redMask, map->redShift));
	DBG( AWT_CLR, printf("    green: %8x, %d\n", map->greenMask, map->greenShift));
	DBG( AWT_CLR, printf("    blue:  %8x, %d\n", map->blueMask, map->blueShift));
  }

  return map;
}


/********************************************************************************
 * DirectColor (RGB) visual: each pixel value is composed of three, non-overlapping
 * colormap indices.
 * Again, our policy is to go with the default colormap, and not to swap it (so that
 * we don't disturb other desktop citizens).
 * This rather simple implementation does not allocate cells for mismatches yet
 *
 * We could save some space in case we have a DirectColor visual with a depth < 24,
 * but that would complicate pixelValue() and probably is rather unusual, anyway
 * (since DirectVisual is a expensive beast to be found on high-end HW)
 *
 * However, we do make some assumptions here which might be a bit too optimistic
 * (black being at index 0, free cells being concentracted at the end of the cmap,
 * with a 0-value)
 */

static void fillUpPartMap ( unsigned char* pix, unsigned char* val )
{
  int i, j, k, i2;

  for ( i=1, k=0; i < N_DIRECT; i++ ) {
	if ( pix[i] == 0 ) {
	  for ( j=i+1; (j < N_DIRECT) && (pix[j] == 0); j++ );
	  if ( j == N_DIRECT ) { /* last one, fill up rest */
		for ( ; i < j; i++ ) {
		  pix[i] = pix[k];
		  val[ pix[i] ] = k;
		}		
	  }
	  else {
		i2 = (i + j) / 2;
		for ( ; i < i2; i++ ){
		  pix[i] = pix[k];
		  val[ pix[i] ] = k;
		}
		for ( ; i < j; i++ ) {
		  pix[i] = pix[j];
		  val[ pix[i] ] = j;
		  k = j;
		}
	  }
	}
	else {
	  k = i;
	}
  }
}

static void setPartMapFromDMap ( Toolkit *X, Colormap dcm,
						  int component, int nIdx, int idxShift,
						  unsigned char* pix, unsigned char* val )
{
  XColor          xclr;
  int             i, idx;
  unsigned short  *v;

  if ( component == 0 )      /* red */
	v = &xclr.red;
  else if ( component == 1 ) /* green */
	v = &xclr.green;
  else                       /* blue */
	v = &xclr.blue;

  for ( i=0; i < nIdx; i++ ) {
	xclr.pixel = i << idxShift;
	XQueryColor( X->dsp, dcm, &xclr);

	if ( i && !*v )
	  break;       /* skip free cells (assuming 0: black) */

	idx = ROUND_SHORT2CHAR( *v);
	pix[idx] = i;
	val[i] = idx;
  }
}

static Rgb2Direct*
initRgb2Direct ( JNIEnv* env UNUSED, jclass clazz UNUSED, Toolkit* tk )
{
  Visual      *v = DefaultVisualOfScreen( DefaultScreenOfDisplay( tk->dsp));
  Rgb2Direct  *map = (Rgb2Direct*) AWT_MALLOC( sizeof( Rgb2Direct));
  Colormap    dcm = DefaultColormapOfScreen( DefaultScreenOfDisplay( tk->dsp));
  int         iBlue, nBlue, iGreen, nGreen, iRed, nRed;
  int         i, m;

  for ( i=0; i<N_DIRECT; i++ ) {
	map->bluePix[i] = map->greenPix[i] = map->redPix[i] = 0;
  }

  for ( iBlue=0, m=v->blue_mask; (m & 1) == 0; iBlue++, m >>= 1);
  for ( nBlue=0; m; nBlue++, m >>= 1 );

  for ( iGreen=0, m=v->green_mask; (m & 1) == 0; iGreen++, m >>= 1);
  for ( nGreen=0; m; nGreen++, m >>= 1 );

  for ( iRed=0, m=v->red_mask; (m & 1) == 0; iRed++, m >>= 1);
  for ( nRed=0; m; nRed++, m >>= 1 );

  /* we simply get the sizes of component colormaps from the visual masks */
  map->nBlue  = v->blue_mask  >> iBlue;
  map->nGreen = v->green_mask >> iGreen;
  map->nRed   = v->red_mask   >> iRed;

  /* the shifts are to convert component indices into a compound pixelvalue */
  map->blueShift  = iBlue;
  map->greenShift = iGreen;
  map->redShift   = iRed;

  /*
   * phase 1: start to populate our component maps (R/G/B -> index) from dcm
   */
  setPartMapFromDMap( tk, dcm, 0, map->nRed, map->redShift, map->redPix, map->red);
  setPartMapFromDMap( tk, dcm, 1, map->nGreen, map->greenShift, map->greenPix, map->green);
  setPartMapFromDMap( tk, dcm, 2, map->nBlue, map->blueShift, map->bluePix, map->blue);

  /*
   * phase 2: fill up missing entries
   * (based on the somewhat optimistic assumption that free cells are at the upper end)
   */
  fillUpPartMap( map->bluePix, map->blue);
  fillUpPartMap( map->greenPix, map->green);
  fillUpPartMap( map->redPix, map->red);

#ifdef NEVER
  for ( i = 0; i<N_DIRECT; i++ ) {
	printf( "%2x :   %3d,%2x  %3d,%2x  %3d,%2x\n", i,
			map->redPix[i], map->red[ map->redPix[i] ],
			map->greenPix[i], map->green[ map->greenPix[i] ],
			map->bluePix[i], map->blue[ map->bluePix[i] ]);
  }
#endif

  tk->colorMode = CM_DIRECT;

  return map;
}


/********************************************************************************
 * common funcs
 */

void
initColorMapping ( JNIEnv* env, jclass clazz,  Toolkit* tk )
{
  Visual *v = DefaultVisualOfScreen( DefaultScreenOfDisplay( tk->dsp));

  DBG( AWT_CLR, printf("X visual:\n"));
  DBG( AWT_CLR, printf("  id:          %ld\n", v->visualid));
  DBG( AWT_CLR, printf("  class:       %d\n", v->class));
  DBG( AWT_CLR, printf("  red_mask     %lx\n", v->red_mask));
  DBG( AWT_CLR, printf("  green_mask   %lx\n", v->green_mask));
  DBG( AWT_CLR, printf("  blue_mask    %lx\n", v->blue_mask));
  DBG( AWT_CLR, printf("  bits_per_rgb %x\n", v->bits_per_rgb));
  DBG( AWT_CLR, printf("  map_entries  %d\n", v->map_entries));

  /* check for directly supported color modes / visuals */
  switch ( v->class ) {
  case DirectColor:
	X->dclr = initRgb2Direct( env, clazz, tk);
	break;
  case TrueColor:
	X->tclr = initRgb2True( env, clazz, tk);
	break;
  case PseudoColor:  
	X->pclr = initRgb2Pseudo( env, clazz, tk); 
	break;
  default:
	X->colorMode = CM_GENERIC;
  }

  DBG( AWT_CLR, printf("colorMode: %d\n", tk->colorMode));
}


/********************************************************************************
 * exported functions
 */

jint
Java_java_awt_Toolkit_clrGetPixelValue ( JNIEnv* env, jclass clazz, jint rgb )
{
  jint pix;

  /*
   * We do this deferred to avoid class init problems with Defaults<->Color
   * (the notorious DefaultsRGB workaround)
   */
  if ( !X->colorMode )
	initColorMapping( env, clazz, X);

  pix = pixelValue( X, rgb);
  DBG( AWT_CLR, printf("clrGetPixelValue: %8x -> %x (%d)\n", rgb, pix, pix));

  return pix;
}

void
Java_java_awt_Toolkit_clrSetSystemColors ( JNIEnv* env UNUSED, jclass clazz UNUSED, jintArray sysClrs UNUSED )
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
 * limited PseudoColor visuals (e.g. a primitive 16 color VGA wouldn't produce any
 * usable results). Even 256 colormaps suffer from that
 */
jlong
Java_java_awt_Toolkit_clrBright ( JNIEnv* env UNUSED, jclass clazz UNUSED, jint rgb )
{
  unsigned int r, g, b;
  jint     modRgb, modPix;

  r = JRED( rgb);
  g = JGREEN( rgb);
  b = JBLUE( rgb);

  r = (r * 4) / 3;
  g = (g * 4) / 3;
  b = (b * 4) / 3;
	
  if ( r > 0xff ) r = 0xff;
  if ( g > 0xff ) g = 0xff;
  if ( b > 0xff ) b = 0xff;

  modRgb = JRGB( r, g, b);
  modPix = pixelValue( X, modRgb);

  return (((jlong)modPix << 32) | modRgb);
}

jlong
Java_java_awt_Toolkit_clrDark ( JNIEnv* env UNUSED, jclass clazz UNUSED, jint rgb )
{
  unsigned int r, g, b;
  jint     modRgb, modPix;

  r = JRED( rgb);
  g = JGREEN( rgb);
  b = JBLUE( rgb);

  r = (r * 2) / 3;
  g = (g * 2) / 3;
  b = (b * 2) / 3;
	
  modRgb = JRGB( r, g, b);
  modPix = pixelValue( X, modRgb);

  return (((jlong)modPix << 32) | modRgb);
}

jobject
Java_java_awt_Toolkit_clrGetColorModel ( JNIEnv* env, jclass clazz )
{
  jobject    cm = 0;
  jclass     cmClazz;
  jmethodID  cmCtorId;
  Visual     *v = DefaultVisualOfScreen( DefaultScreenOfDisplay( X->dsp));

  if ( !X->colorMode )
	initColorMapping( env, clazz, X);

  switch ( v->class ) {
  case DirectColor:
	break;

  case TrueColor:
	cmClazz = (*env)->FindClass( env, "java/awt/image/DirectColorModel");
	cmCtorId = (*env)->GetMethodID( env, cmClazz, "<init>", "(IIIII)V");
	cm = (*env)->NewObject( env, cmClazz, cmCtorId,
							v->bits_per_rgb, v->red_mask, v->green_mask, v->blue_mask, 0);
	break;

  case PseudoColor:  
	cmClazz = (*env)->FindClass( env, "java/awt/IndexColorModel");
	cmCtorId = (*env)->GetMethodID( env, cmClazz, "<init>", "(I[II)V");
	/* rgbs = (*env)->NewIntArray( env, 256, 0);
	 * cm = (*env)->NewObject( env, cmClazz, cmCtorId, 8, rgbs, 0);
	 */
	break;
  }

  return cm;
}
