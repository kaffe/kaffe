/**
 * nano-X AWT backend for Kaffe.
 *
 * Copyright (c) 2001
 *	Exor International Inc. All rights reserved.
 *
 * Copyright (c) 2001
 *	Sinby Corporatin, All rights reserved.
 *
 * Copyright (c) 2005
 *	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-setjmp.h"

#include "toolkit.h"

#include "jni.h"

jobject
Java_java_awt_Toolkit_graInitGraphics(
		JNIEnv *env, jclass k, jobject _jgraphics, 
		jobject _jtarget, jint targetType, 
		jint off_x, jint off_y, jint clip_x, jint clip_y, 
		jint clip_xs, jint clip_ys, jobject _jfont, 
		jint fg, jint bg, jboolean blank) 
{
	GR_ID target; // window or image 
	TLKGraphics *graphicsP;

//printf("init %d %d %d %d %d %d\n", off_x, off_y, clip_x, clip_y, clip_xs, clip_ys);
	
	if ( _jfont == NULL) {
		SignalError("java.lang.NullPointerException", "no object");
		return NULL;
	}
	graphicsP = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( _jtarget ) {
		switch ( targetType ) {
		case TLK_TARGET_TYPE_WINDOW:
			target = (GR_ID)_jtarget; 
			break;
		case TLK_TARGET_TYPE_IMAGE:
			target = (GR_ID)_jtarget; 
			break;
		case TLK_TARGET_TYPE_GRAPHICS:
		{
			TLKGraphics *indgP;
			indgP = tlk_get_graphics((TLK_GRAPHICS_ID)_jtarget);
			if ( indgP == NULL ) {
				SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
				return NULL;
			}
			target = indgP->target;
			break;
		}
		default: 
			SignalError("java.lang.InternalError", "illegal target type");
			return NULL;
		}
	} else if ( graphicsP ) {
		target = graphicsP->target;
	} else {
		SignalError("java.lang.InternalError", "no target and graphics");
		return NULL;
	}

	if ( graphicsP == NULL ) {
		graphicsP = (TLKGraphics*) TLK_MALLOC( sizeof(TLKGraphics));
		graphicsP->gc = GrNewGC();
		graphicsP->region = 0;
		graphicsP->target = (GR_ID)target;
		graphicsP->targetType = targetType;
		graphicsP->gid = (TLK_GRAPHICS_ID)graphicsP;
		graphicsP->magicNo = TLK_GRAPHICS_MAGIC_NO;
	}

	GrSetGCFont(graphicsP->gc,(GR_FONT_ID)_jfont);

	graphicsP->off_x = off_x;
	graphicsP->off_y = off_y;

	GrSetGCForeground(graphicsP->gc,fg);
	GrSetGCBackground(graphicsP->gc,bg);
	graphicsP->fg_color = fg;
	graphicsP->bg_color = bg;
	
	if ( blank ) {
		GrClearWindow(graphicsP->gc, 0);
	}

	return graphicsP;
}

void
Java_java_awt_Toolkit_graFreeGraphics ( JNIEnv* env, jclass clazz, jobject _jgraphics)
{
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
	}

	if ( gp->region ) {
		GrDestroyRegion(gp->region);
	}
	GrDestroyGC(gp->gc);
	TLK_FREE(gp);
}

void
Java_java_awt_Toolkit_graSetClip( JNIEnv* env, jclass clazz, jobject _jgraphics, jint x, jint y, jint width, jint height )
{
	TLKGraphics *gp;
	GR_RECT		clip_rect;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}

	
	if (( width == 0 ) || ( height == 0 )) {
		GrSetGCRegion(gp->gc, 0);
		if ( gp->region != NULL ) {
			GrDestroyRegion(gp->region);
			gp->region = NULL;
		}
	} else if ( gp->region == NULL ) {
		clip_rect.x = y;
		clip_rect.y = x;
		clip_rect.width = width;
		clip_rect.height = height;
		GrUnionRectWithRegion(gp->region, &clip_rect);
		GrSetGCRegion(gp->gc, gp->region);
	}
}

void
Java_java_awt_Toolkit_graSetVisible( JNIEnv* env, jclass clazz, jobject _jgraphics, jboolean isVisible )
{
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
	}

	if ( gp->targetType == TLK_TARGET_TYPE_WINDOW ) {
		if ( isVisible ) {
			GrMapWindow(gp->target);
			GrFlush();
		} else {
			GrUnmapWindow(gp->target);
		}
	} else {
		SignalError("java.lang.InternalError", "I guess it's a illegal oparation, maybe");
	}
}

void
Java_java_awt_Toolkit_graClearRect( JNIEnv* env, jclass clazz, jobject _jgraphics, jint x, jint y, jint width, jint height )
{
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
	}

	GrSetGCForeground(gp->gc, gp->bg_color);
	GrFillRect(gp->target, gp->gc, gp->off_x + x , gp->off_y + y, width, height);
	GrSetGCForeground(gp->gc, gp->fg_color);

	GrFlush();
}

jlong Java_java_awt_Toolkit_clrBright( JNIEnv* envP, jclass clazz, jint rgb );
jlong Java_java_awt_Toolkit_clrDark( JNIEnv* env, jclass clazz, jint rgb );

static inline void
Draw3DRect(TLKGraphics *gp, jint x, jint y, jint xs, jint ys, jboolean raised, jint java_rgb)
{
	jint x0, y0, x1, y1;
	GR_COLOR dark, bright;
	
	dark   = (int) (Java_java_awt_Toolkit_clrDark( NULL, NULL, java_rgb) >> 32);
	bright = (int) (Java_java_awt_Toolkit_clrBright( NULL, NULL, java_rgb) >> 32);
	/* dark, bright are pixel value */

	x0 = x + gp->off_x; 
	y0 = y + gp->off_y;
	x1 = x0 + xs;
	y1 = y0 + ys;

	GrSetGCForeground(gp->gc, (raised ? bright : dark));
	GrLine(gp->target, gp->gc, x0, y0, x1-1, y0);
	GrLine(gp->target, gp->gc, x0, y0, x0, y1-1);

	GrSetGCForeground(gp->gc, (raised ? dark : bright));
	GrLine(gp->target, gp->gc, x0+1, y1, x1, y1);
	GrLine(gp->target, gp->gc, x1, y0, x1, y1);

	GrSetGCForeground(gp->gc, gp->fg_color);

	GrFlush();
}

