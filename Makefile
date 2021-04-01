# Makefile for ifmail Internet/FidoNet gateway
# Copyright (c) 1993-1997 by Eugene G. Crosser

INCDIR = ../iflib

include ../CONFIG

DEFINES = -DNEWSLOG=${NEWSLOG} \
		-DMAILLOG=${MAILLOG} \
		-DNEWSSPOOL=\"${NEWSSPOOL}\" \
		-DFAKEDIR=\"${FAKEDIR}\" \
		${OPTS}

OBJMAIL = version.o ifmail.o rfcmsg.o message.o mkftnhdr.o \
		ifdbm.o batchrd.o areas.o msgflags.o\
		nlindex.o nodecheck.o \
		msgidbm.o attach.o \
		charconv.o charconv_jp.o charconv_hz.o charconv_utf.o
OBJTOSS = version.o iftoss.o areas.o \
		getmessage.o mkrfcmsg.o rfcmsg.o batchrd.o \
		ifdbm.o backalias.o msgflags.o \
		charconv.o charconv_jp.o charconv_hz.o charconv_utf.o
OBJUNPACK = version.o ifunpack.o unpacker.o flock.o
OBJPACK = version.o ifpack.o flock.o
OBJSTAT = version.o ifstat.o
SRCS = ifmail.c rfcmsg.c message.c mkftnhdr.c \
		ifdbm.c batchrd.c areas.c msgflags.c \
		iftoss.c getmessage.c mkrfcmsg.c \
		nlindex.c nodecheck.c \
		ifunpack.c unpacker.c ifpack.c flock.c \
		backalias.c msgidbm.c attach.c ifstat.c lastmtime.c \
		body.c charconv.c charconv_jp.c charconv_hz.c charconv_utf.c
HDRS = areas.h mkrfcmsg.h mkftnhdr.h nlindex.h nodecheck.h \
		charconv.h charconv_jp.h charconv_hz.h
OTHER = README Makefile testmail newsin pkt ifmail.8 iftoss.8
ALL = ifmail ifnews iftoss ifunpack ifpack ifstat

ifeq (yes,${SHARED})
UTLIB =
OBJMAIL   += ../iflib/libifmail.so.${SHVER}
OBJTOSS   += ../iflib/libifmail.so.${SHVER}
OBJUNPACK += ../iflib/libifmail.so.${SHVER}
OBJPACK   += ../iflib/libifmail.so.${SHVER}
OBJSTAT   += ../iflib/libifmail.so.${SHVER}
LIBS += -L ../iflib -lifmail
else
UTLIB = ../iflib/utlib.a
endif

.c.o:
	${CC} -c ${CFLAGS} ${INCLUDES} ${DEFINES} $<

all:	${ALL}

install: all
	${INSTALL} -s -o ${OWNER} -g ${GROUP} -m ${SMODE} ifstat ${BINDIR}/ifstat
	${INSTALL} -s -o ${OWNER} -g ${GROUP} -m ${MODE} ifpack ${BINDIR}/ifpack
	${INSTALL} -s -o ${OWNER} -g ${GROUP} -m ${MODE} ifunpack ${BINDIR}/ifunpack
	${INSTALL} -s -o ${OWNER} -g ${GROUP} -m ${MODE} iftoss ${BINDIR}/iftoss
	${INSTALL} -s -o ${OWNER} -g ${GROUP} -m ${SMODE} ifmail ${BINDIR}/ifmail
	rm -f ${BINDIR}/ifnews
	ln ${BINDIR}/ifmail ${BINDIR}/ifnews

tryt:	iftoss
	./iftoss -I../misc/config -N -xch <pkt

tryn:	ifmail
	./ifmail -I../misc/config -n -N -xc f39.n5000 <newsin

trym:	ifmail
	./ifmail -I../misc/config -rf0.n5020.z100 -N -xce \
		Pete.Kvitek@f5.fido.pccross.msk.su <testmail

tryu:	ifunpack
	./ifunpack -I../misc/config -x15

tryp:	ifpack
	./ifpack -I../misc/config -xc -N

clean:
	rm -f *.o core ${ALL} lastmtime iflog ifdebug filelist version.c \
	Makefile.bak

ifstat:	${OBJSTAT} ${UTLIB}
	${CC} ${LDFLAGS} ${OBJSTAT} ${UTLIB} -o ifstat ${LIBS}

