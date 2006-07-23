/* RMIC.java --
   Copyright (c) 1996, 1997, 1998, 1999, 2001, 2002, 2003, 2004, 2005
   Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA. */

package gnu.classpath.tools.rmi.rmic;

import gnu.java.rmi.server.RMIHashes;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.ObjectInput;
import java.io.ObjectOutput;
import java.io.PrintWriter;
import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLClassLoader;
import java.rmi.MarshalException;
import java.rmi.Remote;
import java.rmi.RemoteException;
import java.rmi.UnexpectedException;
import java.rmi.UnmarshalException;
import java.rmi.server.Operation;
import java.rmi.server.RemoteCall;
import java.rmi.server.RemoteObject;
import java.rmi.server.RemoteRef;
import java.rmi.server.RemoteStub;
import java.rmi.server.Skeleton;
import java.rmi.server.SkeletonMismatchException;
import java.security.MessageDigest;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.StringTokenizer;
import org.objectweb.asm153.ClassVisitor;
import org.objectweb.asm153.ClassWriter;
import org.objectweb.asm153.CodeVisitor;
import org.objectweb.asm153.Constants;
import org.objectweb.asm153.Label;
import org.objectweb.asm153.Type;

public class RMIC
{
  private String[] args;
  private int next;
  private List errors = new ArrayList();
  private boolean keep = false;
  private boolean need11Stubs = true;
  private boolean need12Stubs = true;
  private boolean compile = true;
  private boolean verbose;
  private String destination;
  private String classpath;
  private ClassLoader loader;
  private int errorCount = 0;

  private Class clazz;
  private String classname;
  private String classInternalName;
  private String fullclassname;
  private MethodRef[] remotemethods;
  private String stubname;
  private String skelname;
  private List mRemoteInterfaces;

  private static class C
    implements Constants
  {
  }

  public RMIC(String[] a)
  {
    args = a;
  }

  public static void main(String[] args)
  {
    if (rmic(args))
      System.exit(0);
    else
      System.exit(1);
  }

  /**
   * @return true if compilation was successful
   */
  public static boolean rmic(String[] args)
  {
    RMIC r = new RMIC(args);
    return r.run();
  }

  /**
   * @return true if run was successful
   */
  private boolean run()
  {
    boolean done = parseOptions();
    if (done)
      return errorCount == 0;

    if (next >= args.length)
      {
        usage();
        return false;
      }

    for (int i = next; i < args.length; i++)
      {
	try
	  {
            if (verbose)
	      System.out.println("[Processing class " + args[i] + ".class]");
	    processClass(args[i].replace(File.separatorChar, '.'));
	  }
        catch (IOException e)
          {
            errors.add(e);
          }
        catch (RMICException e)
          {
            errors.add(e);
          }
      }
    if (errors.size() > 0)
      {
        for (Iterator it = errors.iterator(); it.hasNext(); )
          {
            Exception ex = (Exception) it.next();
            logError(ex);
          }
      }

    return errorCount == 0;
  }

  private void processClass(String cls) throws IOException, RMICException
  {
    // reset class specific vars
    clazz = null;
    classname = null;
    classInternalName = null;
    fullclassname = null;
    remotemethods = null;
    stubname = null;
    skelname = null;
    mRemoteInterfaces = new ArrayList();

    analyzeClass(cls);
    generateStub();
    if (need11Stubs)
      generateSkel();
  }

  private void analyzeClass(String cname)
    throws RMICException
  {
    if (verbose)
      System.out.println("[analyze class " + cname + "]");
    int p = cname.lastIndexOf('.');
    if (p != -1)
      classname = cname.substring(p + 1);
    else
      classname = cname;
    fullclassname = cname;

    findClass();
    findRemoteMethods();
  }

  /**
   * @deprecated
   */
  public Exception getException()
  {
    return errors.size() == 0 ? null : (Exception) errors.get(0);
  }

  private void findClass()
    throws RMICException
  {
    ClassLoader cl = (loader == null
                      ? ClassLoader.getSystemClassLoader()
                      : loader);
    try
      {
        clazz = Class.forName(fullclassname, false, cl);
      }
    catch (ClassNotFoundException cnfe)
      {
        throw new RMICException
          ("Class " + fullclassname + " not found in classpath", cnfe);
      }

    if (! Remote.class.isAssignableFrom(clazz))
      {
        throw new RMICException
          ("Class " + clazz.getName()
           + " does not implement a remote interface.");
      }
  }

  private static Type[] typeArray(Class[] cls)
  {
    Type[] t = new Type[cls.length];
    for (int i = 0; i < cls.length; i++)
      {
        t[i] = Type.getType(cls[i]);
      }

    return t;
  }

  private static String[] internalNameArray(Type[] t)
  {
    String[] s = new String[t.length];
    for (int i = 0; i < t.length; i++)
      {
        s[i] = t[i].getInternalName();
      }

    return s;
  }

  private static String[] internalNameArray(Class[] c)
  {
    return internalNameArray(typeArray(c));
  }

  private static final String forName = "class$";

  private static Object param(Method m, int argIndex)
  {
    List l = new ArrayList();
    l.add(m);
    l.add(new Integer(argIndex));
    return l;
  }

  private static void generateClassForNamer(ClassVisitor cls)
  {
    CodeVisitor cv =
      cls.visitMethod
      (C.ACC_PRIVATE + C.ACC_STATIC + C.ACC_SYNTHETIC, forName,
       Type.getMethodDescriptor
       (Type.getType(Class.class), new Type[] { Type.getType(String.class) }),
       null, null);

    Label start = new Label();
    cv.visitLabel(start);
    cv.visitVarInsn(C.ALOAD, 0);
    cv.visitMethodInsn
      (C.INVOKESTATIC,
       Type.getInternalName(Class.class),
       "forName",
       Type.getMethodDescriptor
       (Type.getType(Class.class), new Type[] { Type.getType(String.class) }));
    cv.visitInsn(C.ARETURN);

    Label handler = new Label();
    cv.visitLabel(handler);
    cv.visitVarInsn(C.ASTORE, 1);
    cv.visitTypeInsn(C.NEW, typeArg(NoClassDefFoundError.class));
    cv.visitInsn(C.DUP);
    cv.visitVarInsn(C.ALOAD, 1);
    cv.visitMethodInsn
      (C.INVOKEVIRTUAL,
       Type.getInternalName(ClassNotFoundException.class),
       "getMessage",
       Type.getMethodDescriptor(Type.getType(String.class), new Type[] {}));
    cv.visitMethodInsn
      (C.INVOKESPECIAL,
       Type.getInternalName(NoClassDefFoundError.class),
       "<init>",
       Type.getMethodDescriptor
       (Type.VOID_TYPE, new Type[] { Type.getType(String.class) }));
    cv.visitInsn(C.ATHROW);
    cv.visitTryCatchBlock
      (start, handler, handler,
       Type.getInternalName(ClassNotFoundException.class));
    cv.visitMaxs(-1, -1);
  }

  private void generateClassConstant(CodeVisitor cv, Class cls) {
    if (cls.isPrimitive())
      {
        Class boxCls;
        if (cls.equals(Boolean.TYPE))
          boxCls = Boolean.class;
        else if (cls.equals(Character.TYPE))
          boxCls = Character.class;
        else if (cls.equals(Byte.TYPE))
          boxCls = Byte.class;
        else if (cls.equals(Short.TYPE))
          boxCls = Short.class;
        else if (cls.equals(Integer.TYPE))
          boxCls = Integer.class;
        else if (cls.equals(Long.TYPE))
          boxCls = Long.class;
        else if (cls.equals(Float.TYPE))
          boxCls = Float.class;
        else if (cls.equals(Double.TYPE))
          boxCls = Double.class;
        else if (cls.equals(Void.TYPE))
          boxCls = Void.class;
        else
          throw new IllegalArgumentException("unknown primitive type " + cls);

        cv.visitFieldInsn
          (C.GETSTATIC, Type.getInternalName(boxCls), "TYPE",
           Type.getDescriptor(Class.class));
        return;
      }
    cv.visitLdcInsn(cls.getName());
    cv.visitMethodInsn
      (C.INVOKESTATIC, classInternalName, forName,
       Type.getMethodDescriptor
       (Type.getType(Class.class),
        new Type[] { Type.getType(String.class) }));
  }

