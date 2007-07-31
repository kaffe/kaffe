
/*
 * Copyright (c) 2006 - 2007
 *	Alper Akcan <alper@kaffe.org>, All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "toolkit.h"

KAFFE_GR_FUNC_DECL(jobject, Java_java_awt_Toolkit_graInitGraphics, jobject tgt, jint tgtType, jint xOff, jint yOff, jint xClip, jint yClip, jint wClip, jint hClip, jobject fnt, jint fg, jint bg, jboolean blank)
{
	s_rect_t coor;
	s_rect_t inter;
	s_surface_t *srf;
	graphics_t *gr = NULL;
	DEBUGF("Enter");
	if (jgr != NULL) {
		gr = UNVEIL_GR(jgr);
	}
	DEBUGF("xoff: %d, yoff: %d, xclip: %d, yclip: %d, wclip: %d, hclip: %d", xOff, yOff, xClip, yClip, wClip, hClip);
	if (tgt) {
		void *tgtPtr = JCL_GetRawData(env, tgt);
		DEBUGF("tgttype: %s", (tgtType == 0) ? "window" :
		                      (tgtType == 1) ? "Image" :
				      (tgtType == 2) ? "graphics" : "unknown");
		switch (tgtType) {
			case 0: /* window */
				srf = ((s_window_t *) tgtPtr)->surface;
				break;
			case 2: /* graphics */
				srf = ((graphics_t *) tgtPtr)->surface;
				break;
			case 1: /* image */
			default:
				exit(1);
				break;
		}
	} else {
		DEBUGF("tgt == 0");
		if (gr) {
			srf = gr->surface;
		} else {
			DEBUGF("attempt to set an uninitialized graphics");
			exit(1);
		}
	}
	if (gr != NULL) {
		srf = gr->surface;
	} else {
		gr = (graphics_t *) AWT_MALLOC(sizeof(graphics_t));
		jgr = JCL_NewRawDataObject(env, gr);
	}
	wClip = (wClip > 0) ? wClip : 0;
	hClip = (hClip > 0) ? hClip : 0;
	gr->font = UNVEIL_FONT(fnt);
	gr->surface = srf;
	gr->fg = fg;
	gr->bg = bg;
	gr->x0 = xOff;
	gr->y0 = yOff;
	gr->clip.x = gr->x0 + xClip;
	gr->clip.y = gr->y0 + yClip;
	gr->clip.w = wClip;
	gr->clip.h = hClip;
	if (blank) {
		coor.x = gr->x0 + xClip;
		coor.y = gr->y0 + yClip;
		coor.w = wClip;
		coor.h = hClip;
		if (s_rect_intersect(&(gr->clip), &coor, &inter) == 0) {
			s_fillbox(gr->surface, inter.x, inter.y, inter.w, inter.h, bg);
		}
	}
	DEBUGF("Leave");
	return jgr;
}

KAFFE_GR_FUNC_DECL(void, Java_java_awt_Toolkit_graSetClip, jint xClip, jint yClip, jint wClip, jint hClip)
{
	graphics_t *gr;
	DEBUGF("Enter");
	gr = UNVEIL_GR(jgr);
	DEBUGF("set clip; xClip: %d, yClip: %d, wClip: %d, hClip: %d", xClip, yClip, wClip, hClip);
	gr->clip.x = gr->x0 + xClip;
	gr->clip.y = gr->y0 + yClip;
	gr->clip.w = (wClip > 0) ? wClip : 0;
	gr->clip.h = (hClip > 0) ? hClip : 0;
	DEBUGF("Leave");
}

KAFFE_GR_FUNC_DECL(void, Java_java_awt_Toolkit_graSetColor, jint clr)
{
	graphics_t *gr;
	DEBUGF("Enter");
	gr = UNVEIL_GR(jgr);
	gr->fg = clr;
	DEBUGF("Leave");
}

