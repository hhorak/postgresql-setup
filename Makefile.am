ctl		= $(NAME_BINARYBASE)-ctl
ctl_in		= postgresql-ctl.in

checkdb		= $(NAME_BINARYBASE)-check-db-dir
checkdb_in	= postgresql-check-db-dir.in

service		= $(NAME_SERVICE).service
service_in	= postgresql.service.in
serviceat	= $(NAME_SERVICE)@.service
serviceat_in	= postgresql.service.in

initscript	= initscript
initscript_in	= postgresql.init.in

readme_dist	= README.rpm-dist
readme_dist_db	= README.rpm-dist.sgml
readme_dist_m4	= README.rpm-dist.m4
readme_dist_sed	= README.rpm-dist.sed
readme_template	= README.rpm-dist.template

INSTALL_SCRIPT = $(top_srcdir)/auxdir/our-install-sh
EXTRA_DIST = $(INSTALL_SCRIPT)
INTERMEDIATE_FILES =
GENERATED_FILES =
CLEANFILES =
DISTCLEANFILES =
_AX_TEXT_ADDITIONAL_SED_SUBSTITUTIONS =

DISTCHECK_CONFIGURE_FLAGS = \
	rpmmacrosdir=$$dc_install_base/etc/rpm \
	systemdunitsdir=$$dc_install_base/lib/systemd/system \
	systemdunitsdir=$$dc_install_base/var/lib/systemd/system \
	systemduserunitsdir=$$dc_install_base/etc/systemd/system \
	systemdlegacyscriptsdir=$$dc_install_base/usr/libexec/initscripts/legacy-actions \
	initscriptsdir=$$dc_install_base=/etc/rc.d/init.d \
	initscriptsconfdir=$$dc_install_base/etc/sysconfig/pgsql

# include $(srcdir)/build-helpers/Makefile.inc

SUBDIRS = . tests

libexec_SCRIPTS = $(checkdb)

legacyscriptsdir = $(systemdlegacyscriptsdir)/$(NAME_SERVICE)

noinst_DATA =

# TODO: Ideally, 'make distcheck' should check every file we generate.
if WANT_SYSVINIT
noinst_DATA += $(initscript)
libexec_SCRIPTS += $(ctl)
GENERATED_FILES += $(initscript) $(ctl)
else
systemdunits_DATA = $(service) $(serviceat)
legacyscripts_SCRIPTS = initdb upgrade
endif

pgdoc_DATA = $(readme_dist)

noinst_DATA += $(TEST_GEN_FILES_LIST)

$(ctl): $(ctl_in) $(text_tpl_deps)
	$(text_tpl_gen_script)

$(checkdb): $(check) $(checkdb_in) $(text_tpl_deps)
	$(text_tpl_gen_script)

$(initscript): $(initscript_in) $(text_tpl_deps)
	$(text_tpl_gen_script)

initdb upgrade: legacy-sysv-script.in $(text_tpl_deps)
	$(text_tpl_gen_script)

# In ideal world, postgresql@.service would be hardlink to postgresql.service.
# That would require, however, the rhbz#1141824 resolved so we could install
# system-default drop-in file.  Make a "almost" duplicate files now.

$(serviceat): $(serviceat_in) $(text_tpl_deps)
	$(AM_V_GEN)\
	ax_text_add_sed_substs="-e /@PGDATA_ENVIRONMENT[@]/d" ; \
	$(text_tpl_gen_verbose)

$(service): $(service_in) $(text_tpl_deps)
	$(AM_V_GEN)\
	ax_text_add_sed_substs="-e s|@PGDATA_ENVIRONMENT[@]|Environment=PGDATA=$(PGDATADIR)|" ; \
	$(text_tpl_gen_verbose)

$(readme_dist_sed): $(readme_template) $(text_tpl_deps)
	$(text_tpl_gen)

$(readme_dist_m4): $(readme_dist_sed)
	$(text_tpl_gen_conv) && \
	    m4 -P < $< > $@

$(readme_dist_db): $(readme_dist_m4)
	$(text_tpl_gen_conv) && \
	    $(text_tpl_sed_call) < $(readme_dist_m4) | m4 -P > $(readme_dist_db)

$(readme_dist): $(readme_dist_db) $(srcdir)/style.dsl
	$(text_tpl_gen_conv) && docbook2txt -d $(srcdir)/style.dsl $(readme_dist_db) >/dev/null && \
	    mv -f $(readme_dist).txt $(readme_dist)

EXTRA_DIST += \
	$(srcdir)/*.in \
	$(srcdir)/etc/postgresql-setup/upgrade/postgresql.conf \
	$(readme_template) \
	style.dsl

include $(srcdir)/share/postgresql-setup/Makefile.inc
include $(srcdir)/bin/Makefile.inc
include $(srcdir)/lib/rpm/Makefile.inc
include $(srcdir)/sbin/Makefile.inc
include $(srcdir)/doc/Makefile.inc

GENERATED_FILES += \
	$(bin_SCRIPTS) \
	$(sbin_SCRIPTS) \
	$(libexec_SCRIPTS) \
	$(pgdoc_DATA) \
	$(pkgsysconf_DATA) \
	$(systemdunits_DATA) \
	$(legacyscripts_SCRIPTS)

INTERMEDIATE_FILES += \
	$(readme_dist_m4) \
	$(readme_dist_sed) \
	$(readme_dist_db)

CLEANFILES += $(GENERATED_FILES) $(TEST_GEN_FILES_LIST) $(INTERMEDIATE_FILES)

$(TEST_GEN_FILES_LIST): $(GENERATED_FILES)
	$(AM_V_GEN)rm -rf $(TEST_GEN_FILES_LIST) && \
	for i in $(GENERATED_FILES); do \
	    echo $$i >> $@ ; \
	done

install-data-hook:
if WANT_SYSVINIT
else
	test `id -u` -ne 0 || systemctl daemon-reload
endif
	$(MKDIR_P) $(DESTDIR)/$(sysconfdir)/$(NAME_BINARYBASE)-setup/upgrade

install-exec-hook:
if WANT_SYSVINIT
	$(MKDIR_P) $(DESTDIR)/$(initscriptsdir)
	$(install_sh_SCRIPT) $(initscript) $(DESTDIR)/$(initscriptsdir)/$(NAME_SERVICE)
endif

uninstall-hook:
	rm -rf $(DESTDIR)/$(initscriptsdir)/$(NAME_SERVICE)

GL_GEN_BIN = gitlog-to-changelog
GL_GEN = if test -d .git; then \
	   $(SHELL) $(top_srcdir)/$(config_aux_dir)/missing --run \
	   $(GL_GEN_BIN) --format='%s%n%n%b%n' --since 2014-06-30 > ChangeLog ; \
	   rc=$$? ; \
	   if test $$rc -eq 127; then \
	     if test x"$$IGNORE_CHANGELOG" != xyes; then \
	       echo >&2 "ERROR: Get the $(GL_GEN_BIN) or ignore ChangeLog by IGNORE_CHANGELOG=yes" ; \
	       exit 1 ; \
	     fi ; \
	   else \
	     test $$rc -eq 0 ; \
	   fi \
	 fi

.PHONY: ChangeLog
ChangeLog:
	$(AM_V_GEN)$(GL_GEN)

dist-hook: ChangeLog

DISTCLEANFILES += ChangeLog