  private void generateClassArray(CodeVisitor code, Class[] classes)
  {
    code.visitLdcInsn(new Integer(classes.length));
    code.visitTypeInsn(C.ANEWARRAY, typeArg(Class.class));
    for (int i = 0; i < classes.length; i++)
      {
        code.visitInsn(C.DUP);
        code.visitLdcInsn(new Integer(i));
        generateClassConstant(code, classes[i]);
        code.visitInsn(C.AASTORE);
      }
  }

  private void fillOperationArray(CodeVisitor clinit)
  {
    // Operations array
    clinit.visitLdcInsn(new Integer(remotemethods.length));
    clinit.visitTypeInsn(C.ANEWARRAY, typeArg(Operation.class));
    clinit.visitFieldInsn
      (C.PUTSTATIC, classInternalName, "operations",
       Type.getDescriptor(Operation[].class));

    for (int i = 0; i < remotemethods.length; i++)
      {
        Method m = remotemethods[i].meth;

        StringBuffer desc = new StringBuffer();
        desc.append(getPrettyName(m.getReturnType()) + " ");
        desc.append(m.getName() + "(");

        // signature
        Class[] sig = m.getParameterTypes();
        for (int j = 0; j < sig.length; j++)
          {
            desc.append(getPrettyName(sig[j]));
            if (j + 1 < sig.length)
                desc.append(", ");
          }

        // push operations array
        clinit.visitFieldInsn
          (C.GETSTATIC, classInternalName, "operations",
           Type.getDescriptor(Operation[].class));

        // push array index
        clinit.visitLdcInsn(new Integer(i));

        // instantiate operation and leave a copy on the stack
        clinit.visitTypeInsn(C.NEW, typeArg(Operation.class));
        clinit.visitInsn(C.DUP);
        clinit.visitLdcInsn(desc.toString());
        clinit.visitMethodInsn
          (C.INVOKESPECIAL,
           Type.getInternalName(Operation.class),
           "<init>",
           Type.getMethodDescriptor
           (Type.VOID_TYPE, new Type[] { Type.getType(String.class) }));

        // store in operations array
        clinit.visitInsn(C.AASTORE);
      }
  }

  private void generateStaticMethodObjs(CodeVisitor clinit)
  {
    for (int i = 0; i < remotemethods.length; i++)
      {
        Method m = remotemethods[i].meth;

        /*
         * $method_<i>m.getName()</i>_<i>i</i> =
         *   <i>m.getDeclaringClass()</i>.class.getMethod
         *     (m.getName(), m.getParameterType())
         */
        String methodVar = "$method_" + m.getName() + "_" + i;
        generateClassConstant(clinit, m.getDeclaringClass());
        clinit.visitLdcInsn(m.getName());
        generateClassArray(clinit, m.getParameterTypes());
        clinit.visitMethodInsn
          (C.INVOKEVIRTUAL,
           Type.getInternalName(Class.class),
           "getMethod",
           Type.getMethodDescriptor
           (Type.getType(Method.class),
            new Type[] { Type.getType(String.class),
                         Type.getType(Class[].class) }));

        clinit.visitFieldInsn
          (C.PUTSTATIC, classInternalName, methodVar,
           Type.getDescriptor(Method.class));
      }
  }

