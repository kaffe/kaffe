Summary: A free virtual machine to run Java(tm) code
Name: kaffe
Version: 1.0.b4
Release: 5
Copyright: GPL
Vendor: Transvirtual Technologies, Inc.
Url: http://www.kaffe.org
Group: Development/Languages
Source0: ftp://ftp.transvirtual.com/pub/kaffe/kaffe-1.0.b4.tar.gz
Buildroot: /var/tmp/kaffe-root
ExclusiveArch: i386 sparc

%description
This is Kaffe, a virtual machine designed to execute Java bytecode.
This machine can be configured in two modes.  In one mode it operates as
a pure bytecode interpreter (not unlike Javasoft's machine); in the second
mode if performs "just-in-time" code conversion from the abstract code to
the host machine's native code.  This will ultimately allow execution of
Java code at the same speed as standard compiled code but while maintaining
the advantages and flexibility of code independence.

%prep
%setup -q
%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=/usr --libexecdir=/usr/lib/kaffe/bin

make

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
/usr/*

%changelog
