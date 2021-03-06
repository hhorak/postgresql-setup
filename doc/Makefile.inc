HELP2MAN = $(SHELL) $(top_srcdir)/$(config_aux_dir)/missing --run help2man

# The help2man call
# =================
# Make the rules always dependant on source (thus on postgresql-setup.in, not on
# postgresql-setup), at least for distributed files.  Otherwise, the '*.1'
# manual page is regenerated even when distributed and no code change actually
# happened.  In case there exists some target generating other file directly
# depending on the generated manual page, we would need 'touch $@'.  But it
# seems better to keep warning user about non-existing help2man when '.in'
# changed.
# TODO: make it non-fatal for cross-compilation case also
# TODO: separate it to its own m4 module (for easier distribution)

HELP2MAN_RUN = \
	$(HELP2MAN) --output=$@ $$input -N ; \
	rs=$$? ; \
	if test $$rs -eq 127; then \
	    test -f $@ ; \
	else \
	    test $$rs -eq 0 ; \
	fi

setup_man = %D%/$(NAME_BINARYBASE)-setup.1

new_unit_man = %D%/$(NAME_BINARYBASE)-new-systemd-unit.1

dist_man_MANS = $(setup_man)

if ! WANT_SYSVINIT
dist_man_MANS += $(new_unit_man)
endif

$(setup_man): $(builddir)/bin/$(NAME_BINARYBASE)-setup $(lib)
	$(AM_V_GEN)odir=`dirname "$@"` ; mkdir -p "$$odir" ; \
	export input=$(builddir)/bin/$(NAME_BINARYBASE)-setup ; \
	    $(HELP2MAN_RUN)

$(new_unit_man): sbin/$(NAME_BINARYBASE)-new-systemd-unit $(lib)
	$(AM_V_GEN)odir=`dirname "$@"`; mkdir -p "$$odir" ; \
	export PGSETUP_TEST=: ; \
	export input=$(builddir)/sbin/$(NAME_BINARYBASE)-new-systemd-unit ; \
	    $(HELP2MAN_RUN)

DISTCLEANFILES += $(setup_man) $(new_unit_man)