void
Java_java_awt_Toolkit_graDraw3DRect(JNIEnv *env, jclass clazz, jobject _jgraphics, jint x, jint y, jint xs, jint ys, jboolean raised, jint java_rgb) 
{
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
	}

	Draw3DRect(gp, x, y, xs, ys, raised, java_rgb);
}

void
Java_java_awt_Toolkit_graFill3DRect ( JNIEnv* env, jclass clazz, jobject _jgraphics, jint x, jint y, jint xs, jint ys, jboolean raised, jint java_rgb )
{
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
	}
	GrFillRect(gp->target, gp->gc, gp->off_x + x + 1, gp->off_y + y + 1, xs - 2, ys - 2);
	Draw3DRect(gp, x, y, xs-1, ys-1, raised, java_rgb);
}

void
Java_java_awt_Toolkit_graDrawRect(JNIEnv* env, jclass clazz, jobject _jgraphics , jint x, jint y, jint xs, jint ys )
{
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}

	if ( (xs >= 0) && (ys >= 0) ) {
		GrRect(gp->target, gp->gc, gp->off_x + x, gp->off_y + y, xs, ys);
	}
	GrFlush();
}

void
Java_java_awt_Toolkit_graFillRect( JNIEnv* env, jclass clazz, jobject _jgraphics , jint x, jint y, jint xs, jint ys )
{
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}

	if ( (xs >= 0) && (ys >= 0) ) {
		if ( xs == 1 ) {
			GrLine(gp->target, gp->gc, gp->off_x + x, gp->off_y + y, 
				gp->off_x + x, gp->off_y + y + ys - 1);
		} else if ( ys == 1 ){
			GrLine(gp->target, gp->gc, gp->off_x + x, gp->off_y + y, 
				gp->off_x + x + xs - 1, gp->off_y + y);
		} else {
			GrFillRect(gp->target, gp->gc, gp->off_x + x, gp->off_y + y, xs, ys);
		}
	}
	GrFlush();
}