KAFFE_GR_FUNC_DECL(void, Java_java_awt_Toolkit_graFillRect, jint x, jint y, jint width, jint height)
{
	s_rect_t coor;
	s_rect_t inter;
	graphics_t *gr;
	DEBUGF("Enter");
	gr = UNVEIL_GR(jgr);
	if ((width >= 0) && (height >= 0)) {
		coor.x = gr->x0 + x;
		coor.y = gr->y0 + y;
		coor.w = width;
		coor.h = height;
		if (s_rect_intersect(&(gr->clip), &coor, &inter) == 0) {
			s_fillbox(gr->surface, inter.x, inter.y, inter.w, inter.h, gr->fg);
		}
	}
	DEBUGF("Leave");
}

KAFFE_GR_FUNC_DECL(void, Java_java_awt_Toolkit_graFreeGraphics)
{
	graphics_t *gr;
	DEBUGF("Enter");
	gr = UNVEIL_GR(jgr);
	AWT_FREE(gr);
	DEBUGF("Leave");
}

KAFFE_GR_FUNC_DECL(void, Java_java_awt_Toolkit_graSetVisible, jint isVisible)
{
	graphics_t *gr;
	DEBUGF("Enter");
	gr = UNVEIL_GR(jgr);
	DEBUGF("setvisible: %d", isVisible);
	DEBUGF("Leave");
}

KAFFE_GR_FUNC_DECL(void, Java_java_awt_Toolkit_graClearRect, jint x, jint y, jint width, jint height)
{
	s_rect_t coor;
	s_rect_t inter;
	graphics_t *gr;
	DEBUGF("Enter");
	gr = UNVEIL_GR(jgr);
	coor.x = gr->x0 + x;
	coor.y = gr->y0 + y;
	coor.w = width;
	coor.h = height;
	if (s_rect_intersect(&(gr->clip), &coor, &inter) == 0) {
		s_fillbox(gr->surface, inter.x, inter.y, inter.w, inter.h, gr->bg);
	}
	DEBUGF("Leave");
}

KAFFE_GR_FUNC_DECL(void, Java_java_awt_Toolkit_graDrawImageScaled, jobject nimg, jint dx0, jint dy0, jint dx1, jint dy1, jint sx0, jint sy0, jint sx1, jint sy1, jint bgval)
{
	int x0;
	int y0;
	int x1;
	int y1;
	int iw;
	s_rect_t coor;
	s_rect_t inter;
	graphics_t *gr;
	s_image_t *img;
	s_surface_t *srfs;
	s_surface_t *srfi;
	DEBUGF("Enter");
	gr = UNVEIL_GR(jgr);
	img = UNVEIL_IMG(nimg);
	if (dx1 > dx0) { x0 = dx0; x1 = dx1;
	} else {         x0 = dx1; x1 = dx0;
	}
	if (dy1 > dy0) { y0 = dy0; y1 = dy1;
	} else {         y0 = dy1; y1 = dy0;
	}
	iw = img->w;
	if (sx0 < 0) sx0 = 0;
	if (sx1 < 0) sx1 = 0;
	if (sx0 >= iw) sx0 = iw - 1;
	if (sx1 >= iw) sx1 = iw - 1;
	if (s_surface_create(&srfs, x1 - x0 + 1, y1 - y0 + 1, gr->surface->bitsperpixel)) {
		goto out0;
	}
	if (s_surface_create(&srfi, img->w, img->h, gr->surface->bitsperpixel)) {
		goto out1;
	}
	s_fillbox(srfi, 0, 0, img->w, img->h, gr->bg);
	s_putboxrgba(srfi, 0, 0, img->w, img->h, img->rgba);
	s_scalebox(gr->surface, srfi->width, srfi->height, srfi->vbuf, srfs->width, srfs->height, srfs->vbuf);
	coor.x = gr->x0 + x0;
	coor.y = gr->y0 + y0;
	coor.w = srfs->width;
	coor.h = srfs->height;
	if (s_rect_intersect(&(gr->clip), &coor, &inter) == 0) {
		s_putboxpart(gr->surface, inter.x, inter.y, inter.w, inter.h, srfs->width, srfs->height, srfs->vbuf, inter.x - coor.x, inter.y - coor.y);
	}
	s_surface_destroy(srfi);
out1:	s_surface_destroy(srfs);
out0:	DEBUGF("Leave");
	return;
}

