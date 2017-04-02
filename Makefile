PROJECT := carbot-bridge
SCRIPTDIR := $(shell pwd)
ROOTDIR := $(shell cd $(SCRIPTDIR) && pwd)
VERSION:= $(shell cat $(ROOTDIR)/VERSION)
COMMIT := $(shell git rev-parse --short HEAD)

GOBUILDDIR := $(SCRIPTDIR)/.gobuild
SRCDIR := $(SCRIPTDIR)
BINDIR := $(ROOTDIR)
VENDORDIR := $(SCRIPTDIR)/vendor

ORGPATH := github.com/HuuskeRobots
ORGDIR := $(GOBUILDDIR)/src/$(ORGPATH)
REPONAME := $(PROJECT)
REPODIR := $(ORGDIR)/$(REPONAME)
REPOPATH := $(ORGPATH)/$(REPONAME)
BIN := $(BINDIR)/$(PROJECT)

GOPATH := $(GOBUILDDIR)
GOVERSION := 1.8-alpine

ifndef GOOS
	GOOS := darwin
endif
ifndef GOARCH
	GOARCH := amd64
endif
ifeq ("$(GOOS)", "windows")
        GOEXE := .exe
endif

SOURCES := $(shell find $(SRCDIR) -name '*.go')

.PHONY: all clean deps sample

all: deployment

build: $(BIN)

deployment:
	@${MAKE} -B GOOS=windows GOARCH=amd64 GOEXE=.exe build
	@${MAKE} -B GOOS=linux GOARCH=amd64 build
	@${MAKE} -B GOOS=linux GOARCH=386 build
	@${MAKE} -B GOOS=darwin GOARCH=amd64 build

clean:
	rm -Rf $(BIN) $(GOBUILDDIR) .pioenvs bin

deps:
	@${MAKE} -B -s $(GOBUILDDIR)

$(GOBUILDDIR):
	@mkdir -p $(ORGDIR)
	@rm -f $(REPODIR) && ln -s ../../../.. $(REPODIR)

update-vendor:
	@rm -Rf $(VENDORDIR)
	@pulsar go vendor -V $(VENDORDIR) \
		github.com/juju/errgo \
		github.com/oleksandr/bonjour

$(BIN): $(GOBUILDDIR) $(SOURCES)
	GOOS=$(GOOS) GOARCH=$(GOARCH) GOPATH=$(GOBUILDDIR) go build -ldflags "-s -X main.projectVersion=$(VERSION) -X main.projectBuild=$(COMMIT)" -o bin/$(GOOS)-$(GOARCH)/$(PROJECT)$(GOEXE) $(REPOPATH)

