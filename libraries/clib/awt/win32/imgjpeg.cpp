/**
* imgjpeg.cpp - interface for libjpeg GIF input routines
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#include "toolkit.hpp"

#if defined(HAVE_JPEGLIB_H) && defined(HAVE_LIBJPEG)
#define INCLUDE_JPEG 1
#endif

/* references to img.cpp */
Image* createImage ( int width, int height );

extern "C" {

#ifdef INCLUDE_JPEG
#include "setjmp.h"
#include "jpeglib.h"
#include "jversion.h"
#include "jerror.h"

#endif
	
	void Java_java_awt_Toolkit_imgFreeImage( JNIEnv* env, jclass clazz, Image * img);
	
#define MAX_JPEG_COLORS 256
	
#ifdef INCLUDE_JPEG
	
	/*******************************************************************************************
	* required typedefs and structs
	*/
	
	struct error_mgr {
		struct jpeg_error_mgr pub;
		jmp_buf setjmp_buffer;
	};
	
	typedef struct error_mgr * error_ptr;
	
	METHODDEF(void)
		error_exit ( j_common_ptr cinfo)
	{
		error_ptr myerr = (error_ptr) cinfo->err;
		(*cinfo->err->output_message)(cinfo);
		longjmp(myerr->setjmp_buffer, 1);
	}
	
	typedef struct {
		struct jpeg_source_mgr pub;	/* public fields */
		
		long   buflen;       /* length of external buffer */
		JOCTET * buffer;		/* start of buffer */
		
		boolean start_of_file;	/* any data yet? */
	} bufsrc_mgr;
	
	typedef bufsrc_mgr * bufsrc_ptr;
	

#define BUF_SIZE 2048
	
	typedef struct {
		struct jpeg_source_mgr pub;	/* public fields */
		
		int fd;
		JOCTET *buffer;
	} filesrc_mgr;
	
	typedef filesrc_mgr * filesrc_ptr;


	/*******************************************************************************************
	* memory buffer extension for the standard jpeg lib input management (jdatasrc.c)
	*/
	
	METHODDEF(void)
		init_buf_source (j_decompress_ptr cinfo)
	{
		bufsrc_ptr src = (bufsrc_ptr) cinfo->src;
		
		/* We reset the empty-input-file flag for each image,
		* but we don't clear the input buffer.
		* This is correct behavior for reading a series of images from one source.
		*/
		src->start_of_file = TRUE;
	}
	
	
	static JOCTET end_marker[2] = { (JOCTET) 0xFF, (JOCTET) JPEG_EOI };
	
	METHODDEF(boolean)
		get_buf_source (j_decompress_ptr cinfo)
	{
		bufsrc_ptr src = (bufsrc_ptr) cinfo->src;
		
		if ( src->pub.next_input_byte && (src->start_of_file == FALSE) ) {
			WARNMS(cinfo, JWRN_JPEG_EOF);
			src->pub.next_input_byte = end_marker;
			src->pub.bytes_in_buffer = sizeof(end_marker);
		}
		else {                    /* we read in one seep (no need to use two buffers) */
			src->pub.next_input_byte = src->buffer;
			src->pub.bytes_in_buffer = src->buflen;
			src->start_of_file = FALSE;
		}
		
		return TRUE;
	}
	
	
	METHODDEF(void)
		skip_buf_source ( j_decompress_ptr cinfo, long num_bytes )
	{
		bufsrc_ptr src = (bufsrc_ptr) cinfo->src;
		
		if ( src->start_of_file == TRUE )
			get_buf_source( cinfo);
		
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
	
	
	METHODDEF(void)
		term_buf_source (j_decompress_ptr cinfo)
	{
		bufsrc_ptr src = (bufsrc_ptr) cinfo->src;
		
		/*
		* Take care the (external) buffer isn't deleted by jpeg_destroy_decompress.
		* This shouldn't be necessary since jpeg_destroy cleans up by means of the 
		* jpeg memory manager, but we better play it safe
		*/
		src->buffer = 0;
	}
	
	
	GLOBAL(void)
		jpeg_buffer_src (j_decompress_ptr cinfo, unsigned char* buf, long len )
	{
		bufsrc_ptr src;
		
		if (cinfo->src == NULL) {	/* first time for this JPEG object? */
			cinfo->src = (struct jpeg_source_mgr *)
				(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				sizeof(bufsrc_mgr));
		}
		
		src = (bufsrc_ptr) cinfo->src;
		src->pub.init_source = init_buf_source;
		src->pub.fill_input_buffer = get_buf_source;
		src->pub.skip_input_data = skip_buf_source;
		src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
		src->pub.term_source = term_buf_source;
		
		src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
		src->pub.next_input_byte = NULL; /* until buffer loaded */
		
		src->buffer = buf;
		src->buflen = len;
	}
	
	

	/*******************************************************************************************
	* file desriptor read extension for the standard jpeg lib input management (jdatasrc.c)
	*/
	
	METHODDEF(void)
		init_file_source (j_decompress_ptr cinfo)
	{
		/* nothing to do */
	}
	
	METHODDEF(boolean)
		get_file_source (j_decompress_ptr cinfo)
	{
		filesrc_ptr src = (filesrc_ptr) cinfo->src;
		int         n = AWT_READ( src->fd, src->buffer, BUF_SIZE);
		
		if ( n < 0 ) {
			WARNMS(cinfo, JWRN_JPEG_EOF);
			src->pub.next_input_byte = end_marker;
			src->pub.bytes_in_buffer = sizeof(end_marker);
		}
		else {
			src->pub.next_input_byte = src->buffer;
			src->pub.bytes_in_buffer = n;
		}
		
		return TRUE;
	}
	
	
	METHODDEF(void)
		skip_file_source ( j_decompress_ptr cinfo, long num_bytes )
	{
		filesrc_ptr src = (filesrc_ptr) cinfo->src;
		
		if ( (unsigned long)num_bytes < src->pub.bytes_in_buffer ){
			src->pub.next_input_byte += num_bytes;
			src->pub.bytes_in_buffer -= num_bytes;
		}
		else {
			num_bytes -= src->pub.bytes_in_buffer;
			
			AWT_SETPOS( src->fd, num_bytes);
			get_file_source( cinfo);
		}
	}
	
	
	METHODDEF(void)
		term_file_source (j_decompress_ptr cinfo)
	{
	/* nothing to do, here
	* the file is closed by our caller, the buffer is allocated on the stack
		*/
	}
	
	
	GLOBAL(void)
		jpeg_file_src (j_decompress_ptr cinfo, int fd )
	{
		filesrc_ptr src;
		
		if (cinfo->src == NULL) {	/* first time for this JPEG object? */
			src = (filesrc_ptr)
				(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				sizeof(filesrc_mgr));
			src->buffer = (JOCTET *)
				(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				BUF_SIZE * sizeof(JOCTET));
			cinfo->src = (struct jpeg_source_mgr *) src;
		}
		else {
			src = (filesrc_ptr) cinfo->src;
		}
		
		src->pub.init_source = init_file_source;
		src->pub.fill_input_buffer = get_file_source;
		src->pub.skip_input_data = skip_file_source;
		src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
		src->pub.term_source = term_file_source;
		
		src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
		src->pub.next_input_byte = NULL; /* until buffer loaded */
		
		src->fd = fd;
	}
	

	/************************************************************************************
	* JPEG auxiliary funcs (production callbacks)
	*/
	
	void
		jscan_to_img( Image * img, JSAMPROW buf, struct jpeg_decompress_struct * cinfo)
	{
		register unsigned int col, pix, idx;
		register JSAMPARRAY colormap = cinfo->colormap;
		
		for ( col = 0; col < cinfo->output_width; col++) {
			idx = *buf++;
			pix = (colormap[2][idx] << 16) | (colormap[1][idx] << 8) | colormap[0][idx];    
			PIXEL( img->dc, col, cinfo->output_scanline-1, pix);
		}
	}

	/************************************************************************************
	* reduced Jpeg error handling (we might not have stdio)
	*/
#define JMESSAGE(code,string)	string ,
	
	const char * const jpeg_std_message_table[] = {
//#include "jerror.h"
		NULL
	};


	METHODDEF(void)
		min_error_exit (j_common_ptr cinfo)
	{
	}
	
	METHODDEF(void)
		min_output_message (j_common_ptr cinfo)
	{
	}
			
	METHODDEF(void)
		min_emit_message (j_common_ptr cinfo, int msg_level)
	{
	}
	
	METHODDEF(void)
		min_format_message (j_common_ptr cinfo, char * buffer)
	{
#ifdef NEVER
		struct jpeg_error_mgr * err = cinfo->err;
		int msg_code = err->msg_code;
		const char * msgtext = NULL;
		const char * msgptr;
		char ch;
		boolean isstring;

		/* Look up message string in proper table */
		if (msg_code > 0 && msg_code <= err->last_jpeg_message) {
			msgtext = err->jpeg_message_table[msg_code];
		} else if (err->addon_message_table != NULL &&
			msg_code >= err->first_addon_message &&
			msg_code <= err->last_addon_message) {
			msgtext = err->addon_message_table[msg_code - err->first_addon_message];
		}
		
		/* Defend against bogus message number */
		if (msgtext == NULL) {
			err->msg_parm.i[0] = msg_code;
			msgtext = err->jpeg_message_table[0];
		}
		
		/* Check for string parameter, as indicated by %s in the message text */
		isstring = FALSE;
		msgptr = msgtext;
		while ((ch = *msgptr++) != '\0') {
			if (ch == '%') {
				if (*msgptr == 's') isstring = TRUE;
				break;
			}
		}
		
		//...
#endif
	}
 
	METHODDEF(void)
		min_reset_error_mgr (j_common_ptr cinfo)
	{
		cinfo->err->num_warnings = 0;
		/* trace_level is not reset since it is an application-supplied parameter */
		cinfo->err->msg_code = 0;	/* may be useful as a flag for "no error" */
	}


	GLOBAL(struct jpeg_error_mgr *)
		jpeg_min_error (struct jpeg_error_mgr * err)
	{
		err->error_exit = min_error_exit;
		err->emit_message = min_emit_message;
		err->output_message = min_output_message;
		err->format_message = min_format_message;
		err->reset_error_mgr = min_reset_error_mgr;
		
		err->trace_level = 0;		/* default = no tracing */
		err->num_warnings = 0;	/* no warnings emitted yet */
		err->msg_code = 0;		/* may be useful as a flag for "no error" */
		
		/* Initialize message table pointers */
		err->jpeg_message_table = jpeg_std_message_table;
		err->last_jpeg_message = (int) JMSG_LASTMSGCODE - 1;
		
		err->addon_message_table = NULL;
		err->first_addon_message = 0;	/* for safety */
		err->last_addon_message = 0;
		
		return err;
	}

	/************************************************************************************
	* Jpeg production
	*/
	
	Image*
		readJpeg ( struct jpeg_decompress_struct* cinfo, int colors )
	{
		struct error_mgr jerr;
		JSAMPARRAY buffer;
		Image      *img = 0;
		int        row_stride;
		
		cinfo->err = jpeg_min_error(&jerr.pub);

		jerr.pub.error_exit = error_exit;
		
		if ( setjmp(jerr.setjmp_buffer)) {
			if ( img )
				Java_java_awt_Toolkit_imgFreeImage( 0, 0, img);
			return 0;
		}
		
		jpeg_read_header( cinfo, TRUE);
		
		if ( colors < 8 )
			colors = 8;
		else if ( colors > 256 )
			colors = 256;
		
		cinfo->desired_number_of_colors = colors;
		cinfo->quantize_colors = TRUE;
		cinfo->out_color_space = JCS_RGB;
		
		jpeg_start_decompress( cinfo);
		row_stride = cinfo->output_width * cinfo->output_components;
		
		/* it's time to create the target image */
		img = createImage( cinfo->output_width, cinfo->output_height);
		
		buffer = (*cinfo->mem->alloc_sarray)((j_common_ptr) cinfo, JPOOL_IMAGE, row_stride, 1);
		
		while (cinfo->output_scanline < cinfo->output_height) {
			jpeg_read_scanlines( cinfo, buffer, 1);
			jscan_to_img( img, buffer[0], cinfo);
		}
		
		jpeg_finish_decompress( cinfo);
		
		return img;
	}
	
#endif /* INCLUDE_JPEG */
	
	
	/**************************************************************************************
	* these are the "exported" production interface functions
	*/

	Image*
		readJpegFile ( int infile )
	{
		Image *img = 0;
		
#ifdef INCLUDE_JPEG
		struct jpeg_decompress_struct cinfo;
		
		jpeg_create_decompress( &cinfo);
		jpeg_file_src( &cinfo, infile);
		
		img = readJpeg( &cinfo, MAX_JPEG_COLORS);
		
		jpeg_destroy_decompress( &cinfo);
#endif /* INCLUDE_JPEG */
		
		return img;
	}

	
	Image*
		readJpegData ( unsigned char* buf, long len )
	{
		Image *img = 0;
		
#ifdef INCLUDE_JPEG
		struct jpeg_decompress_struct cinfo;
		
		jpeg_create_decompress( &cinfo);
		jpeg_buffer_src( &cinfo, buf, len);
		
		img = readJpeg( &cinfo, MAX_JPEG_COLORS);
		
		jpeg_destroy_decompress( &cinfo);
#endif /* INCLUDE_JPEG */
		
		return img;
	}
	
}