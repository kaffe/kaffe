package java.awt;

import java.lang.ref.WeakReference;

/**
 * GraphicsLink is a auxiliary construct to implement functionality which
 * is usually done by native window systems. Resident Graphicses for
 * native-like Components are automatically updated (by the window system)
 * in case their target Component changes visibility and/or bounds. Since
 * we don't rely on the native system, we have to keep track of resident
 * Graphics objects by ourselves. But unfortunately, many apps are rather
 * careless with Graphics objects, never disposing them. If we directly
 * link Graphics objects into native-like Components (just relying on
 * dispose in order to unlink them), we have a great memory leak. Even
 * worse, if the list isn't reorganized, subsequent Graphics updates
 * become slower and slower (leave alone the dangers of recursive list
 * traversal). The answer is WeakReference. Our ref graph looks like
 * this:

                 (per-Graphics cache)
                +---------------------+   ...
                |                     |    ^
                |          (weak)     V    |       native-like
       NativeGraphics  <- - - - -  GraphicsLink <-- Component
                  |                                    ^
                  |                                    |
                  +------------------------------------+

 * The trick is to have no strong references from the Component
 * to the Graphics objects.
 * If somebody forgets about the Graphics.dispose, the link will
 * be removed the next time the list is traversed (either when
 * linking in a new Graphics, updating linked Graphicses, or
 * - highly recommended - during a explicit Graphics.dispose().
 *
 * Note that WeakReferences are immutable (why?), i.e. we can't
 * cache them independently of Graphics objects. In order to
 * avoid as much heap traffic as possible during draw operations,
 * we cache the GraphicsLink objects inside of their Graphicses
 */
class GraphicsLink
  extends WeakReference
{
	GraphicsLink next;
	int xOffset;
	int yOffset;
	boolean isVisible;
	int width;
	int height;

GraphicsLink ( NativeGraphics g ){
	super( g);
}

boolean updateGraphics ( Component tgt ) {
	NativeGraphics g = (NativeGraphics)get();
	
	if ( g != null ) {
		boolean isTgtVisible = ((tgt.flags & tgt.IS_SHOWING) == tgt.IS_SHOWING);
	
		if ( isTgtVisible != isVisible ){
			Toolkit.graSetVisible( g.nativeData, isTgtVisible);
			isVisible = isTgtVisible;
		}

		if ( isTgtVisible ) {
			if ( (g.xClip == 0) && (g.yClip == 0) &&
			     (g.wClip == width) && (g.hClip == height) ){
			  NativeGraphics.getClippedGraphics( g, tgt,
		 	                 g.xOffset - xOffset, g.yOffset - yOffset,
	 	                   0, 0, tgt.width, tgt.height, false);
				width = g.wClip;
				height = g.hClip;
			}
			else {
		  	NativeGraphics.getClippedGraphics( g, tgt,
		 	                 g.xOffset - xOffset, g.yOffset - yOffset,
	 	                   g.xClip, g.yClip, g.wClip, g.hClip, false);
			}
		}

		xOffset = g.xOffset;
		yOffset = g.yOffset;

		return true;
	}
	else {
		return false;
	}
}
}
