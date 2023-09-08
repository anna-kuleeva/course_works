
ifeq ($(DISTRO), ELBRUS)
ARCH := e2k-8c
KVER := $(shell uname -r)
endif

## Paths and directories
WORKDIR         = build
CWD             = $(shell pwd)
TOPDIR          = $(WORKDIR)/rpm
BUILDROOT       = $(TOPDIR)/BUILDROOT/$(PKGNAME)-$(RDX_VERSION)-$(BUILD_NUMBER).el7.$(ARCH)
PREFIX          = /usr
DEVEL_PREFIX    = /usr/share/$(PRODUCT)/core
EPREFIX         = $(PREFIX)
SYSCONFDIR      = /etc
BINDIR          = $(EPREFIX)/bin
SBINDIR         = $(EPREFIX)/sbin
LIBDIR          = $(PREFIX)/lib64
LIBEXECDIR      = $(EPREFIX)/libexec
DATADIR         = $(PREFIX)/share
PERLDIR         = $(DATADIR)/perl5
STATEDIR        = /var
DOCS_PREFIX     = /srv/$(PRODUCT)/public/docs/userspace

ifeq ($(DISTRO), SUSE)
REV         = suse
CONTRIB     = contrib_suse
HELPERSDIR  = $(BINDIR)
SYSLOGCONF  = $(CONTRIB)/syslog.conf
INITDIR     = $(SYSCONFDIR)/init.d
else
CONTRIB     = contrib
HELPERSDIR  = $(LIBEXECDIR)
SYSLOGCONF  = $(CONTRIB)/rsyslog.conf
INITDIR     = $(SYSCONFDIR)/rc.d/init.d
endif

ifeq ($(INIT), systemd)
INITDIR     = /usr/lib/systemd/system
endif

DOCKER_UID          ?= $(shell id -u)
DOCKER_GID          ?= $(shell id -g)
DOCKER_NAME         ?= $(shell id -u -n)
DOCKER_TAG           = $(PKGNAME)-$(OS):$(RDX_VERSION)
DOCKER_REGISTRY     ?= gitlab.raidix.com:4567/raidix/raidix-iso/
DOCKER_DEPLOY_PATH   = /tmp/deploy

ifdef NO_CACHE
DOCKER_BUILD_OPTS += --no-cache
endif

RYEAR ?= $(shell date +%Y)

TBD = tbd.txt