KAFFE_GR_FUNC_DECL(void, Java_java_awt_Toolkit_graDraw3DRect, jint x, jint y, jint width, jint height, jboolean raised, jint rgb)
{
	int xw;
	int yh;
	int dark;
	int bright;
	graphics_t *gr;
	DEBUGF("Enter");
	gr = UNVEIL_GR(jgr);
	dark = (int) (Java_java_awt_Toolkit_clrDark(env, clazz, rgb) >> 32);
	bright = (int) (Java_java_awt_Toolkit_clrBright(env, clazz, rgb) >> 32);
	x += gr->x0;
	y += gr->y0;
	xw = x + width;
	yh = y + height;
	s_hline(gr->surface, x, y, xw - 1, (raised) ? bright : dark);
	s_vline(gr->surface, x, y, yh, (raised) ? bright : dark);
	s_hline(gr->surface, x + 1, yh, xw, (raised) ? dark : bright);
	s_vline(gr->surface, xw, y, yh, (raised) ? dark : bright);
	DEBUGF("Leave");
}

KAFFE_GR_FUNC_DECL(void, Java_java_awt_Toolkit_graFill3DRect, jint x, jint y, jint width, jint height, jboolean raised, jint rgb)
{
	s_rect_t coor;
	s_rect_t inter;
	graphics_t *gr;
	DEBUGF("Enter");
	gr = UNVEIL_GR(jgr);
	coor.x = gr->x0 + x + 1;
	coor.y = gr->y0 + y + 1;
	coor.w = width - 2;
	coor.h = height - 2;
	if (s_rect_intersect(&(gr->clip), &coor, &inter) == 0) {
		s_fillbox(gr->surface, inter.x, inter.y, inter.w, inter.h, gr->fg);
	}
	Java_java_awt_Toolkit_graDraw3DRect(env, clazz, jgr, x, y, width - 1, height - 1, raised, rgb);
	DEBUGF("Leave");
}

KAFFE_GR_FUNC_DECL(void, Java_java_awt_Toolkit_graDrawString, jstring jstr, jint x, jint y)
{
	int r;
	int g;
	int b;
	char *str;
	s_rect_t coor;
	s_rect_t inter;
	graphics_t *gr;
	DEBUGF("Enter");
	gr = UNVEIL_GR(jgr);
	str = java2CString(env, jstr);
	DEBUGF("string: %s", str);
	s_font_set_str(gr->font, str);
	s_colorrgb(gr->surface, gr->fg, &r, &g, &b);
	s_font_set_rgb(gr->font, r, g, b);
	s_font_get_glyph(gr->font);
	coor.x = gr->x0 + x;
	coor.y = gr->y0 + y - gr->font->glyph.yMax;
	coor.w = gr->font->glyph.img->w;
	coor.h = gr->font->glyph.img->h;
	if (s_rect_intersect(&(gr->clip), &coor, &inter) == 0) {
		s_putboxpartrgba(gr->surface, inter.x, inter.y, inter.w, inter.h, coor.w, coor.h, gr->font->glyph.img->rgba, inter.x - coor.x, inter.y - coor.y);
	}
	AWT_FREE(str);
	DEBUGF("Leave");
}

KAFFE_GR_FUNC_DECL(void, Java_java_awt_Toolkit_graDrawLine, jint x1, jint y1, jint x2, jint y2)
{
	graphics_t *gr;
	DEBUGF("Enter");
	gr = UNVEIL_GR(jgr);
	if ((x1 == x2) && (y1 == y2)) {
		s_setpixel(gr->surface, gr->x0 + x1, gr->y0 + y1, gr->fg);
	} else if (x1 == x2) {
		s_vline(gr->surface, gr->x0 + x1, gr->y0 + y1, gr->y0 + y2, gr->fg);
	} else if (y1 == y2) {
		s_hline(gr->surface, gr->x0 + x1, gr->y0 + y1, gr->x0 + x2, gr->fg);
	} else {
		s_line(gr->surface, gr->x0 + x1, gr->y0 + y1, gr->x0 + x2, gr->y0 + y2, gr->fg);
	}
	DEBUGF("Leave");
}

