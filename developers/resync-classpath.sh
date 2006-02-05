## Script to automatically download and apply a patch
## from GNU Classpath's CVS tree.

## Invoke as resync-classpath.sh 

## Invoke from a directory at same level as
## Kaffe's top source directory.

WORKDIR=/tmp/classpath-merge-work


echo "Building file/directories listing for new classpath"
(
    mkdir -p ${WORKDIR}
    cd ${WORKDIR};

    echo "Extracting classpath from CVS"
     cvs -z3 -d:pserver:anonymous@cvs.sv.gnu.org:/sources/classpath co -P classpath;
    
    cd classpath;

    sh ./autogen.sh
    cd ..
    mkdir -p build
    cd build
    rm -rf ../build/*
    echo "building classpath distribution"
    ../classpath/configure --with-jikes && make && make dist
    tar zxf classpath-*tar.gz
    cd classpath-*

    find . '(' -name "CVS" -o -name "autom4te*" ')' -prune \
	-o '(' -type d -a !  -name "." ')' \
        -print | sort > ${WORKDIR}/new_directories;

    find . '(' -name "CVS" -o -name "autom4te*" ')' -prune \
	-o '(' -type f -a ! -name "*~" ')'  -print | \
	sort > ${WORKDIR}/new_file_list;
)

echo "Building file/directories listing for kaffe's classpath"
(
    cd libraries/javalib/external/classpath;

    find . '(' -name "CVS" -o -name "autom4te*" ')' -prune \
	-o '(' -type d -a !  -name "." ')' \
        -print | sort > ${WORKDIR}/old_directories;

    find . '(' -name "CVS" -o -name "autom4te*" ')' -prune \
	-o '(' -type f -a ! -name "*~" ')'  -print | \
	sort > ${WORKDIR}/old_file_list;


echo "Building remove list for files"
diff -u ${WORKDIR}/old_file_list ${WORKDIR}/new_file_list | grep "^-\./" | sed "s%^-\./%%g" > ${WORKDIR}/remove_list

diff -u ${WORKDIR}/old_file_list ${WORKDIR}/new_file_list | grep "^+\./" | sed "s%^+\./%%g" > ${WORKDIR}/add_list

echo "Making directories"
for i in `cat ${WORKDIR}/new_directories`; do
    mkdir -p ${i};
done

echo "Copying all files from classpath's CVS"
for i in `cat ${WORKDIR}/new_file_list`; do
    cp -pf ${WORKDIR}/build/classpath-*/${i} ${i};
done

)