  private void generateStub()
    throws IOException
  {
    stubname = fullclassname + "_Stub";
    String stubclassname = classname + "_Stub";
    File file = new File((destination == null ? "." : destination)
                         + File.separator
                         + stubname.replace('.', File.separatorChar)
                         + ".class");

    if (verbose)
      System.out.println("[Generating class " + stubname + "]");

    final ClassWriter stub = new ClassWriter(true);
    classInternalName = stubname.replace('.', '/');
    final String superInternalName =
      Type.getType(RemoteStub.class).getInternalName();

    String[] remoteInternalNames =
      internalNameArray((Class[]) mRemoteInterfaces.toArray(new Class[] {}));
    stub.visit
      (C.V1_2, C.ACC_PUBLIC + C.ACC_FINAL, classInternalName,
       superInternalName, remoteInternalNames, null);

    if (need12Stubs)
      {
        stub.visitField
          (C.ACC_PRIVATE + C.ACC_STATIC + C.ACC_FINAL, "serialVersionUID",
           Type.LONG_TYPE.getDescriptor(), new Long(2L), null);
      }

    if (need11Stubs)
      {
        stub.visitField
          (C.ACC_PRIVATE + C.ACC_STATIC + C.ACC_FINAL,
           "interfaceHash", Type.LONG_TYPE.getDescriptor(),
           new Long(RMIHashes.getInterfaceHash(clazz)), null);

        if (need12Stubs)
          {
            stub.visitField
              (C.ACC_PRIVATE + C.ACC_STATIC, "useNewInvoke",
               Type.BOOLEAN_TYPE.getDescriptor(), null, null);
          }

        stub.visitField
          (C.ACC_PRIVATE + C.ACC_STATIC + C.ACC_FINAL,
           "operations", Type.getDescriptor(Operation[].class), null, null);
      }

    // Set of method references.
    if (need12Stubs)
      {
        for (int i = 0; i < remotemethods.length; i++)
          {
            Method m = remotemethods[i].meth;
            String slotName = "$method_" + m.getName() + "_" + i;
            stub.visitField
              (C.ACC_PRIVATE + C.ACC_STATIC, slotName,
               Type.getDescriptor(Method.class), null, null);
          }
      }

    CodeVisitor clinit = stub.visitMethod
      (C.ACC_STATIC, "<clinit>",
       Type.getMethodDescriptor(Type.VOID_TYPE, new Type[] {}), null, null);

    if (need11Stubs)
      {
        fillOperationArray(clinit);
        if (! need12Stubs)
          clinit.visitInsn(C.RETURN);
      }

    if (need12Stubs)
      {
        // begin of try
        Label begin = new Label();

        // beginning of catch
        Label handler = new Label();
        clinit.visitLabel(begin);

        // Initialize the methods references.
        if (need11Stubs)
          {
            /*
             * RemoteRef.class.getMethod("invoke", new Class[] {
             *   Remote.class, Method.class, Object[].class, long.class })
             */
            generateClassConstant(clinit, RemoteRef.class);
            clinit.visitLdcInsn("invoke");
            generateClassArray
              (clinit, new Class[] { Remote.class, Method.class,
                                     Object[].class, long.class });
            clinit.visitMethodInsn
              (C.INVOKEVIRTUAL,
               Type.getInternalName(Class.class),
               "getMethod",
               Type.getMethodDescriptor
               (Type.getType(Method.class),
                new Type[] { Type.getType(String.class),
                             Type.getType(Class[].class) }));

            // useNewInvoke = true
            clinit.visitInsn(C.ICONST_1);
            clinit.visitFieldInsn
              (C.PUTSTATIC, classInternalName, "useNewInvoke",
               Type.BOOLEAN_TYPE.getDescriptor());
          }

        generateStaticMethodObjs(clinit);

        // jump past handler
        clinit.visitInsn(C.RETURN);
        clinit.visitLabel(handler);
        if (need11Stubs)
          {
            // useNewInvoke = false
            clinit.visitInsn(C.ICONST_0);
            clinit.visitFieldInsn
              (C.PUTSTATIC, classInternalName, "useNewInvoke",
               Type.BOOLEAN_TYPE.getDescriptor());
            clinit.visitInsn(C.RETURN);
          }
        else
          {
            // throw NoSuchMethodError
            clinit.visitTypeInsn(C.NEW, typeArg(NoSuchMethodError.class));
            clinit.visitInsn(C.DUP);
            clinit.visitLdcInsn("stub class initialization failed");
            clinit.visitMethodInsn
              (C.INVOKESPECIAL,
               Type.getInternalName(NoSuchMethodError.class),
               "<init>",
               Type.getMethodDescriptor
               (Type.VOID_TYPE,
                new Type[] { Type.getType(String.class) }));
            clinit.visitInsn(C.ATHROW);
          }

        clinit.visitTryCatchBlock
          (begin, handler, handler,
           Type.getInternalName(NoSuchMethodException.class));

      }

    clinit.visitMaxs(-1, -1);

    generateClassForNamer(stub);

    // Constructors
    if (need11Stubs)
      {
        // no arg public constructor
        CodeVisitor code = stub.visitMethod
          (C.ACC_PUBLIC, "<init>",
           Type.getMethodDescriptor(Type.VOID_TYPE, new Type[] {}),
           null, null);
        code.visitVarInsn(C.ALOAD, 0);
        code.visitMethodInsn
          (C.INVOKESPECIAL, superInternalName, "<init>",
           Type.getMethodDescriptor(Type.VOID_TYPE, new Type[] {}));
        code.visitInsn(C.RETURN);

        code.visitMaxs(-1, -1);
      }

    // public RemoteRef constructor
    CodeVisitor constructor = stub.visitMethod
      (C.ACC_PUBLIC, "<init>",
       Type.getMethodDescriptor
       (Type.VOID_TYPE, new Type[] {Type.getType(RemoteRef.class)}),
       null, null);
    constructor.visitVarInsn(C.ALOAD, 0);
    constructor.visitVarInsn(C.ALOAD, 1);
    constructor.visitMethodInsn
      (C.INVOKESPECIAL, superInternalName, "<init>",
       Type.getMethodDescriptor
       (Type.VOID_TYPE, new Type[] {Type.getType(RemoteRef.class)}));
    constructor.visitInsn(C.RETURN);
    constructor.visitMaxs(-1, -1);

    // Method implementations
    for (int i = 0; i < remotemethods.length; i++)
      {
        Method m = remotemethods[i].meth;
        Class[] sig = m.getParameterTypes();
        Class returntype = m.getReturnType();
        Class[] except = sortExceptions
          ((Class[]) remotemethods[i].exceptions.toArray(new Class[0]));

        CodeVisitor code = stub.visitMethod
          (C.ACC_PUBLIC,
           m.getName(),
           Type.getMethodDescriptor(Type.getType(returntype), typeArray(sig)),
           internalNameArray(typeArray(except)),
           null);

        final Variables var = new Variables();

        // this and parameters are the declared vars
        var.declare("this");
        for (int j = 0; j < sig.length; j++)
          var.declare(param(m, j), size(sig[j]));

        Label methodTryBegin = new Label();
        code.visitLabel(methodTryBegin);

        if (need12Stubs)
          {
            Label oldInvoke = new Label();
            if (need11Stubs)
              {
                // if not useNewInvoke jump to old invoke
                code.visitFieldInsn
                  (C.GETSTATIC, classInternalName, "useNewInvoke",
                   Type.getDescriptor(boolean.class));
                code.visitJumpInsn(C.IFEQ, oldInvoke);
              }

            // this.ref
            code.visitVarInsn(C.ALOAD, var.get("this"));
            code.visitFieldInsn
              (C.GETFIELD, Type.getInternalName(RemoteObject.class),
               "ref", Type.getDescriptor(RemoteRef.class));

            // "this" is first arg to invoke
            code.visitVarInsn(C.ALOAD, var.get("this"));

            // method object is second arg to invoke
            String methName = "$method_" + m.getName() + "_" + i;
            code.visitFieldInsn
              (C.GETSTATIC, classInternalName, methName,
               Type.getDescriptor(Method.class));

            // args to remote method are third arg to invoke
            if (sig.length == 0)
              code.visitInsn(C.ACONST_NULL);
            else
              {
                // create arg Object[] (with boxed primitives) and push it
                code.visitLdcInsn(new Integer(sig.length));
                code.visitTypeInsn(C.ANEWARRAY, typeArg(Object.class));

                var.allocate("argArray");
                code.visitVarInsn(C.ASTORE, var.get("argArray"));

                for (int j = 0; j < sig.length; j++)
                  {
                    int size = size(sig[j]);
                    int insn = loadOpcode(sig[j]);
                    Class box = sig[j].isPrimitive() ? box(sig[j]) : null;

                    code.visitVarInsn(C.ALOAD, var.get("argArray"));
                    code.visitLdcInsn(new Integer(j));

                    // put argument on stack
                    if (box != null)
                      {
                        code.visitTypeInsn(C.NEW, typeArg(box));
                        code.visitInsn(C.DUP);
                        code.visitVarInsn(insn, var.get(param(m, j)));
                        code.visitMethodInsn
                          (C.INVOKESPECIAL,
                           Type.getInternalName(box),
                           "<init>",
                           Type.getMethodDescriptor
                           (Type.VOID_TYPE,
                            new Type[] { Type.getType(sig[j]) }));
                      }
                    else
                      code.visitVarInsn(insn, var.get(param(m, j)));

                    code.visitInsn(C.AASTORE);
                  }

                code.visitVarInsn(C.ALOAD, var.deallocate("argArray"));
              }

            // push remote operation opcode
            code.visitLdcInsn(new Long(remotemethods[i].hash));
            code.visitMethodInsn
              (C.INVOKEINTERFACE,
               Type.getInternalName(RemoteRef.class),
               "invoke",
               Type.getMethodDescriptor
               (Type.getType(Object.class),
                new Type[] { Type.getType(Remote.class),
                             Type.getType(Method.class),
                             Type.getType(Object[].class),
                             Type.LONG_TYPE }));

            if (! returntype.equals(Void.TYPE))
              {
                int retcode = returnOpcode(returntype);
                Class boxCls =
                  returntype.isPrimitive() ? box(returntype) : null;
                code.visitTypeInsn
                  (C.CHECKCAST, typeArg(boxCls == null ? returntype : boxCls));
                if (returntype.isPrimitive())
                  {
                    // unbox
                    code.visitMethodInsn
                      (C.INVOKEVIRTUAL,
                       Type.getType(boxCls).getInternalName(),
                       unboxMethod(returntype),
                       Type.getMethodDescriptor
                       (Type.getType(returntype), new Type[] {}));
                  }

                code.visitInsn(retcode);
              }
            else
              code.visitInsn(C.RETURN);


            if (need11Stubs)
              code.visitLabel(oldInvoke);
          }

        if (need11Stubs)
          {

            // this.ref.newCall(this, operations, index, interfaceHash)
            code.visitVarInsn(C.ALOAD, var.get("this"));
            code.visitFieldInsn
              (C.GETFIELD,
               Type.getInternalName(RemoteObject.class),
               "ref",
               Type.getDescriptor(RemoteRef.class));

            // "this" is first arg to newCall
            code.visitVarInsn(C.ALOAD, var.get("this"));

            // operations is second arg to newCall
            code.visitFieldInsn
              (C.GETSTATIC, classInternalName, "operations",
               Type.getDescriptor(Operation[].class));

            // method index is third arg
            code.visitLdcInsn(new Integer(i));

            // interface hash is fourth arg
            code.visitFieldInsn
              (C.GETSTATIC, classInternalName, "interfaceHash",
               Type.LONG_TYPE.getDescriptor());

            code.visitMethodInsn
              (C.INVOKEINTERFACE,
               Type.getInternalName(RemoteRef.class),
               "newCall",
               Type.getMethodDescriptor
               (Type.getType(RemoteCall.class),
                new Type[] { Type.getType(RemoteObject.class),
                             Type.getType(Operation[].class),
                             Type.INT_TYPE,
                             Type.LONG_TYPE }));

            // store call object on stack and leave copy on stack
            var.allocate("call");
            code.visitInsn(C.DUP);
            code.visitVarInsn(C.ASTORE, var.get("call"));

            Label beginArgumentTryBlock = new Label();
            code.visitLabel(beginArgumentTryBlock);

            // ObjectOutput out = call.getOutputStream();
            code.visitMethodInsn
              (C.INVOKEINTERFACE,
               Type.getInternalName(RemoteCall.class),
               "getOutputStream",
               Type.getMethodDescriptor
               (Type.getType(ObjectOutput.class), new Type[] {}));

            for (int j = 0; j < sig.length; j++)
              {
                // dup the ObjectOutput
                code.visitInsn(C.DUP);

                // get j'th arg to remote method
                code.visitVarInsn(loadOpcode(sig[j]), var.get(param(m, j)));

                Class argCls =
                  sig[j].isPrimitive() ? sig[j] : Object.class;

                // out.writeFoo
                code.visitMethodInsn
                  (C.INVOKEINTERFACE,
                   Type.getInternalName(ObjectOutput.class),
                   writeMethod(sig[j]),
                   Type.getMethodDescriptor
                   (Type.VOID_TYPE,
                    new Type[] { Type.getType(argCls) }));
              }

            // pop ObjectOutput
            code.visitInsn(C.POP);

            Label iohandler = new Label();
            Label endArgumentTryBlock = new Label();
            code.visitJumpInsn(C.GOTO, endArgumentTryBlock);
            code.visitLabel(iohandler);

            // throw new MarshalException(msg, ioexception);
            code.visitVarInsn(C.ASTORE, var.allocate("exception"));
            code.visitTypeInsn(C.NEW, typeArg(MarshalException.class));
            code.visitInsn(C.DUP);
            code.visitLdcInsn("error marshalling arguments");
            code.visitVarInsn(C.ALOAD, var.deallocate("exception"));
            code.visitMethodInsn
              (C.INVOKESPECIAL,
               Type.getInternalName(MarshalException.class),
               "<init>",
               Type.getMethodDescriptor
               (Type.VOID_TYPE,
                new Type[] { Type.getType(String.class),
                             Type.getType(Exception.class) }));
            code.visitInsn(C.ATHROW);

            code.visitLabel(endArgumentTryBlock);
            code.visitTryCatchBlock
              (beginArgumentTryBlock, iohandler, iohandler,
               Type.getInternalName(IOException.class));

            // this.ref.invoke(call)
            code.visitVarInsn(C.ALOAD, var.get("this"));
            code.visitFieldInsn
              (C.GETFIELD, Type.getInternalName(RemoteObject.class),
               "ref", Type.getDescriptor(RemoteRef.class));
            code.visitVarInsn(C.ALOAD, var.get("call"));
            code.visitMethodInsn
              (C.INVOKEINTERFACE,
               Type.getInternalName(RemoteRef.class),
               "invoke",
               Type.getMethodDescriptor
               (Type.VOID_TYPE,
                new Type[] { Type.getType(RemoteCall.class) }));

            // handle return value
            boolean needcastcheck = false;

            Label beginReturnTryCatch = new Label();
            code.visitLabel(beginReturnTryCatch);

            int returncode = returnOpcode(returntype);

            if (! returntype.equals(Void.TYPE))
              {
                // call.getInputStream()
                code.visitVarInsn(C.ALOAD, var.get("call"));
                code.visitMethodInsn
                  (C.INVOKEINTERFACE,
                   Type.getInternalName(RemoteCall.class),
                   "getInputStream",
                   Type.getMethodDescriptor
                   (Type.getType(ObjectInput.class), new Type[] {}));

                Class readCls =
                  returntype.isPrimitive() ? returntype : Object.class;
                code.visitMethodInsn
                  (C.INVOKEINTERFACE,
                   Type.getInternalName(ObjectInput.class),
                   readMethod(returntype),
                   Type.getMethodDescriptor
                   (Type.getType(readCls), new Type[] {}));

                boolean castresult = false;

                if (! returntype.isPrimitive())
                  {
                    if (! returntype.equals(Object.class))
                      castresult = true;
                    else
                      needcastcheck = true;
                  }

                if (castresult)
                  code.visitTypeInsn(C.CHECKCAST, typeArg(returntype));

                // leave result on stack for return
              }

            // this.ref.done(call)
            code.visitVarInsn(C.ALOAD, var.get("this"));
            code.visitFieldInsn
              (C.GETFIELD,
               Type.getInternalName(RemoteObject.class),
               "ref",
               Type.getDescriptor(RemoteRef.class));
            code.visitVarInsn(C.ALOAD, var.deallocate("call"));
            code.visitMethodInsn
              (C.INVOKEINTERFACE,
               Type.getInternalName(RemoteRef.class),
               "done",
               Type.getMethodDescriptor
               (Type.VOID_TYPE,
                new Type[] { Type.getType(RemoteCall.class) }));

            // return; or return result;
            code.visitInsn(returncode);

            // exception handler
            Label handler = new Label();
            code.visitLabel(handler);
            code.visitVarInsn(C.ASTORE, var.allocate("exception"));

            // throw new UnmarshalException(msg, e)
            code.visitTypeInsn(C.NEW, typeArg(UnmarshalException.class));
            code.visitInsn(C.DUP);
            code.visitLdcInsn("error unmarshalling return");
            code.visitVarInsn(C.ALOAD, var.deallocate("exception"));
            code.visitMethodInsn
              (C.INVOKESPECIAL,
               Type.getInternalName(UnmarshalException.class),
               "<init>",
               Type.getMethodDescriptor
               (Type.VOID_TYPE,
                new Type[] { Type.getType(String.class),
                             Type.getType(Exception.class) }));
            code.visitInsn(C.ATHROW);

            Label endReturnTryCatch = new Label();

            // catch IOException
            code.visitTryCatchBlock
              (beginReturnTryCatch, handler, handler,
               Type.getInternalName(IOException.class));

            if (needcastcheck)
              {
                // catch ClassNotFoundException
                code.visitTryCatchBlock
                  (beginReturnTryCatch, handler, handler,
                   Type.getInternalName(ClassNotFoundException.class));
              }
          }

        Label rethrowHandler = new Label();
        code.visitLabel(rethrowHandler);
        // rethrow declared exceptions
        code.visitInsn(C.ATHROW);

        boolean needgeneral = true;
        for (int j = 0; j < except.length; j++)
          {
            if (except[j] == Exception.class)
              needgeneral = false;
          }

        for (int j = 0; j < except.length; j++)
          {
            code.visitTryCatchBlock
              (methodTryBegin, rethrowHandler, rethrowHandler,
               Type.getInternalName(except[j]));
          }

        if (needgeneral)
          {
            // rethrow unchecked exceptions
            code.visitTryCatchBlock
              (methodTryBegin, rethrowHandler, rethrowHandler,
               Type.getInternalName(RuntimeException.class));

            Label generalHandler = new Label();
            code.visitLabel(generalHandler);
            String msg = "undeclared checked exception";

            // throw new java.rmi.UnexpectedException(msg, e)
            code.visitVarInsn(C.ASTORE, var.allocate("exception"));
            code.visitTypeInsn(C.NEW, typeArg(UnexpectedException.class));
            code.visitInsn(C.DUP);
            code.visitLdcInsn(msg);
            code.visitVarInsn(C.ALOAD, var.deallocate("exception"));
            code.visitMethodInsn
              (C.INVOKESPECIAL,
               Type.getInternalName(UnexpectedException.class),
               "<init>",
               Type.getMethodDescriptor
               (Type.VOID_TYPE,
                new Type [] { Type.getType(String.class),
                              Type.getType(Exception.class) }));
            code.visitInsn(C.ATHROW);

            code.visitTryCatchBlock
              (methodTryBegin, rethrowHandler, generalHandler,
               Type.getInternalName(Exception.class));
          }

        code.visitMaxs(-1, -1);
      }

    stub.visitEnd();
    byte[] classData = stub.toByteArray();
    if (file.exists())
      file.delete();
    if (file.getParentFile() != null)
      file.getParentFile().mkdirs();
    FileOutputStream fos = new FileOutputStream(file);
    fos.write(classData);
    fos.flush();
    fos.close();
  }

