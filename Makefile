JAVA := "$(JAVA_HOME)/bin/java"

NATIVE_OUTPUT_DIR ?= build
JAVA_OUTPUT_DIR := target

Default_LIBNAME    := libcameracapture.so
Default_CLASSIFIER := linux-x86_64

Linux-x86_64_LIBNAME    := libcameracapture.so
Linux-x86_64_CLASSIFIER := linux-x86_64

Mac-x86_64_LIBNAME    := libcameracapture.dylib
Mac-x86_64_CLASSIFIER := darwin-x86_64

Mac-aarch64_LIBNAME    := libcameracapture.dylib
Mac-aarch64_CLASSIFIER := darwin-aarch64

Windows-x86_64_LIBNAME    := cameracapture.dll
Windows-x86_64_CLASSIFIER := windows-x86_64

ifeq ($(OS),Windows_NT)
    OS_NAME := Windows
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
		OS_ARCH := x86_64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
			OS_ARCH := x86_64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            OS_ARCH := x86
        endif
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
	    OS_NAME := Linux
    endif
    ifeq ($(UNAME_S),Darwin)
	    OS_NAME := Mac
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
		OS_ARCH := x86_64
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
		OS_ARCH := aarch64
    endif
endif

known_targets := Linux-x86_64 Mac-x86_64 Mac-aarch64 Windows-x86_64
target := $(OS_NAME)-$(OS_ARCH)

ifeq (,$(findstring $(strip $(target)),$(known_targets)))
  target := Default
endif

$(info target is $(target))

LIBNAME    := $($(target)_LIBNAME)
CLASSIFIER := $($(target)_CLASSIFIER)
NATIVE_LIB := src/main/resources/$(LIBNAME)

$(NATIVE_OUTPUT_DIR):
	cmake -B build

native: $(NATIVE_LIB)

$(NATIVE_LIB): $(NATIVE_OUTPUT_DIR)
	cmake --build $(NATIVE_OUTPUT_DIR) --config Release
	mkdir -p $(@D)
	cp $(NATIVE_OUTPUT_DIR)/$(LIBNAME) $(NATIVE_LIB)

extract:
	$(JEXTRACT)/bin/jextract --header-class-name NativeCapture \
		--output target/generated-sources/jextract \
		--target-package com.gluonhq.cameracapture.h \
		--include-function init \
		--include-function start \
		--include-function stop \
		--include-function got_frame \
		src/main/c/cameracapture.h

java: native extract
	mvn -Dclassifier=$(CLASSIFIER) package

install: java
	mvn -Dclassifier=$(CLASSIFIER) install

deploy: java
	mvn -Dclassifier=$(CLASSIFIER) deploy

clean-native:
	rm -rf $(NATIVE_OUTPUT_DIR)

clean-java:
	rm -rf $(JAVA_OUTPUT_DIR)

clean: clean-native clean-java