ifmail:	${OBJMAIL} ${UTLIB}
	${CC} ${LDFLAGS} ${OBJMAIL} ${UTLIB} -o ifmail ${LIBS}

ifnews:	ifmail
	rm -f ifnews
	ln ifmail ifnews

iftoss:	${OBJTOSS} ${UTLIB}
	${CC} ${LDFLAGS} ${OBJTOSS} ${UTLIB} -o iftoss ${LIBS}

ifunpack: ${OBJUNPACK} ${UTLIB}
	${CC} ${LDFLAGS} ${OBJUNPACK} ${UTLIB} -o ifunpack ${LIBS}

ifpack: ${OBJPACK} ${UTLIB}
	${CC} ${LDFLAGS} ${OBJPACK} ${UTLIB} -o ifpack ${LIBS}

lastmtime: lastmtime.c
	${CC} ${CFLAGS} lastmtime.c -o lastmtime

tar:	ifmail.tar.z

ifmail.tar.z: ${SRCS} ${HDRS} ${OTHER}
	${TAR} cf - ${SRCS} ${HDRS} ${OTHER} | \
	gzip >$@

filelist:	Makefile
	BASE=`pwd`; \
	BASE=`basename $${BASE}`; \
	(for f in ${SRCS} ${HDRS} ${OTHER} ;do echo ifmail/$${BASE}/$$f; done) \
	>filelist

man:
	${INSTALL} -o ${MANOWNER} -g ${MANGROUP} -m ${MANMODE} ifmail.8 ${MANDIR}/man8/ifmail.8
	${INSTALL} -o ${MANOWNER} -g ${MANGROUP} -m ${MANMODE} iftoss.8 ${MANDIR}/man8/iftoss.8
	echo ".so man8/ifmail.8" > ${MANDIR}/man8/ifnews.8

config:
	test -f config

version.c:	${SRCS} ${HDRS} lastmtime ../CONFIG
	echo 'char *version=${VERSION};' >version.c
	echo 'char *copyright=${COPYRIGHT};' >>version.c
	echo "char *reldate=\"`./lastmtime ${SRCS} ${HDRS}`\";" >>version.c

depend:	version.c
	@rm -f Makefile.bak; \
	mv Makefile Makefile.bak; \
	sed -e '/^# DO NOT DELETE/,$$d' Makefile.bak >Makefile; \
	${ECHO} '# DO NOT DELETE THIS LINE - MAKE DEPEND RELIES ON IT' \
		>>Makefile; \
	${ECHO} '# Dependencies generated by make depend' >>Makefile; \
	for f in ${SRCS} version.c; \
	do \
		${ECHO} "Dependencies for $$f:\c"; \
		${ECHO} "`basename $$f .c`.o:\c" >>Makefile; \
		for h in `sed -n -e \
			's/^#[ 	]*include[ 	]*"\([^"]*\)".*/\1/p' $$f`; \
		do \
			${ECHO} " $$h\c"; \
			if [ -r $$h ]; \
			then \
				${ECHO} " $$h\c" >>Makefile; \
			else \
				${ECHO} " ${INCDIR}/$$h\c" >>Makefile; \
			fi; \
		done; \
		${ECHO} " done."; \
		${ECHO} "" >>Makefile; \
	done; \
	${ECHO} '# End of generated dependencies' >>Makefile