  private void generateSkel() throws IOException
  {
    skelname = fullclassname + "_Skel";
    String skelclassname = classname + "_Skel";
    File file = new File(destination == null ? "" : destination
                         + File.separator
                         + skelname.replace('.', File.separatorChar)
                         + ".class");
    if (verbose)
      System.out.println("[Generating class " + skelname + "]");

    final ClassWriter skel = new ClassWriter(true);
    classInternalName = skelname.replace('.', '/');
    skel.visit
      (C.V1_1, C.ACC_PUBLIC + C.ACC_FINAL,
       classInternalName, Type.getInternalName(Object.class),
       new String[] { Type.getType(Skeleton.class).getInternalName() }, null);

    skel.visitField
      (C.ACC_PRIVATE + C.ACC_STATIC + C.ACC_FINAL, "interfaceHash",
       Type.LONG_TYPE.getDescriptor(),
       new Long(RMIHashes.getInterfaceHash(clazz)),
       null);

    skel.visitField
      (C.ACC_PRIVATE + C.ACC_STATIC + C.ACC_FINAL, "operations",
       Type.getDescriptor(Operation[].class), null, null);

    CodeVisitor clinit = skel.visitMethod
      (C.ACC_STATIC, "<clinit>",
       Type.getMethodDescriptor(Type.VOID_TYPE, new Type[] {}), null, null);

    fillOperationArray(clinit);
    clinit.visitInsn(C.RETURN);

    clinit.visitMaxs(-1, -1);

    // no arg public constructor
    CodeVisitor init = skel.visitMethod
      (C.ACC_PUBLIC, "<init>",
       Type.getMethodDescriptor(Type.VOID_TYPE, new Type[] {}), null, null);
    init.visitVarInsn(C.ALOAD, 0);
    init.visitMethodInsn
      (C.INVOKESPECIAL, Type.getInternalName(Object.class), "<init>",
       Type.getMethodDescriptor(Type.VOID_TYPE, new Type[] {}));
    init.visitInsn(C.RETURN);
    init.visitMaxs(-1, -1);

    /*
     * public Operation[] getOperations()
     * returns a clone of the operations array
     */
    CodeVisitor getOp = skel.visitMethod
      (C.ACC_PUBLIC, "getOperations",
       Type.getMethodDescriptor
       (Type.getType(Operation[].class), new Type[] {}),
       null, null);
    getOp.visitFieldInsn
      (C.GETSTATIC, classInternalName, "operations",
       Type.getDescriptor(Operation[].class));
    getOp.visitMethodInsn
      (C.INVOKEVIRTUAL, Type.getInternalName(Object.class),
       "clone", Type.getMethodDescriptor(Type.getType(Object.class),
                                         new Type[] {}));
    getOp.visitTypeInsn(C.CHECKCAST, typeArg(Operation[].class));
    getOp.visitInsn(C.ARETURN);
    getOp.visitMaxs(-1, -1);

    // public void dispatch(Remote, RemoteCall, int opnum, long hash)
    CodeVisitor dispatch = skel.visitMethod
      (C.ACC_PUBLIC,
       "dispatch",
       Type.getMethodDescriptor
       (Type.VOID_TYPE,
        new Type[] { Type.getType(Remote.class),
                     Type.getType(RemoteCall.class),
                     Type.INT_TYPE, Type.LONG_TYPE }),
       new String[] { Type.getInternalName(Exception.class) },
       null);

    Variables var = new Variables();
    var.declare("this");
    var.declare("remoteobj");
    var.declare("remotecall");
    var.declare("opnum");
    var.declareWide("hash");

    /*
     * if opnum >= 0
     * XXX it is unclear why there is handling of negative opnums
     */
    dispatch.visitVarInsn(C.ILOAD, var.get("opnum"));
    Label nonNegativeOpnum = new Label();
    Label opnumSet = new Label();
    dispatch.visitJumpInsn(C.IFGE, nonNegativeOpnum);

    for (int i = 0; i < remotemethods.length; i++)
      {
        // assign opnum if hash matches supplied hash
        dispatch.visitVarInsn(C.LLOAD, var.get("hash"));
        dispatch.visitLdcInsn(new Long(remotemethods[i].hash));
        Label notIt = new Label();
        dispatch.visitInsn(C.LCMP);
        dispatch.visitJumpInsn(C.IFNE, notIt);

        // opnum = <opnum>
        dispatch.visitLdcInsn(new Integer(i));
        dispatch.visitVarInsn(C.ISTORE, var.get("opnum"));
        dispatch.visitJumpInsn(C.GOTO, opnumSet);
        dispatch.visitLabel(notIt);
      }

    // throw new SkeletonMismatchException
    Label mismatch = new Label();
    dispatch.visitJumpInsn(C.GOTO, mismatch);

    dispatch.visitLabel(nonNegativeOpnum);

    // if opnum is already set, check that the hash matches the interface
    dispatch.visitVarInsn(C.LLOAD, var.get("hash"));
    dispatch.visitFieldInsn
      (C.GETSTATIC, classInternalName,
       "interfaceHash", Type.LONG_TYPE.getDescriptor());
    dispatch.visitInsn(C.LCMP);
    dispatch.visitJumpInsn(C.IFEQ, opnumSet);

    dispatch.visitLabel(mismatch);
    dispatch.visitTypeInsn
      (C.NEW, typeArg(SkeletonMismatchException.class));
    dispatch.visitInsn(C.DUP);
    dispatch.visitLdcInsn("interface hash mismatch");
    dispatch.visitMethodInsn
      (C.INVOKESPECIAL,
       Type.getInternalName(SkeletonMismatchException.class),
       "<init>",
       Type.getMethodDescriptor
       (Type.VOID_TYPE, new Type[] { Type.getType(String.class) }));
    dispatch.visitInsn(C.ATHROW);

    // opnum has been set
    dispatch.visitLabel(opnumSet);

    dispatch.visitVarInsn(C.ALOAD, var.get("remoteobj"));
    dispatch.visitTypeInsn(C.CHECKCAST, typeArg(clazz));
    dispatch.visitVarInsn(C.ASTORE, var.get("remoteobj"));

    Label deflt = new Label();
    Label[] methLabels = new Label[remotemethods.length];
    for (int i = 0; i < methLabels.length; i++)
      methLabels[i] = new Label();

    // switch on opnum
    dispatch.visitVarInsn(C.ILOAD, var.get("opnum"));
    dispatch.visitTableSwitchInsn
      (0, remotemethods.length - 1, deflt, methLabels);

    // Method dispatch
    for (int i = 0; i < remotemethods.length; i++)
      {
        dispatch.visitLabel(methLabels[i]);
        Method m = remotemethods[i].meth;
        generateMethodSkel(dispatch, m, var);
      }

    dispatch.visitLabel(deflt);
    dispatch.visitTypeInsn(C.NEW, typeArg(UnmarshalException.class));
    dispatch.visitInsn(C.DUP);
    dispatch.visitLdcInsn("invalid method number");
    dispatch.visitMethodInsn
      (C.INVOKESPECIAL,
       Type.getInternalName(UnmarshalException.class),
       "<init>",
       Type.getMethodDescriptor
       (Type.VOID_TYPE, new Type[] { Type.getType(String.class) }));
    dispatch.visitInsn(C.ATHROW);

    dispatch.visitMaxs(-1, -1);

    skel.visitEnd();
    byte[] classData = skel.toByteArray();
    if (file.exists())
      file.delete();
    if (file.getParentFile() != null)
      file.getParentFile().mkdirs();
    FileOutputStream fos = new FileOutputStream(file);
    fos.write(classData);
    fos.flush();
    fos.close();
  }

