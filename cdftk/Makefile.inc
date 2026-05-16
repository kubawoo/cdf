CC=gcc -std=c17
CFLAGS=-c -Wall -fPIC
LDFLAGS=
PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
CDF_HOME ?= ${HOME}/.cdf
MOD_GROUP := $(shell jq -r '.group' $(PROJECT_ROOT)cdfmodule.json)
MOD_NAME := $(shell jq -r '.name' $(PROJECT_ROOT)cdfmodule.json)
MOD_VERSION := $(shell jq -r '.version' $(PROJECT_ROOT)cdfmodule.json | sed 's|-|.|' | awk -F"." '{if ($$4) {print $$1"."$$2"-"$$4} else {print $$1"."$$2}}')
MOD_INSTALL_PATH = $(CDF_HOME)/$(MOD_GROUP)/$(MOD_NAME)/$(MOD_VERSION)

TEST_FRAMEWORK_VERSION := $(shell jq -r '.dependencies[] | select(.group=="cdf") | select(.name=="test-framework") | .version' $(PROJECT_ROOT)cdfmodule.json | sed 's|-|.|' | awk -F"." '{if ($$4) {print $$1"."$$2"-"$$4} else {print $$1"."$$2}}')
TEST_FRAMEWORK_PATH := $(CDF_HOME)/cdf/test-framework/$(TEST_FRAMEWORK_VERSION)
TEST_RUNNER := $(TEST_FRAMEWORK_PATH)/bin/testrunner

CDF_CORE_VERSION := $(shell jq -r '.dependencies[] | select(.group=="cdf") | select(.name=="cdf-core") | .version' $(PROJECT_ROOT)cdfmodule.json | sed 's|-|.|' | awk -F"." '{if ($$4) {print $$1"."$$2"-"$$4} else {print $$1"."$$2}}')
CDF_CORE_PATH := $(CDF_HOME)/cdf/cdf-core/$(CDF_CORE_VERSION)

CDF_JSON_VERSION := $(shell jq -r '.dependencies[] | select(.group=="cdf") | select(.name=="cdf-json") | .version' $(PROJECT_ROOT)cdfmodule.json | sed 's|-|.|' | awk -F"." '{if ($$4) {print $$1"."$$2"-"$$4} else {print $$1"."$$2}}')
CDF_JSON_PATH := $(CDF_HOME)/cdf/cdf-json/$(CDF_JSON_VERSION)

ifneq (,$(findstring SNAPSHOT,$(MOD_VERSION)))
CFLAGS += -g
else
CFLAGS += -Os
endif
