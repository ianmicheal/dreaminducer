CC=$(KOS_CC)
# If you know what your system's byte order is, define XML_BYTE_ORDER:
# use -DXML_BYTE_ORDER=12 for little-endian byte order;
# use -DXML_BYTE_ORDER=21 for big-endian (network) byte order.
# -DXML_NS adds support for checking of lexical aspects of XML namespaces spec
# -DXML_MIN_SIZE makes a smaller but slower parser
# -DXML_DTD adds full support for parsing DTDs
CFLAGS=$(KOS_CFLAGS) $(KOS_LOCAL_CFLAGS) -D_arch_$(KOS_ARCH) -Ixmltok -Ixmlparse -DXML_NS -DXML_DTD -I$(KOS_INCS) -I$(KOS_BASE)/kernel/arch/$(KOS_ARCH)/include
AR=$(KOS_AR)
# Use one of the next two lines; unixfilemap is better if it works.
#FILEMAP_OBJ=xmlwf/unixfilemap.o
FILEMAP_OBJ=xmlwf/readfilemap.o
LIBOBJS=xmltok/xmltok.o \
  xmltok/xmlrole.o \
  xmlparse/xmlparse.o

OBJS=xmlwf/xmlwf.o \
  xmlwf/xmlfile.o \
  xmlwf/codepage.o \
  $(FILEMAP_OBJ)
LIB=libexpat.a
XMLWF=xmlwf/xmlwf

all: $(LIB)

$(XMLWF): $(OBJS) $(LIB)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIB)

$(LIB): $(LIBOBJS)
	$(AR) rc $(LIB) $(LIBOBJS)

clean:
	rm -f $(OBJS) $(LIBOBJS) $(LIB) $(XMLWF)

xmltok/nametab.h: gennmtab/gennmtab
	rm -f $@
	gennmtab/gennmtab >$@

gennmtab/gennmtab: gennmtab/gennmtab.c
	gcc -Wall -O0 -Ixmltok -Ixmlparse -DXML_NS -DXML_DTD -o $@ gennmtab/gennmtab.c

xmltok/xmltok.o: xmltok/nametab.h

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
