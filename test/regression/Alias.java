// Demonstrates Serialization aliasing bug

import java.io.*;

class Pt implements Serializable {
	public Pt(int x, int y) {
		this.x=x;
		this.y=y;
	}

	public void move(int dx, int dy) {
		x+=dx;
		y+=dy;
	}

	public boolean equals(Object obj) {
		if (!(obj instanceof Pt))
			return false;
		Pt other=(Pt)obj;
		return x==other.x && y==other.y;
	}

	public int hashCode() {
		return x+y;
	}

	public String toString() {
		return "("+x+", "+y+")";
	}

	private int x, y;
};

class Rect implements Serializable {
	private Pt corner1, corner2;

	public Rect(int x1, int y1, int x2, int y2) {
		corner1=new Pt(x1, y1);
		corner2=new Pt(x2, y2);
	}

	public void move(int dx, int dy) {
		corner1.move(dx, dy);
		corner2.move(dx, dy);
	}

	public String toString() {
		return corner1.toString()+"-"+corner2;
	}
}

public class Alias {
	public static void main(String args[]) {
		try {
			Rect rect1=new Rect(1,1,1,1);
			ByteArrayOutputStream baos=new ByteArrayOutputStream();
			ObjectOutputStream oos=new ObjectOutputStream(baos);
			oos.writeObject(rect1);
			oos.flush();
			byte data[]=baos.toByteArray();
			System.out.println("Tam: "+data.length);
			ByteArrayInputStream bais=new ByteArrayInputStream(data);
			ObjectInputStream ois=new ObjectInputStream(bais);
			Rect rect2=(Rect)ois.readObject();
			System.out.println("rect1="+rect1+" rect2="+rect2);
			rect1.move(2,2);
			rect2.move(2,2);
			System.out.println("rect1="+rect1+" rect2="+rect2);
		} catch(Exception e) {
			e.printStackTrace();
		}
	}
};


/* Expected Output:
Tam: 106
rect1=(1, 1)-(1, 1) rect2=(1, 1)-(1, 1)
rect1=(3, 3)-(3, 3) rect2=(3, 3)-(3, 3)
*/