KAFFE_GR_FUNC_DECL(void, Java_java_awt_Toolkit_graDrawChars, jcharArray jChars, jint offset, jint len, jint x, jint y)
{
	int i;
	int n;
	int r;
	int g;
	int b;
	char *str;
	jchar *jc;
	jchar *jco;
	jint isCopy;
	s_rect_t coor;
	s_rect_t inter;
	graphics_t *gr;
	DEBUGF("Enter");
	if (!jChars) {
		return;
	}
	gr = UNVEIL_GR(jgr);
	n = (*env)->GetArrayLength(env, jChars);
	jc = (*env)->GetCharArrayElements(env, jChars, &isCopy);
	jco = jc + offset;
	if (offset + len > n) {
		n = n - offset;
	} else {
		n = len;
	}
	if (n <= 0) {
		return;
	}
	str = (char *) AWT_MALLOC(sizeof(char) * (n + 1));
	for (i = 0; i < n; i++) {
		*(str + i) = *(jco + i);
	}
	*(str + i) = '\0';
	DEBUGF("string: %s", str);
	s_font_set_str(gr->font, str);
	s_colorrgb(gr->surface, gr->fg, &r, &g, &b);
	s_font_set_rgb(gr->font, r, g, b);
	s_font_get_glyph(gr->font);
	coor.x = gr->x0 + x;
	coor.y = gr->y0 + y - gr->font->glyph.yMax;
	coor.w = gr->font->glyph.img->w;
	coor.h = gr->font->glyph.img->h;
	if (s_rect_intersect(&(gr->clip), &coor, &inter) == 0) {
		s_putboxpartrgba(gr->surface, inter.x, inter.y, inter.w, inter.h, coor.w, coor.h, gr->font->glyph.img->rgba, inter.x - coor.x, inter.y - coor.y);
	}
	AWT_FREE(str);
	(*env)->ReleaseCharArrayElements(env, jChars, jc, JNI_ABORT);
}

KAFFE_GR_FUNC_DECL(void, Java_java_awt_Toolkit_graCopyArea, jint x, jint y, jint width, jint height, jint xDelta, jint yDelta)
{
	char *vbuf;
	s_rect_t coor;
	s_rect_t inter;
	graphics_t *gr;
	DEBUGF("Enter");
	gr = UNVEIL_GR(jgr);
	vbuf = (char *) AWT_MALLOC(width * height * gr->surface->bytesperpixel);
	s_getbox(gr->surface, gr->x0 + x, gr->y0 + y, width, height, vbuf);
	coor.x = gr->x0 + x + xDelta;
	coor.y = gr->y0 + y + yDelta;
	coor.w = width;
	coor.h = height;
	if (s_rect_intersect(&(gr->clip), &coor, &inter) == 0) {
		s_putboxpart(gr->surface, inter.x, inter.y, inter.w, inter.h, coor.w, coor.h, vbuf, inter.x - coor.x, inter.y - coor.y);
	}
	AWT_FREE(vbuf);
	DEBUGF("Leave");
}

KAFFE_GR_FUNC_DECL(void, Java_java_awt_Toolkit_graSetFont, jobject jfnt)
{
	s_font_t *fnt;
	graphics_t *gr;
	DEBUGF("Enter");
	gr = UNVEIL_GR(jgr);
	fnt = UNVEIL_FONT(jfnt);
	gr->font = fnt;
	DEBUGF("Leave");
}

KAFFE_GR_FUNC_DECL(void, Java_java_awt_Toolkit_graDrawRect, jint x, jint y, jint width, jint height)
{
	DEBUGF("Enter");
	Java_java_awt_Toolkit_graDrawLine(env, clazz, jgr, x, y, x + width - 1, y);
	Java_java_awt_Toolkit_graDrawLine(env, clazz, jgr, x, y + height - 1, x + width - 1, y + height - 1);
	Java_java_awt_Toolkit_graDrawLine(env, clazz, jgr, x, y, x, y + height - 1);
	Java_java_awt_Toolkit_graDrawLine(env, clazz, jgr, x + width - 1, y, x + width - 1, y);
	DEBUGF("Leave");
}
