/* Written by Artur Biesiadowski <abies@pg.gda.pl> */

/*
   This class test basic 4 conversion types and compares results to ready ones, done
   on sure VM (suns JDK). Conversions are
   (obj instanceof clazz)
   (clazz)obj
   clazz.isInstance(obj)
   clazz1.isAssignableFrom(clazz2);

   Hopefully all needed cases are covered. If you want to add object just put it
   into objs table. If you want to add class, you need to add it to both cls and to
   testCode method. Of course you need to regenerate results after that.
 */


/*
   You can copy/modify/use this file for any purposes, as long as you do not delete
   my name from top of that file. Of course you can add your own below that :)
 */

import java.io.Serializable;

interface I1 {}
interface I2 {}
interface I3 extends I2{}
class A1 implements I1 {}
class AB12 extends A1 implements I2 {}
class ABC12 extends AB12 {}
class D3 implements I3 {}

public class TestCasts
{

    static void reportClIsInst(Object obj, Class cl, boolean b )
    {
	if ( verbose )
            System.out.println(obj +"\tis\t"+ cl + "\t?" + b);
	else
	    System.out.print(b?'t':'f');
    }

    static void reportClIsAssign( Class c1, Class c2, boolean b )
    {
	if (verbose)
	    System.out.println(c1 + "\tisAssignableFrom\t" + c2 + "\t?\t" + b);
	else
	    System.out.print(b?'t':'f');
    }

    static void reportInstanceof( Object obj, Class cl, boolean b )
    {
	if ( verbose )
	    System.out.println(obj +"\tinstanceof\t"+ cl + "\t?" + b);
	else
	    System.out.print(b?'t':'f');
    }

    static void reportCast( Object obj, Class cl, boolean b )
    {
	if ( verbose )
	    System.out.println(obj +"\tcastto     \t"+ cl + "\t?" + b);
	else
	    System.out.print(b?'t':'f');
    }

    static final boolean verbose = false;

    public static void main(String argv[] )
    {
	test();
	System.out.println();
    }


    static void test()
    {
	int i;
	int j;
	for ( i=0; i < objs.length; i++ )
	    {
		for ( j=0; j < cls.length; j++ )
		    {
			reportClIsInst(objs[i], cls[j], cls[j].isInstance(objs[i]) );
		    }
	    }

	for (i=0; i < objs.length; i++ )
	    {
		testCode(objs[i]);
	    }

	for ( i=0; i < cls.length; i++ )
	    {
		for ( j=0; j < cls.length; j++ )
		    {
			reportClIsAssign(cls[i], cls[j], cls[i].isAssignableFrom(cls[j]));
		    }
	    }
    }

    static final Class intArray = new int[0].getClass();

    public static final Object objs[] =
    {
	null,
	new Object(),
	new A1(),
	new AB12(),
	new ABC12(),
	new D3(),
	new A1[1],
	new AB12[1],
	new ABC12[1],
	new D3[1],
	new I1[1],
	new I2[1],
	new I3[1],
	new int[1],
	new A1[1][1],
	new AB12[1][1],
	new I1[1][1]
    };

    public static final Class cls[] =
    {
	Object.class,
	A1.class,
	AB12.class,
	ABC12.class,
	D3.class,
	I1.class,
	I2.class,
	I3.class,
	Cloneable.class,
	Serializable.class,
	A1[].class,
	AB12[].class,
	ABC12[].class,
	D3[].class,
	I1[].class,
	I2[].class,
	I3[].class,
	intArray,
	A1[][].class,
	AB12[][].class,
	I1[][].class
    };