void 
Java_java_awt_Toolkit_graDrawBytes(JNIEnv *envP, jclass k, jobject _jgraphics, jbyteArray _jbytes, jint off, jint len, jint x, jint y) 
{
	TLKGraphics *gp;
	int bytes_len;
	jbyte *jb_top,*jb;
	jboolean isCopy;
	
	if ( _jbytes == NULL ) {
		SignalError("java.lang.NullPointerException", "byte array is null");
		return;
	}
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}

	bytes_len = (*envP)->GetArrayLength(envP, _jbytes);
	jb_top = (*envP)->GetByteArrayElements(envP, _jbytes, &isCopy);
	if ( jb_top == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return;
	}
	jb = jb_top + off;
	if (off + len > bytes_len) {
		len = bytes_len - off;
	}

	GrText(gp->target, gp->gc, gp->off_x + x, gp->off_y + y, jb, len, GR_TFASCII);

	(*envP)->ReleaseByteArrayElements( envP, _jbytes, jb_top, JNI_ABORT);
}

void 
Java_java_awt_Toolkit_graDrawChars(JNIEnv *envP, jclass k, jobject _jgraphics, jcharArray _jchars, jint off, jint len, jint x, jint y) 
{
	TLKGraphics *gp;
	int chars_len;
	jchar *jc_top,*jc;
	jboolean isCopy;
	
	if ( _jchars == NULL ) {
		SignalError("java.lang.NullPointerException", "char array is null");
		return;
	}
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}

	chars_len = (*envP)->GetArrayLength(envP, _jchars);
	jc_top = (*envP)->GetCharArrayElements(envP, _jchars, &isCopy);
	if ( jc_top == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return;
	}
	jc = jc_top + off;
	if (off + len > chars_len) {
		len = chars_len - off;
	}

	GrText(gp->target, gp->gc, gp->off_x + x, gp->off_y + y, jc, len, GR_TFUC16);

	(*envP)->ReleaseCharArrayElements( envP, _jchars, jc_top, JNI_ABORT);
}

void 
Java_java_awt_Toolkit_graDrawString(JNIEnv *envP, jclass k, jobject _jgraphics, jstring _jstr, jint x, jint y) 
{
	TLKGraphics *gp;
	jboolean isCopy;
	jint chars_len;
	const jchar *jc;
	
	if ( _jstr == NULL ) {
		SignalError("java.lang.NullPointerException", "char array is null");
		return;
	}
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}

	chars_len = (*envP)->GetStringLength(envP, _jstr);
	jc = (*envP)->GetStringChars(envP, _jstr, &isCopy);
	if ( jc == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return;
	}

	GrText(gp->target, gp->gc, gp->off_x + x, gp->off_y + y, (char *)jc, chars_len, GR_TFUC16);

	(*envP)->ReleaseStringChars(envP, _jstr, jc);
}

void
Java_java_awt_Toolkit_graCopyArea(JNIEnv* envP, jclass clazz, jobject _jgraphics, jint sx, jint sy, jint width, jint height, jint xDelta, jint yDelta )
{
	TLKGraphics *gp;
	
	if ((width < 0) || (height < 0)) {
		return;
	}
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
	}

	sx += gp->off_x; 
	sy += gp->off_y;

	GrCopyArea(gp->target, gp->gc, sx + xDelta, sx + yDelta, width, height, gp->gc, sx, sy, GR_MODE_SET);
	GrFlush();
}

void
Java_java_awt_Toolkit_graDrawLine( JNIEnv* env, jclass clazz, jobject _jgraphics, jint x1, jint y1, jint x2, jint y2 )
{
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
	}

	if ( (x1==x2) && (y1==y2) ) { 
		GrPoint(gp->target, gp->gc, gp->off_x + x1 , gp->off_y + y1);
	} else {
		GrLine(gp->target, gp->gc, gp->off_x + x1, gp->off_y + y1, gp->off_x + x2, gp->off_y + y2);
	}
	GrFlush();
}

