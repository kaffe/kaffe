package java.text;

public class ParsePosition {

int idx;

public ParsePosition(int idx) {
	this.idx = idx;
}

public int getIndex() {
	return (idx);
}

public void setIndex(int idx) {
	this.idx = idx;
}

}