    static void testCode(Object o)
    {

	reportInstanceof(o, Object.class, (o instanceof Object) );
	try
	    {
		Object r1 = (Object) o;
		reportCast(o, Object.class, true );
	    } catch (ClassCastException e) {
		reportCast(o,Object.class, false );
	    }

	reportInstanceof(o, A1.class, (o instanceof A1) );
	try
	    {
		A1 r1 = (A1) o;
		reportCast(o, A1.class, true );
	    } catch (ClassCastException e) {
		reportCast(o,A1.class, false );
	    }
	reportInstanceof(o, AB12.class, (o instanceof AB12) );
	try
	    {
		AB12 r1 = (AB12) o;
		reportCast(o, AB12.class, true );
	    } catch (ClassCastException e) {
		reportCast(o,AB12.class, false );
	    }
	reportInstanceof(o, ABC12.class, (o instanceof ABC12) );
	try
	    {
		ABC12 r1 = (ABC12) o;
		reportCast(o, ABC12.class, true );
	    } catch (ClassCastException e) {
		reportCast(o,ABC12.class, false );
	    }
	reportInstanceof(o, D3.class, (o instanceof D3) );
	try
	    {
		D3 r1 = (D3) o;
		reportCast(o, D3.class, true );
	    } catch (ClassCastException e) {
		reportCast(o,D3.class, false );
	    }
	reportInstanceof(o, I1.class, (o instanceof I1) );
	try
	    {
		I1 r1 = (I1) o;
		reportCast(o, I1.class, true );
	    } catch (ClassCastException e) {
		reportCast(o,I1.class, false );
	    }
	reportInstanceof(o, I2.class, (o instanceof I2) );
	try
	    {
		I2 r1 = (I2) o;
		reportCast(o, I2.class, true );
	    } catch (ClassCastException e) {
		reportCast(o,I2.class, false );
	    }
	reportInstanceof(o, I3.class, (o instanceof I3) );
	try
	    {
		I3 r1 = (I3) o;
		reportCast(o, I3.class, true );
	    } catch (ClassCastException e) {
		reportCast(o,I3.class, false );
	    }
	reportInstanceof(o, Cloneable.class, (o instanceof Cloneable) );
	try
	    {
		Cloneable r1 = (Cloneable) o;
		reportCast(o, Cloneable.class, true );
	    } catch (ClassCastException e) {
		reportCast(o,Cloneable.class, false );
	    }

	reportInstanceof(o, Serializable.class, (o instanceof Serializable) );
	try
	    {
		Serializable r1 = (Serializable) o;
		reportCast(o, Serializable.class, true );
	    } catch (ClassCastException e) {
		reportCast(o,Serializable.class, false );
	    }
	reportInstanceof(o, A1[].class, (o instanceof A1[]) );
	try
	    {
		A1[] r1 = (A1[]) o;
		reportCast(o, A1[].class, true );
	    } catch (ClassCastException e) {
		reportCast(o,A1[].class, false );
	    }

	reportInstanceof(o, AB12[].class, (o instanceof AB12[]) );
	try
	    {
		AB12[] r1 = (AB12[]) o;
		reportCast(o, AB12[].class, true );
	    } catch (ClassCastException e) {
		reportCast(o,AB12[].class, false );
	    }
	reportInstanceof(o, ABC12[].class, (o instanceof ABC12[]) );
	try
	    {
		ABC12[] r1 = (ABC12[]) o;
		reportCast(o, ABC12[].class, true );
	    } catch (ClassCastException e) {
		reportCast(o,ABC12[].class, false );
	    }
	reportInstanceof(o, D3[].class, (o instanceof D3[]) );
	try
	    {
		D3[] r1 = (D3[]) o;
		reportCast(o, D3[].class, true );
	    } catch (ClassCastException e) {
		reportCast(o,D3[].class, false );
	    }
	reportInstanceof(o, I1[].class, (o instanceof I1[]) );
	try
	    {
		I1[] r1 = (I1[]) o;
		reportCast(o, I1[].class, true );
	    } catch (ClassCastException e) {
		reportCast(o,I1[].class, false );
	    }
	reportInstanceof(o, I2[].class, (o instanceof I2[]) );
	try
	    {
		I2[] r1 = (I2[]) o;
		reportCast(o, I2[].class, true );
	    } catch (ClassCastException e) {
		reportCast(o,I2[].class, false );
	    }

	reportInstanceof(o, I3[].class, (o instanceof I3[]) );
	try
	    {
		I3[] r1 = (I3[]) o;
		reportCast(o, I3[].class, true );
	    } catch (ClassCastException e) {
		reportCast(o,I3[].class, false );
	    }

	reportInstanceof(o, intArray, (o instanceof int[]) );
	try
	    {
		int[] r1 = (int[]) o;
		reportCast(o, intArray, true );
	    } catch (ClassCastException e) {
		reportCast(o,intArray, false );
	    }

	reportInstanceof(o, A1[][].class, (o instanceof A1[][]) );
	try
	    {
		A1[][] r1 = (A1[][]) o;
		reportCast(o, A1[][].class, true );
	    } catch (ClassCastException e) {
		reportCast(o,A1[][].class, false );
	    }
	reportInstanceof(o, AB12[][].class, (o instanceof AB12[][]) );
	try
	    {
		AB12[][] r1 = (AB12[][]) o;
		reportCast(o, AB12[][].class, true );
	    } catch (ClassCastException e) {
		reportCast(o,AB12[][].class, false );
	    }
	reportInstanceof(o, I1[][].class, (o instanceof I1[][]) );
	try
	    {
		I1[][] r1 = (I1[][]) o;
		reportCast(o, I1[][].class, true );
	    } catch (ClassCastException e) {
		reportCast(o,I1[][].class, false );
	    }

    }

}

// javac flags: -nowarn
/* Expected Output:
ffffffffffffffffffffftffffffffffffffffffffttffftffffffffffffffftttffttffffffffffffffttttfttfffffffffffffftffftfttffffffffffffftffffffftttffftfffffftfffffffttttffttffffftffffffftttttfttffffftfffffffttffftfttfffftfffffffttfffftfffffftfffffffttffffftffffftfffffffttfffffttfffftfffffffttffffffftffftfffffffttfffffffftfttfffffffttffffffffttttfffffffttfffffffffftftftftftftftftftftftftftftftftftftftftftftttffffffffffffffffffffffffffffffffffffffffttttffffffttffffffffffffffffffffffffffffffttttttffffttttffffffffffffffffffffffffffffttttttttffttttffffffffffffffffffffffffffffttffffffttffttttffffffffffffffffffffffffffttffffffffffffffttttttffffffttffffffffffffttffffffffffffffttttttttffffttttffffffffffttffffffffffffffttttttttttffttttffffffffffttffffffffffffffttttffffffttffttttffffffffttffffffffffffffttttffffffffttffffffffffffttffffffffffffffttttffffffffffttffffffffffttffffffffffffffttttffffffffffttttffffffffttffffffffffffffttttffffffffffffffttffffffttffffffffffffffttttffffffffffffffffttffttttffffffffffffffttttffffffffffffffffttttttttffffffffffffffttttfffffffffffffffffffftttttttttttttttttttttttftttfffffffffffffffffffttfffffffffffffffffffftffffffffffffffffffffftffffffffffffffffftttftffffffffffffffffftttfttffffffffffffffffftfftffffffffffffffffffffftftttttttttttfffffffffttttttttttttfffffffffftttfffffffffffffffffffttfffffffffffffffffffftffffffffffffffffffffftffffffffffffffffftttftffffffffffffffffftttfttffffffffffffffffftfftffffffffffffffffffffftfffffffffffffffffffffttfffffffffffffffffffftfffffffffffffffffffttt
*/
