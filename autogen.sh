# http://www.developingprogrammers.com/index.php/2006/01/05/autotools-tutorial/

rm -f aclocal.m4 configure

aclocal         # aclocal.m4
autoheader      # config.h.in
automake -ac    # Makefile.in
autoconf        # configure

rm -rf autom4te.cache