void
Java_java_awt_Toolkit_graDrawArc( JNIEnv* env, jclass clazz, jobject _jgraphics, jint x, jint y, jint width, jint height, jint startAngle, jint arcAngle )
{
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}

	GrArcAngle(gp->target, gp->gc, gp->off_x + x, gp->off_y + y, width, height, startAngle<<6, arcAngle<<6, GR_ARC);
	GrFlush();
}

void
Java_java_awt_Toolkit_graFillArc( JNIEnv* env, jclass clazz, jobject _jgraphics, jint x, jint y, jint width, jint height, jint startAngle, jint arcAngle )
{
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}

	GrArcAngle(gp->target, gp->gc, gp->off_x + x, gp->off_y + y, width, height, startAngle<<6, arcAngle<<6, GR_PIE);
	GrFlush();
}

void
Java_java_awt_Toolkit_graDrawOval( JNIEnv* env, jclass clazz, jobject _jgraphics, jint x, jint y, jint width, jint height )
{
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}

	GrArcAngle(gp->target, gp->gc, gp->off_x + x, gp->off_y + y, width, height, 0, 360 << 6, GR_ARC);
	GrFlush();
}

void
Java_java_awt_Toolkit_graFillOval( JNIEnv* env, jclass clazz, jobject _jgraphics, jint x, jint y, jint width, jint height )
{
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}

	GrArcAngle(gp->target, gp->gc, gp->off_x + x, gp->off_y + y, width, height, 0, 360 << 6, GR_PIE);
	GrFlush();
}

// NOTE: it's a funny interface, because I have no idea.
static inline GR_POINT *
jint2gr_points(int off_x, int off_y, jint *xa, jint *ya, int *np, jboolean needConnect)
{
	int alloc_n, i;
	GR_POINT *xyp;
	
	if ( needConnect ) {
		alloc_n = (*np)+1;
	} else {
		alloc_n = (*np);
	}
	xyp = TLK_CALLOC(alloc_n, sizeof(GR_POINT));
	if ( xyp == NULL ) {
		return NULL;
	}
	for( i = 0 ; i < *np ; i++ ) {
		xyp[i].x = xa[i];
		xyp[i].y = ya[i];
	}

	if ( needConnect ) {
		xyp[*np].x = xa[0];
		xyp[*np].y = ya[0];
		(*np)++;
	}

	return xyp;
}

void
Java_java_awt_Toolkit_graDrawPolygon(JNIEnv* envP, jclass clazz, jobject _jgraphics, jintArray xPoints, jintArray yPoints, jint nPoints )
{
	TLKGraphics *gp;
	jint *xa,*ya;
	int n;
	jboolean isCopy, needConnect;
	GR_POINT *xyp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}

	if ( !xPoints || !yPoints ) {
		SignalError("java.lang.NullPointerException", "no object");
		return;
	}
	if ((*envP)->GetArrayLength(envP, xPoints) < nPoints ) {
		SignalError("ArrayIndexOutOfBoundsException", "x array error");
		return;
	}
	if ((*envP)->GetArrayLength(envP, yPoints) < nPoints ) {
		SignalError("ArrayIndexOutOfBoundsException", "y array error");
		return;
	}

	xa = (*envP)->GetIntArrayElements(envP, xPoints, &isCopy);
	if ( xa == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return;
	}
	ya = (*envP)->GetIntArrayElements(envP, yPoints, &isCopy);
	if ( ya == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return;
	}

	if ((xa[nPoints-1] == xa[0]) && (ya[nPoints-1] == ya[0])) {
		needConnect = JNI_FALSE;
	} else {
		needConnect = JNI_TRUE;
	}
	n = nPoints;
	xyp = jint2gr_points(gp->off_x, gp->off_y, xa, ya, &n, needConnect);
	if ( xyp == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return;
	}

	GrPoly(gp->target, gp->gc, n, xyp);
	GrFlush();
	TLK_FREE(xyp);
	(*envP)->ReleaseIntArrayElements(envP, xPoints, xa, JNI_ABORT);
	(*envP)->ReleaseIntArrayElements(envP, yPoints, ya, JNI_ABORT);
}

