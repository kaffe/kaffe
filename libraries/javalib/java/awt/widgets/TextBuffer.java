package java.awt;


class TextBuffer
{
	char[] buf;
	int len;
	int tabWidth;
	FontMetrics fm;
	static int defLen = 128;

public TextBuffer() {
	this( defLen);
}

public TextBuffer( String str) {
	this( defLen);
	append( str);
}

public TextBuffer( int length) {
	buf = new char[length];
}

public void append( String s) {
	insert( len, s);
}

public void append( TextBuffer tb) {
	checkCapacity( len + tb.len);
	for ( int i=0; i<tb.len; i++)
		buf[len++] = tb.buf[i];
}

public void append( char c) {
	insert( len, c);
}

void checkCapacity( int newLen) {
	if ( newLen <= buf.length)
		return;
	int nl = buf.length+defLen;
	if ( newLen > nl)
		nl = newLen;
	char[] pb = buf;
	buf = new char[nl];
	System.arraycopy( pb, 0, buf, 0, pb.length);
}

public void copyLevelFrom( TextBuffer tb){
	int orgLevel = tb.getLevel();
	int level = getLevel();
	
	for ( int i = 0; i<orgLevel - level; i++)
		insert( 0, '\t');
}

public int getIdx( int pos){
	return getIdxFrom( 0, pos);
}

public int getIdxFrom( int start, int pos){
	int idx = start;
	int x = 0;
	
	if ( pos < 0 ) {
		if ( idx == 0 )
			return 0;
		for ( idx--; idx >= 0; idx--) {
			if ( buf[idx] == '\t')
				x = (x+tabWidth)/tabWidth*tabWidth;
			else
				x += fm.charWidth( buf[idx]);
			if ( -x < pos)
				return idx;
		}
	}
	else {
		for ( ; idx<len; idx++){
			if ( buf[idx] == '\t')
				x = (x+tabWidth)/tabWidth*tabWidth;
			else
				x += fm.charWidth( buf[idx]);
			if ( x > pos)
				return idx;
		}
	}
	return idx;
}

public int getLevel(){
	int level = 0;
	
	for ( int i=0; i<len; i++) {
		if ( buf[i] != '\t' )
			break;
		level++;
	}

	return level;	
}

public int getPos( int idx){
	int x = 0;

	if ( idx > len)
		idx = len;

	for ( int i=0; i<idx; i++){
		if ( buf[i] == '\t')
			x = (x+tabWidth)/tabWidth*tabWidth;
		else 
			x += fm.charWidth( buf[i]);
	}
	return x;
}

public String getString( int start, int len){
	if ( this.len < start + len)
		len = this.len - start;
	if ( len > 0)
		return new String( buf, start, len);
	return null;
}

public int getWidth(){
	int x0 = getPos( 0);
	int x1 = getPos( len);
	return x1 - x0;
}

public int getWidth( int sIdx, int eIdx){
	int x0 = getPos( sIdx);
	int x1 = getPos( eIdx);
	return x1 - x0;
}

public void insert( int idx, String s) {
	replace( idx, 0, s);
}

public void insert( int idx, char c) {
	replace( idx, 0, c);
}

public static void main( String[] args) {
	TextBuffer tb = new TextBuffer();
	tb.append( "12356789");
	System.out.println( tb);
	tb.insert( 3, '4');
	System.out.println( tb);
	tb.replace( 3, 2, "xxxxxxxx"); 
	System.out.println( tb);
	tb.replace( 3, 8, "45");
	System.out.println( tb);
	tb.remove( 0, 2);
	System.out.println( tb);
}

public void paint( Graphics g, int x, int y, int height) {
	paint( g, x, y, height, 0, len);
}

public void paint( Graphics g, int x, int y, int height, int start) {
	paint( g, x, y, height, start, len - start);
}

public void paint( Graphics g, int x, int y, int height, int start, int len) {
	int x0 = getPos( start);
	int y0;
	int i0 = start;
	int idx = start;

	if ( fm == null) {
		fm = g.getFontMetrics();
		tabWidth = 4 * fm.charWidth('x');
	}
	
	y0 = y + height - (height - fm.getHeight() ) / 2 - fm.getDescent();
	
	if ( this.len < start + len)
		len = this.len - start;
		
	for ( ; idx<start+len; idx++) {
		if ( buf[idx] == '\t' ){
			if ( idx > i0) {
				g.drawChars( buf, i0, idx-i0, x+x0, y0);
				x0 += fm.charsWidth( buf, i0, idx-i0);
			}
			i0 = idx+1;
			x0 = (x0 + tabWidth)/tabWidth * tabWidth;
		}
	}

	if ( i0 < idx)
		g.drawChars( buf, i0, idx-i0, x+x0, y0);
}

public void paintFrom( Graphics g, int x, int y, int height, int bIdx, int start, int len) {
	int x0, y0;
	int i0 = start;
	int idx = start;

	if ( fm == null) {
		fm = g.getFontMetrics();
		tabWidth = 4 * fm.charWidth('x');
	}
	
	x0 = getWidth( bIdx, start);
	y0 = y + height - (height - fm.getHeight() ) / 2 - fm.getDescent();
	
	if ( this.len < start + len)
		len = this.len - start;
		
	for ( ; idx<start+len; idx++) {
		if ( buf[idx] == '\t' ){
			if ( idx > i0) {
				g.drawChars( buf, i0, idx-i0, x+x0, y0);
				x0 += fm.charsWidth( buf, i0, idx-i0);
			}
			i0 = idx+1;
			x0 = (x0 + tabWidth)/tabWidth * tabWidth;
		}
	}

	if ( i0 < idx)
		g.drawChars( buf, i0, idx-i0, x+x0, y0);
}

public void remove( int start, int len) {
	if ( (start > this.len) || (len <= 0) )
		return;
	int ll = this.len - start - len;
	if ( ll > 0 )
		System.arraycopy( buf, start+len, buf, start, ll);
	this.len -= len; 
}

public void replace( int start, int len, String s) {
	int sLen = ( s != null) ? s.length() : 0;
	int dl = sLen - len;

	checkCapacity( this.len+dl);
	int eIdx = start+len;
	System.arraycopy( buf, eIdx, buf, eIdx+dl, this.len-eIdx);
	
	if ( s != null)
		s.getChars(0, sLen, buf, start);
	this.len += dl; 
}

public void replace( int start, int len, char c) {
	int dl = 1 - len;

	checkCapacity( this.len+dl);
	int eIdx = start+len;
	System.arraycopy( buf, eIdx, buf, eIdx+dl, this.len-eIdx);
	
	buf[start] = c;
	this.len += dl; 
}

public void set( String s) {
	if ( (s == null) || (s.length() == 0) ){
		len = 0;
	}
	else {
		int sLen = s.length();
		checkCapacity( sLen);
		s.getChars( 0, sLen, buf, 0);
		len = sLen;
	}
}

public void set( char c, int count) {
	checkCapacity( count);
	
	for ( len = 0; len < count; len++)
		buf[len] = c;
}

public void setMetrics( FontMetrics fm, int tabWidth) {
	this.fm = fm;
	this.tabWidth = tabWidth;
}

public String toString() {
	return new String( buf, 0, len);
}
}