  private void generateMethodSkel(CodeVisitor cv, Method m, Variables var)
  {
    Class[] sig = m.getParameterTypes();

    Label readArgs = new Label();
    cv.visitLabel(readArgs);

    boolean needcastcheck = false;

    // ObjectInput in = call.getInputStream();
    cv.visitVarInsn(C.ALOAD, var.get("remotecall"));
    cv.visitMethodInsn
      (C.INVOKEINTERFACE,
       Type.getInternalName(RemoteCall.class), "getInputStream",
       Type.getMethodDescriptor
       (Type.getType(ObjectInput.class), new Type[] {}));
    cv.visitVarInsn(C.ASTORE, var.allocate("objectinput"));

    for (int i = 0; i < sig.length; i++)
      {
        // dup input stream
        cv.visitVarInsn(C.ALOAD, var.get("objectinput"));

        Class readCls = sig[i].isPrimitive() ? sig[i] : Object.class;

        // in.readFoo()
        cv.visitMethodInsn
          (C.INVOKEINTERFACE,
           Type.getInternalName(ObjectInput.class),
           readMethod(sig[i]),
           Type.getMethodDescriptor
           (Type.getType(readCls), new Type [] {}));

        if (! sig[i].isPrimitive() && ! sig[i].equals(Object.class))
          {
            needcastcheck = true;
            cv.visitTypeInsn(C.CHECKCAST, typeArg(sig[i]));
          }

        // store arg in variable
        cv.visitVarInsn
          (storeOpcode(sig[i]), var.allocate(param(m, i), size(sig[i])));
      }

    var.deallocate("objectinput");

    Label doCall = new Label();
    Label closeInput = new Label();

    cv.visitJumpInsn(C.JSR, closeInput);
    cv.visitJumpInsn(C.GOTO, doCall);

    // throw new UnmarshalException
    Label handler = new Label();
    cv.visitLabel(handler);
    cv.visitVarInsn(C.ASTORE, var.allocate("exception"));
    cv.visitTypeInsn(C.NEW, typeArg(UnmarshalException.class));
    cv.visitInsn(C.DUP);
    cv.visitLdcInsn("error unmarshalling arguments");
    cv.visitVarInsn(C.ALOAD, var.deallocate("exception"));
    cv.visitMethodInsn
      (C.INVOKESPECIAL,
       Type.getInternalName(UnmarshalException.class),
       "<init>",
       Type.getMethodDescriptor
       (Type.VOID_TYPE, new Type[] { Type.getType(String.class),
                                     Type.getType(Exception.class) }));
    cv.visitVarInsn(C.ASTORE, var.allocate("toThrow"));
    cv.visitJumpInsn(C.JSR, closeInput);
    cv.visitVarInsn(C.ALOAD, var.get("toThrow"));
    cv.visitInsn(C.ATHROW);

    cv.visitTryCatchBlock
      (readArgs, handler, handler, Type.getInternalName(IOException.class));
    if (needcastcheck)
      {
        cv.visitTryCatchBlock
          (readArgs, handler, handler,
           Type.getInternalName(ClassCastException.class));
      }

    // finally block
    cv.visitLabel(closeInput);
    cv.visitVarInsn(C.ASTORE, var.allocate("retAddress"));
    cv.visitVarInsn(C.ALOAD, var.get("remotecall"));
    cv.visitMethodInsn
      (C.INVOKEINTERFACE,
       Type.getInternalName(RemoteCall.class),
       "releaseInputStream",
       Type.getMethodDescriptor(Type.VOID_TYPE, new Type[] {}));
    cv.visitVarInsn(C.RET, var.deallocate("retAddress"));
    var.deallocate("toThrow");

    // do the call using args stored as variables
    cv.visitLabel(doCall);
    cv.visitVarInsn(C.ALOAD, var.get("remoteobj"));
    for (int i = 0; i < sig.length; i++)
      cv.visitVarInsn(loadOpcode(sig[i]), var.deallocate(param(m, i)));
    cv.visitMethodInsn
      (C.INVOKEVIRTUAL, Type.getInternalName(clazz), m.getName(),
       Type.getMethodDescriptor(m));

    Class returntype = m.getReturnType();
    if (! returntype.equals(Void.TYPE))
      {
        cv.visitVarInsn
          (storeOpcode(returntype), var.allocate("result", size(returntype)));
      }

    // write result to result stream
    Label writeResult = new Label();
    cv.visitLabel(writeResult);
    cv.visitVarInsn(C.ALOAD, var.get("remotecall"));
    cv.visitInsn(C.ICONST_1);
    cv.visitMethodInsn
      (C.INVOKEINTERFACE,
       Type.getInternalName(RemoteCall.class),
       "getResultStream",
       Type.getMethodDescriptor
       (Type.getType(ObjectOutput.class),
        new Type[] { Type.BOOLEAN_TYPE }));

    if (! returntype.equals(Void.TYPE))
      {
        // out.writeFoo(result)
        cv.visitVarInsn(loadOpcode(returntype), var.deallocate("result"));
        Class writeCls = returntype.isPrimitive() ? returntype : Object.class;
        cv.visitMethodInsn
          (C.INVOKEINTERFACE,
           Type.getInternalName(ObjectOutput.class),
           writeMethod(returntype),
           Type.getMethodDescriptor
           (Type.VOID_TYPE, new Type[] { Type.getType(writeCls) }));
      }

    cv.visitInsn(C.RETURN);

    // throw new MarshalException
    Label marshalHandler = new Label();
    cv.visitLabel(marshalHandler);
    cv.visitVarInsn(C.ASTORE, var.allocate("exception"));
    cv.visitTypeInsn(C.NEW, typeArg(MarshalException.class));
    cv.visitInsn(C.DUP);
    cv.visitLdcInsn("error marshalling return");
    cv.visitVarInsn(C.ALOAD, var.deallocate("exception"));
    cv.visitMethodInsn
      (C.INVOKESPECIAL,
       Type.getInternalName(MarshalException.class),
       "<init>",
       Type.getMethodDescriptor
       (Type.VOID_TYPE, new Type[] { Type.getType(String.class),
                                     Type.getType(Exception.class) }));
    cv.visitInsn(C.ATHROW);
    cv.visitTryCatchBlock
      (writeResult, marshalHandler, marshalHandler,
       Type.getInternalName(IOException.class));
  }