void
Java_java_awt_Toolkit_graDrawPolyline(JNIEnv* envP, jclass clazz, jobject _jgraphics, jintArray xPoints, jintArray yPoints, jint nPoints )
{
	TLKGraphics *gp;
	jint *xa,*ya;
	jboolean isCopy;
	GR_POINT *xyp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}

	if ( !xPoints || !yPoints ) {
		SignalError("java.lang.NullPointerException", "no object");
		return;
	}
	if ((*envP)->GetArrayLength(envP, xPoints) < nPoints ) {
		SignalError("ArrayIndexOutOfBoundsException", "x array error");
		return;
	}
	if ((*envP)->GetArrayLength(envP, yPoints) < nPoints ) {
		SignalError("ArrayIndexOutOfBoundsException", "y array error");
		return;
	}

	xa = (*envP)->GetIntArrayElements(envP, xPoints, &isCopy);
	if ( xa == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return;
	}
	ya = (*envP)->GetIntArrayElements(envP, yPoints, &isCopy);
	if ( ya == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return;
	}

	xyp = jint2gr_points(gp->off_x, gp->off_y, xa, ya, &nPoints, JNI_FALSE);
	if ( xyp == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return;
	}

	GrPoly(gp->target, gp->gc, nPoints, xyp);
	GrFlush();
	TLK_FREE(xyp);
	(*envP)->ReleaseIntArrayElements(envP, xPoints, xa, JNI_ABORT);
	(*envP)->ReleaseIntArrayElements(envP, yPoints, ya, JNI_ABORT);
}

void
Java_java_awt_Toolkit_graFillPolyline(JNIEnv* envP, jclass clazz, jobject _jgraphics, jintArray xPoints, jintArray yPoints, jint nPoints )
{
	TLKGraphics *gp;
	jint *xa,*ya;
	jboolean isCopy;
	GR_POINT *xyp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}

	if ( !xPoints || !yPoints ) {
		SignalError("java.lang.NullPointerException", "no object");
		return;
	}
	if ((*envP)->GetArrayLength(envP, xPoints) < nPoints ) {
		SignalError("ArrayIndexOutOfBoundsException", "x array error");
		return;
	}
	if ((*envP)->GetArrayLength(envP, yPoints) < nPoints ) {
		SignalError("ArrayIndexOutOfBoundsException", "y array error");
		return;
	}

	xa = (*envP)->GetIntArrayElements(envP, xPoints, &isCopy);
	if ( xa == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return;
	}
	ya = (*envP)->GetIntArrayElements(envP, yPoints, &isCopy);
	if ( ya == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return;
	}

	xyp = jint2gr_points(gp->off_x, gp->off_y, xa, ya, &nPoints, JNI_FALSE);
	if ( xyp == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return;
	}

	GrFillPoly(gp->target, gp->gc, nPoints, xyp);
	GrFlush();
	TLK_FREE(xyp);
	(*envP)->ReleaseIntArrayElements(envP, xPoints, xa, JNI_ABORT);
	(*envP)->ReleaseIntArrayElements(envP, yPoints, ya, JNI_ABORT);
}

void
Java_java_awt_Toolkit_graDrawRoundRect(JNIEnv* env, jclass clazz, jobject _jgraphics, jint x, jint y, jint width, jint height, jint wArc, jint hArc )
{
	TLKGraphics *gp;
	int x1, x2, y1, y2, a, b;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
	}

	x += gp->off_x;  
	y += gp->off_y;
	a = wArc / 2;
	b = hArc / 2;

	x1 = x + a;   
	x2 = x + width - a;  
	y2 = y + height;
	GrLine(gp->target, gp->gc, x1, y, x2, y);
	GrLine(gp->target, gp->gc, x1, y2, x2, y2);

	y1 = y + b;   
	y2 = y + height - b; 
	x2 = x + width;
	GrLine(gp->target, gp->gc, x, y1, x, y2);
	GrLine(gp->target, gp->gc, x2, y1, x2, y2);

	GrArcAngle(gp->target, gp->gc, x, y, wArc, hArc, 90 * 64, 90 * 64, GR_ARC);

	x2 = x + width - wArc;
	GrArcAngle(gp->target, gp->gc, x2, y, wArc, hArc, 0, 90 * 64, GR_ARC);

	y2 = y + height - hArc;
	GrArcAngle(gp->target, gp->gc, x2, y2, wArc, hArc, 0, -90 * 64, GR_ARC);

	GrArcAngle(gp->target, gp->gc,     x, y2, wArc, hArc, 180 * 64, -90 * 64, GR_ARC);

	GrFlush();
}

