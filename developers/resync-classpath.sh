## Script to automatically download and apply a patch
## from GNU Classpath's CVS tree.

## Invoke as resync-classpath.sh month patch
## for example: resync-classpath.sh 2005-02 168

## Invoke from a directory at same level as
## Kaffe's top source directory.

PATCH_ARCHIVE=http://lists.gnu.org/archive/html/commit-classpath/${1}/msg00

for i in ${1+"$@"} ; do  \
  lynx -dump -width=1024 $PATCH_ARCHIVE$i.html \
  | grep diff \
  | grep http \
  | grep -v "\[" \
  | grep -v ChangeLog \
  | sed 's/r2=text/r2=text\&diff_format=u/g' \
  | xargs wget ; thisdir=`pwd`; \
  cd ../kaffe/libraries/javalib/ ; \
  for p in $thisdir/*diff* ; do \
    patch -p 2 -i $p && rm $p ; \
  done ; 
  cd $thisdir ; 
done