  private static String typeArg(Class cls)
  {
    if (cls.isArray())
      return Type.getDescriptor(cls);

    return Type.getInternalName(cls);
  }

  private static String readMethod(Class cls)
  {
    if (cls.equals(Void.TYPE))
      throw new IllegalArgumentException("can not read void");

    String method;
    if (cls.equals(Boolean.TYPE))
      method = "readBoolean";
    else if (cls.equals(Byte.TYPE))
      method = "readByte";
    else if (cls.equals(Character.TYPE))
      method = "readChar";
    else if (cls.equals(Short.TYPE))
      method = "readShort";
    else if (cls.equals(Integer.TYPE))
      method = "readInt";
    else if (cls.equals(Long.TYPE))
      method = "readLong";
    else if (cls.equals(Float.TYPE))
      method = "readFloat";
    else if (cls.equals(Double.TYPE))
      method = "readDouble";
    else
      method = "readObject";

    return method;
  }

  private static String writeMethod(Class cls)
  {
    if (cls.equals(Void.TYPE))
      throw new IllegalArgumentException("can not read void");

    String method;
    if (cls.equals(Boolean.TYPE))
      method = "writeBoolean";
    else if (cls.equals(Byte.TYPE))
      method = "writeByte";
    else if (cls.equals(Character.TYPE))
      method = "writeChar";
    else if (cls.equals(Short.TYPE))
      method = "writeShort";
    else if (cls.equals(Integer.TYPE))
      method = "writeInt";
    else if (cls.equals(Long.TYPE))
      method = "writeLong";
    else if (cls.equals(Float.TYPE))
      method = "writeFloat";
    else if (cls.equals(Double.TYPE))
      method = "writeDouble";
    else
      method = "writeObject";

    return method;
  }