# DO NOT DELETE THIS LINE - MAKE DEPEND RELIES ON IT
# Dependencies generated by make depend
ifmail.o: ../iflib/getopt.h ../iflib/lutil.h ../iflib/xutil.h ../iflib/ftn.h ../iflib/rfcaddr.h ../iflib/falists.h ../iflib/rfcmsg.h ../iflib/ftnmsg.h areas.h ../iflib/config.h ../iflib/version.h ../iflib/trap.h ../iflib/hash.h ../iflib/needed.h nodecheck.h ../iflib/charset.h ../iflib/ref_interface.h
rfcmsg.o: ../iflib/xutil.h ../iflib/lutil.h ../iflib/rfcmsg.h
message.o: ../iflib/xutil.h ../iflib/lutil.h ../iflib/ftn.h ../iflib/rfcaddr.h ../iflib/ftnmsg.h ../iflib/rfcmsg.h ../iflib/config.h ../iflib/bwrite.h ../iflib/falists.h ../iflib/version.h ../iflib/hash.h ../iflib/needed.h mkftnhdr.h areas.h ../iflib/charset.h charconv.h ../iflib/mime.h ../iflib/ref_interface.h
mkftnhdr.o: ../iflib/xutil.h ../iflib/lutil.h ../iflib/ftn.h ../iflib/ftnmsg.h ../iflib/rfcmsg.h ../iflib/rfcaddr.h ../iflib/mime.h ../iflib/charset.h charconv.h ../iflib/config.h ../iflib/version.h ../iflib/hash.h areas.h
ifdbm.o: ../iflib/lutil.h ../iflib/config.h
batchrd.o: ../iflib/lutil.h
areas.o: ../iflib/lutil.h ../iflib/xutil.h areas.h ../iflib/config.h ../iflib/lhash.h ../iflib/needed.h ../iflib/charset.h
msgflags.o: ../iflib/xutil.h ../iflib/lutil.h ../iflib/ftnmsg.h
iftoss.o: ../iflib/getopt.h ../iflib/lutil.h ../iflib/config.h ../iflib/version.h ../iflib/ftn.h ../iflib/getheader.h ../iflib/trap.h ../iflib/charset.h ../iflib/ref_interface.h
getmessage.o: ../iflib/xutil.h ../iflib/lutil.h ../iflib/bread.h ../iflib/ftn.h ../iflib/rfcmsg.h mkrfcmsg.h ../iflib/config.h ../iflib/crc.h ../iflib/charset.h
mkrfcmsg.o: ../iflib/lutil.h ../iflib/xutil.h mkrfcmsg.h ../iflib/ftnmsg.h ../iflib/rfcmsg.h areas.h ../iflib/falists.h ../iflib/config.h ../iflib/needed.h ../iflib/charset.h charconv.h ../iflib/mime.h ../iflib/ref_interface.h
nlindex.o: ../iflib/directory.h ../iflib/xutil.h ../iflib/lutil.h ../iflib/ftn.h ../iflib/config.h nodecheck.h nlindex.h
nodecheck.o: ../iflib/directory.h ../iflib/xutil.h ../iflib/lutil.h ../iflib/ftn.h ../iflib/config.h nodecheck.h nlindex.h ../iflib/needed.h
ifunpack.o: ../iflib/directory.h ../iflib/getopt.h ../iflib/xutil.h ../iflib/lutil.h ../iflib/config.h ../iflib/version.h ../iflib/trap.h
unpacker.o: ../iflib/lutil.h ../iflib/config.h
ifpack.o: ../iflib/directory.h ../iflib/getopt.h ../iflib/lutil.h ../iflib/xutil.h ../iflib/ftn.h ../iflib/config.h ../iflib/scanout.h ../iflib/version.h ../iflib/trap.h ../iflib/needed.h
flock.o: ../iflib/lutil.h
backalias.o: ../iflib/xutil.h ../iflib/lutil.h ../iflib/ftn.h ../iflib/needed.h
msgidbm.o: ../iflib/lutil.h ../iflib/xutil.h ../iflib/config.h ../iflib/ftn.h ../iflib/rfcaddr.h
attach.o: ../iflib/ftn.h ../iflib/lutil.h ../iflib/config.h
ifstat.o: ../iflib/getopt.h ../iflib/lutil.h ../iflib/xutil.h ../iflib/ftn.h ../iflib/config.h ../iflib/scanout.h ../iflib/version.h ../iflib/trap.h ../iflib/needed.h
lastmtime.o:
body.o: ../iflib/xutil.h ../iflib/lutil.h ../iflib/rfcmsg.h ../iflib/config.h ../iflib/bwrite.h ../iflib/mime.h ../iflib/charset.h charconv.h ../iflib/version.h ../iflib/hash.h
charconv.o: ../iflib/config.h ../iflib/xutil.h ../iflib/lutil.h ../iflib/charset.h ../iflib/mime.h charconv.h
charconv_jp.o: ../iflib/config.h ../iflib/xutil.h ../iflib/lutil.h ../iflib/charset.h charconv.h charconv_jp.h
charconv_hz.o: ../iflib/config.h ../iflib/xutil.h ../iflib/lutil.h ../iflib/charset.h charconv.h charconv_hz.h
version.o:
# End of generated dependencies