void
Java_java_awt_Toolkit_graFillRoundRect(JNIEnv* env, jclass clazz, jobject _jgraphics, jint x, jint y, jint width, jint height, jint wArc, jint hArc )
{
	TLKGraphics *gp;
	int x1, x2, y1, y2, a, b;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
	}

	x += gp->off_x;  
	y += gp->off_y;
	a = wArc / 2;
	b = hArc / 2;

	y1 = y + b;  
	y2 = y + height - b;
	if ( y2 > y1 ) {
		GrFillRect(gp->target, gp->gc, x, y1, width, y2-y1);
	}
  
	x1 = x + a;
	x2 = x + width - a;
	if ( x2 > x1 ) {
		GrFillRect(gp->target, gp->gc, x1, y, (x2-x1), b);
		GrFillRect(gp->target, gp->gc, x1, y2, (x2-x1), b);
	}

	GrArcAngle(gp->target, gp->gc, x, y, wArc, hArc, 90*64, 90*64, GR_PIE);

	x2 = x + width - wArc;
	GrArcAngle(gp->target, gp->gc, x2, y, wArc, hArc, 0, 90*64, GR_PIE);

	y2 = y + height - hArc;
	GrArcAngle(gp->target, gp->gc, x2, y2, wArc, hArc, 0, -90*64, GR_PIE);

	GrArcAngle(gp->target, gp->gc, x, y2, wArc, hArc, 180*64, 90*64, GR_PIE);

	GrFlush();
}

void
Java_java_awt_Toolkit_graAddClip ( JNIEnv* env, jclass clazz, jobject _jgraphics, jint xClip, jint yClip, jint wClip, jint hClip )
{
	/* not implemented yet */
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
	}
}

void
Java_java_awt_Toolkit_graSetPaintMode ( JNIEnv* env, jclass clazz, jobject _jgraphics)
{
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
	}

	gp->flags &= ~TLK_GRAPHICS_FLAG_XOR_MODE;
	GrSetGCForeground(gp->gc, gp->fg_color);
	GrSetGCMode(gp->gc, GR_MODE_SET);
}

void
Java_java_awt_Toolkit_graSetXORMode( JNIEnv* env, jclass clazz, jobject _jgraphics, jint xor_color)
{
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
	}

	gp->flags |= TLK_GRAPHICS_FLAG_XOR_MODE;
	gp->xor_color = xor_color;
	GrSetGCForeground(gp->gc, gp->fg_color ^ xor_color);
	GrSetGCMode(gp->gc, GR_MODE_XOR);
}

void
Java_java_awt_Toolkit_graSetColor( JNIEnv* env, jclass clazz, jobject _jgraphics, jint native_color )
{
	TLKGraphics *gp;
	jint set_color;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}

	gp->fg_color = native_color;

	if ( gp->flags & TLK_GRAPHICS_FLAG_XOR_MODE ) {
		set_color = gp->fg_color ^ gp->xor_color;
	} else {
		set_color = gp->fg_color;
	}

	GrSetGCForeground(gp->gc, set_color);
}

void
Java_java_awt_Toolkit_graSetBackColor ( JNIEnv* env, jclass clazz, jobject _jgraphics, jint color)
{
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}

	gp->bg_color = color;
	GrSetGCBackground(gp->gc, color);
}

