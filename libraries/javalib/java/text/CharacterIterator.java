package java.text;

public interface CharacterIterator extends Cloneable {

public static final char DONE = (char)0xFFFF;

public abstract char first();
public abstract char last();
public abstract char current();
public abstract char next();
public abstract char previous();
public abstract void setIndex(int pos);
public abstract int getBeginIndex();
public abstract int getEndIndex();
public abstract int getIndex();
public abstract Object clone();

};
