include Make.def.clang

VERSION = 2.1.0-dev
PROG = pty
SCRIPTS = pty-agent pty_driver.pm pty-driver.pl script.sh
MANIFEST= *.c *.h ${SCRIPTS} Make* README.md


all:	${PROG} isatty ttyname echoon echooff

echoon: set_echo.o error.o
	${LINK.c} -o $@ $^
	strip $@
echooff: unset_echo.o error.o
	${LINK.c} -o $@ $^
	strip $@

ttyname: ttyname.o
	${LINK.c} -o $@ $^
	strip $@

isatty: isatty.o
	${LINK.c} -o $@ $^
	strip $@

pty:	main.o loop.o driver.o error.o spipe.o ttymodes.o writen.o signalintr.o
	${LINK.c} -o pty $^ ${LDLIBS}
	strip $@

install:all
	@[ "${USER}" != "root" ] \
	|| (echo "Don't install as root! Run 'make install' from your user account; this process will sudo if needed." >&2 \
	    && false)

	@[ -d ~/.pty-agent ] \
	|| (mkdir ~/.pty-agent && chmod 0700 ~/.pty-agent && echo "Created secure ~/.pty-agent dir.") \
	|| (echo "Can't create secure ~/.pty-agent dir: $$!" >&2 && false)

	@[ -d ~/bin ] \
	|| (mkdir ~/bin && echo "Created ~/bin dir.") \
	|| (echo "Can't create ~/bin dir: $$!" >&2 && false)

	@perl -MIO::Select -MTerm::ReadKey -MURI -e 1 \
	|| (echo "Installing IO::Select and Term::ReadKey and URI to system perl tree from CPAN via sudo." \
	    && sudo cpan install IO::Select Term::ReadKey URI) \
	|| (echo "Installation Failed!" >&2 && false)

	@pkill -u $$USER pty-agent; ./pty-agent || ( ([ -x $$(which pip3) ] || sudo install python3-pip) && sudo pip3 install setproctitle cryptography && ./pty-agent)

	cp -f ${PROG} isatty ttyname echoon echooff ${SCRIPTS} ~/bin

clean:
	rm -f ${PROGS} ${TEMPFILES}

%.o:	%.c Make* *.h
	${COMPILE.c} -DVERSION=\"${VERSION}\" $< -o $@

manifest:
	@rm -rf ${PROG}-${VERSION} && mkdir ${PROG}-${VERSION} && cp ${MANIFEST} ${PROG}-${VERSION}

release:manifest
	@tar -czf ${PROG}-${VERSION}.tar.gz ${PROG}-${VERSION} && echo "Made ${PROG}-${VERSION}.tar.gz" && rm -rf ${PROG}-${VERSION}