  private static int returnOpcode(Class cls)
  {
    int returncode;
    if (cls.equals(Boolean.TYPE))
      returncode = C.IRETURN;
    else if (cls.equals(Byte.TYPE))
      returncode = C.IRETURN;
    else if (cls.equals(Character.TYPE))
      returncode = C.IRETURN;
    else if (cls.equals(Short.TYPE))
      returncode = C.IRETURN;
    else if (cls.equals(Integer.TYPE))
      returncode = C.IRETURN;
    else if (cls.equals(Long.TYPE))
      returncode = C.LRETURN;
    else if (cls.equals(Float.TYPE))
      returncode = C.FRETURN;
    else if (cls.equals(Double.TYPE))
      returncode = C.DRETURN;
    else if (cls.equals(Void.TYPE))
      returncode = C.RETURN;
    else
      returncode = C.ARETURN;

    return returncode;
  }

  private static int loadOpcode(Class cls)
  {
    if (cls.equals(Void.TYPE))
      throw new IllegalArgumentException("can not load void");

    int loadcode;
    if (cls.equals(Boolean.TYPE))
      loadcode = C.ILOAD;
    else if (cls.equals(Byte.TYPE))
      loadcode = C.ILOAD;
    else if (cls.equals(Character.TYPE))
      loadcode = C.ILOAD;
    else if (cls.equals(Short.TYPE))
      loadcode = C.ILOAD;
    else if (cls.equals(Integer.TYPE))
      loadcode = C.ILOAD;
    else if (cls.equals(Long.TYPE))
      loadcode = C.LLOAD;
    else if (cls.equals(Float.TYPE))
      loadcode = C.FLOAD;
    else if (cls.equals(Double.TYPE))
      loadcode = C.DLOAD;
    else
      loadcode = C.ALOAD;

    return loadcode;
  }

  private static int storeOpcode(Class cls)
  {
    if (cls.equals(Void.TYPE))
      throw new IllegalArgumentException("can not load void");

    int storecode;
    if (cls.equals(Boolean.TYPE))
      storecode = C.ISTORE;
    else if (cls.equals(Byte.TYPE))
      storecode = C.ISTORE;
    else if (cls.equals(Character.TYPE))
      storecode = C.ISTORE;
    else if (cls.equals(Short.TYPE))
      storecode = C.ISTORE;
    else if (cls.equals(Integer.TYPE))
      storecode = C.ISTORE;
    else if (cls.equals(Long.TYPE))
      storecode = C.LSTORE;
    else if (cls.equals(Float.TYPE))
      storecode = C.FSTORE;
    else if (cls.equals(Double.TYPE))
      storecode = C.DSTORE;
    else
      storecode = C.ASTORE;

    return storecode;
  }

  private static String unboxMethod(Class primitive)
  {
    if (! primitive.isPrimitive())
      throw new IllegalArgumentException("can not unbox nonprimitive");

    String method;
    if (primitive.equals(Boolean.TYPE))
      method = "booleanValue";
    else if (primitive.equals(Byte.TYPE))
      method = "byteValue";
    else if (primitive.equals(Character.TYPE))
      method = "charValue";
    else if (primitive.equals(Short.TYPE))
      method = "shortValue";
    else if (primitive.equals(Integer.TYPE))
      method = "intValue";
    else if (primitive.equals(Long.TYPE))
      method = "longValue";
    else if (primitive.equals(Float.TYPE))
      method = "floatValue";
    else if (primitive.equals(Double.TYPE))
      method = "doubleValue";
    else
      throw new IllegalStateException("unknown primitive class " + primitive);

    return method;
  }

  public static Class box(Class cls)
  {
    if (! cls.isPrimitive())
      throw new IllegalArgumentException("can only box primitive");

    Class box;
    if (cls.equals(Boolean.TYPE))
      box = Boolean.class;
    else if (cls.equals(Byte.TYPE))
      box = Byte.class;
    else if (cls.equals(Character.TYPE))
      box = Character.class;
    else if (cls.equals(Short.TYPE))
      box = Short.class;
    else if (cls.equals(Integer.TYPE))
      box = Integer.class;
    else if (cls.equals(Long.TYPE))
      box = Long.class;
    else if (cls.equals(Float.TYPE))
      box = Float.class;
    else if (cls.equals(Double.TYPE))
      box = Double.class;
    else
      throw new IllegalStateException("unknown primitive type " + cls);

    return box;
  }

  private static int size(Class cls) {
    if (cls.equals(Long.TYPE) || cls.equals(Double.TYPE))
      return 2;
    else
      return 1;
  }

  /**
   * Sort exceptions so the most general go last.
   */
  private Class[] sortExceptions(Class[] except)
  {
    for (int i = 0; i < except.length; i++)
      {
	for (int j = i + 1; j < except.length; j++)
	  {
	    if (except[i].isAssignableFrom(except[j]))
	      {
		Class tmp = except[i];
		except[i] = except[j];
		except[j] = tmp;
	      }
	  }
      }
    return (except);
  }