void
Java_java_awt_Toolkit_graSetFont(JNIEnv* env, jclass clazz, jobject _jgraphics, jobject _jfont)
{
	TLKGraphics *gp;
	
	if ( _jfont == NULL ) {
		SignalError("java.lang.NullPointerException", "no font object");
		return;
	}
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}

	GrSetGCFont(gp->gc, (GR_FONT_ID)_jfont);
}

void
Java_java_awt_Toolkit_graSetOffset ( JNIEnv* env, jclass clazz, jobject _jgraphics, jint xOff, jint yOff )
{
	TLKGraphics *gp;
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}
	gp->off_x = xOff;
	gp->off_y = yOff;
}

void
Java_java_awt_Toolkit_graDrawImage(JNIEnv* env, jclass clazz, jobject _jgraphics, jobject _jimage, jint grX, jint grY, jint imgX, jint imgY, jint width, jint height, jint native_bg_color )
{
	TLKGraphics *gp;
	GR_WINDOW_ID image;

	if ( _jimage == NULL ) {
		SignalError("java.lang.NullPointerException", "no object");
		return;
	}
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}
	image = (GR_WINDOW_ID)_jimage;

	GrSetGCForeground(gp->gc, native_bg_color);
	GrFillRect(gp->target, gp->gc, gp->off_x + grX , gp->off_y + grY, width, height);
	GrSetGCForeground(gp->gc, gp->fg_color);

	GrCopyArea(gp->target, gp->gc, gp->off_x + grX, gp->off_y + grY, width, height, image, imgX, imgY, MWROP_SRCCOPY);

	GrFlush();
}

void
Java_java_awt_Toolkit_graDrawImageScaled(JNIEnv* env, jclass clazz, jobject _jgraphics, jobject _jimage, 
jint grX0, jint grY0, jint grX1, jint grY1, 
jint imgX0, jint imgY0, jint imgX1, jint imgY1, 
jint native_bg_color )
{
#if 0
	GR_WINDOW_ID image;
	GR_WINDOW_INFO image_info;
	GR_PIXELVAL *pixelP;
	int x, y, xi, yi, off_y;
	float scale_x, scale_y;
	TLKGraphics *gp;

	if ( _jimage == NULL ) {
		SignalError("java.lang.NullPointerException", "no object");
		return;
	}
	
	gp = tlk_get_graphics((TLK_GRAPHICS_ID)_jgraphics);
	if ( gp == NULL ) {
		SignalError("java.lang.InternalError", "illegal TLK_GRAPHICS_ID");
		return;
	}
	image = (GR_WINDOW_ID)_jimage;

	GrGetWindowInfo(image, &image_info);

	pixelP = TLK_MALLOC((image_info.width - imgX0)*(image_info.height - imgY0));
	if ( pixelP == NULL ) {
		SignalError("java.lang.OutOfMemoryError", "can't alloc");
		return;
	}
	GrReadArea(image, imgX0, imgY0, image_info.width - imgX0, image_info.height - imgY0, pixelP);

	scale_y = (imgY1 - imgY0 + 1) / ( grY1 - grY0 + 1);
	scale_x = (imgX1 - imgX0 + 1) / ( grX1 - grX0 + 1);
	for( yi = grY0 ; yi <= grY1; yi++ ) {
		y = (yi - grY0) * scale_y;
		off_y = y * (image_info.width - imgX0) + imgY0;
		for( xi = grX0 ; xi < grX1; xi++ ) {
			x = (xi - grX0) * scale_x;
			if (((y + imgY0) >= image_info.height ) || ((x + imgX0) >= image_info.width )) {
				GrSetGCForeground(gp->gc, native_bg_color);
			} else {
				GrSetGCForeground(gp->gc, pixelP[off_y + x]);
			}
			GrPoint(gp->target, gp->gc, gp->off_x + xi, gp->off_x + yi);
		}
	}
	TLK_FREE(pixelP);
	GrFlush();
	GrSetGCForeground(gp->gc, gp->fg_color);
	return;
#endif
	Java_java_awt_Toolkit_graDrawImage(env, clazz, _jgraphics, _jimage, 
	grX0, grY0, imgX0, imgY0, imgX1 - imgX0, imgY1 - imgY0, native_bg_color);
}
