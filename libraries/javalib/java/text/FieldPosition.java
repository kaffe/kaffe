package java.text;

public class FieldPosition {

int field;
int begin;
int end;

public FieldPosition(int field) {
	this.field = field;
	this.begin = 0;
	this.end = 0;
}

public int getBeginIndex() {
	return (begin);
}

public int getEndIndex() {
	return (end);
}

public int getField() {
	return (field);
}

}