  /**
   * Process the options until we find the first argument.
   *
   * @return true if further processing should stop
   */
  private boolean parseOptions()
  {
    for (;;)
      {
	if (next >= args.length || args[next].charAt(0) != '-')
	  break;
	String arg = args[next];
	next++;

	// Accept `--' options if they look long enough.
	if (arg.length() > 3 && arg.charAt(0) == '-' && arg.charAt(1) == '-')
	  arg = arg.substring(1);

	if (arg.equals("-keep"))
	  keep = true;
	else if (arg.equals("-keepgenerated"))
	  keep = true;
	else if (arg.equals("-v1.1"))
	  {
	    need11Stubs = true;
	    need12Stubs = false;
	  }
	else if (arg.equals("-vcompat"))
	  {
	    need11Stubs = true;
	    need12Stubs = true;
	  }
	else if (arg.equals("-v1.2"))
	  {
	    need11Stubs = false;
	    need12Stubs = true;
	  }
	else if (arg.equals("-g"))
	  {
	  }
	else if (arg.equals("-depend"))
	  {
	  }
	else if (arg.equals("-nowarn"))
	  {
	  }
	else if (arg.equals("-verbose"))
	  verbose = true;
	else if (arg.equals("-nocompile"))
	  compile = false;
	else if (arg.equals("-classpath"))
          {
            classpath = args[next];
            next++;
            StringTokenizer st =
              new StringTokenizer(classpath, File.pathSeparator);
            URL[] u = new URL[st.countTokens()];
            for (int i = 0; i < u.length; i++)
              {
                String path = st.nextToken();
                File f = new File(path);
                try
                  {
                    u[i] = f.toURL();
                  }
                catch (java.net.MalformedURLException mue)
                  {
                    logError("malformed classpath component " + path);
                    return true;
                  }
              }
            loader = new URLClassLoader(u);
          }
	else if (arg.equals("-help"))
          {
            usage();
            return true;
          }
	else if (arg.equals("-version"))
	  {
	    System.out.println("rmic (" + System.getProperty("java.vm.name")
	                       + ") " + System.getProperty("java.vm.version"));
	    System.out.println();
	    System.out.println("Copyright 2002 Free Software Foundation, Inc.");
	    System.out.println("This is free software; see the source for copying conditions.  There is NO");
	    System.out.println("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
            return true;
	  }
	else if (arg.equals("-d"))
	  {
	    destination = args[next];
	    next++;
	  }
	else if (arg.charAt(1) == 'J')
          /* ignoring -J flags that are supposed to be passed to the
             underlying Java interpreter */
          continue;
        else
          {
            logError("unrecognized option '" + arg + "'");
            return true;
          }
      }

    return false;
  }

  private void findRemoteMethods()
    throws RMICException
  {
    List rmeths = new ArrayList();
    for (Class cur = clazz; cur != null; cur = cur.getSuperclass())
      {
        Class[] interfaces = cur.getInterfaces();
        for (int i = 0; i < interfaces.length; i++)
          {
            if (java.rmi.Remote.class.isAssignableFrom(interfaces[i]))
              {
                Class remoteInterface = interfaces[i];
                if (verbose)
                  System.out.println
                    ("[implements " + remoteInterface.getName() + "]");

                // check if the methods declare RemoteExceptions
                Method[] meths = remoteInterface.getMethods();
                for (int j = 0; j < meths.length; j++)
                  {
                    Method m = meths[j];
                    Class[] exs = m.getExceptionTypes();

                    boolean throwsRemote = false;
                    for (int k = 0; k < exs.length; k++)
                      {
                        if (exs[k].isAssignableFrom(RemoteException.class))
                          throwsRemote = true;
                      }

                    if (! throwsRemote)
                      {
                        throw new RMICException
                          ("Method " + m + " in interface " + remoteInterface
                           + " does not throw a RemoteException");
                      }

                    rmeths.add(m);
                  }

                mRemoteInterfaces.add(remoteInterface);
              }
          }
      }

    // intersect exceptions for doubly inherited methods
    boolean[] skip = new boolean[rmeths.size()];
    for (int i = 0; i < skip.length; i++)
      skip[i] = false;
    List methrefs = new ArrayList();
    for (int i = 0; i < rmeths.size(); i++)
      {
        if (skip[i]) continue;
        Method current = (Method) rmeths.get(i);
        MethodRef ref = new MethodRef(current);
        for (int j = i+1; j < rmeths.size(); j++)
          {
            Method other = (Method) rmeths.get(j);
            if (ref.isMatch(other))
              {
                ref.intersectExceptions(other);
                skip[j] = true;
              }
          }
        methrefs.add(ref);
      }

    // Convert into a MethodRef array and sort them
    remotemethods = (MethodRef[])
      methrefs.toArray(new MethodRef[methrefs.size()]);
    Arrays.sort(remotemethods);
  }

  /**
   * Prints an error to System.err and increases the error count.
   */
  private void logError(Exception theError)
  {
    logError(theError.getMessage());
    if (verbose)
      theError.printStackTrace(System.err);
  }

  /**
   * Prints an error to System.err and increases the error count.
   */
  private void logError(String theError)
  {
    errorCount++;
    System.err.println("error: " + theError);
  }

  private static void usage()
  {
    System.out.println("Usage: rmic [OPTION]... CLASS...\n" + "\n"
                       + "	-keep *			Don't delete any intermediate files\n"
                       + "	-keepgenerated *	Same as -keep\n"
                       + "	-v1.1			Java 1.1 style stubs only\n"
                       + "	-vcompat		Java 1.1 & Java 1.2 stubs\n"
                       + "	-v1.2			Java 1.2 style stubs only\n"
                       + "	-g *			Generated debugging information\n"
                       + "	-depend *		Recompile out-of-date files\n"
                       + "	-nowarn	*		Suppress warning messages\n"
                       + "	-nocompile *		Don't compile the generated files\n"
                       + "	-verbose 		Output what's going on\n"
                       + "	-classpath <path> 	Use given path as classpath\n"
                       + "	-d <directory> 		Specify where to place generated classes\n"
                       + "	-J<flag> *		Pass flag to Java\n"
                       + "	-help			Print this help, then exit\n"
                       + "	-version		Print version number, then exit\n" + "\n"
                       + "  * Option currently ignored\n"
                       + "Long options can be used with `--option' form as well.");
  }

  private static String getPrettyName(Class cls)
  {
    StringBuffer str = new StringBuffer();
    for (int count = 0;; count++)
      {
	if (! cls.isArray())
	  {
	    str.append(cls.getName());
	    for (; count > 0; count--)
	      str.append("[]");
	    return (str.toString());
	  }
	cls = cls.getComponentType();
      }
  }

  private static class MethodRef
    implements Comparable
  {
    Method meth;
    long hash;
    List exceptions;
    private String sig;

    MethodRef(Method m) {
      meth = m;
      sig = Type.getMethodDescriptor(meth);
      hash = RMIHashes.getMethodHash(m);
      // add exceptions removing subclasses
      exceptions = removeSubclasses(m.getExceptionTypes());
    }

    public int compareTo(Object obj) {
      MethodRef that = (MethodRef) obj;
      int name = this.meth.getName().compareTo(that.meth.getName());
      if (name == 0) {
        return this.sig.compareTo(that.sig);
      }
      return name;
    }

    public boolean isMatch(Method m)
    {
      if (!meth.getName().equals(m.getName()))
        return false;

      Class[] params1 = meth.getParameterTypes();
      Class[] params2 = m.getParameterTypes();
      if (params1.length != params2.length)
        return false;

      for (int i = 0; i < params1.length; i++)
        if (!params1[i].equals(params2[i])) return false;

      return true;
    }

    private static List removeSubclasses(Class[] classes)
    {
      List list = new ArrayList();
      for (int i = 0; i < classes.length; i++)
        {
          Class candidate = classes[i];
          boolean add = true;
          for (int j = 0; j < classes.length; j++)
            {
              if (classes[j].equals(candidate))
                continue;
              else if (classes[j].isAssignableFrom(candidate))
                add = false;
            }
          if (add) list.add(candidate);
        }

      return list;
    }

    public void intersectExceptions(Method m)
    {
      List incoming = removeSubclasses(m.getExceptionTypes());

      List updated = new ArrayList();

      for (int i = 0; i < exceptions.size(); i++)
        {
          Class outer = (Class) exceptions.get(i);
          boolean addOuter = false;
          for (int j = 0; j < incoming.size(); j++)
            {
              Class inner = (Class) incoming.get(j);

              if (inner.equals(outer) || inner.isAssignableFrom(outer))
                addOuter = true;
              else if (outer.isAssignableFrom(inner))
                updated.add(inner);
            }

          if (addOuter)
            updated.add(outer);
        }

      exceptions = updated;
    }
  }
}
