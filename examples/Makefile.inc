CC=gcc -std=c17
CFLAGS=-c -Wall -fPIC -Os
LDFLAGS=
PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
CDF_HOME ?= ${HOME}/.cdf
BUILD_DIR := $(PROJECT_ROOT)build

CDF_CORE_VERSION := $(shell jq -r '.dependencies[] | select(.group=="cdf") | select(.name=="cdf") | .version' $(PROJECT_ROOT)cdfmodule.json | sed 's|-|.|' | awk -F"." '{if ($$4) {print $$1"."$$2"-"$$4} else {print $$1"."$$2}}')
CDF_CORE_PATH := $(CDF_HOME)/cdf/cdf/$(CDF_CORE_VERSION)

CDF_JSON_VERSION := $(shell jq -r '.dependencies[] | select(.group=="cdf") | select(.name=="cdf-json") | .version' $(PROJECT_ROOT)cdfmodule.json | sed 's|-|.|' | awk -F"." '{if ($$4) {print $$1"."$$2"-"$$4} else {print $$1"."$$2}}')
CDF_JSON_PATH := $(CDF_HOME)/cdf/cdf-json/$(CDF_JSON_VERSION)

CDF_HTTP_VERSION := $(shell jq -r '.dependencies[] | select(.group=="cdf") | select(.name=="cdf-http") | .version' $(PROJECT_ROOT)cdfmodule.json | sed 's|-|.|' | awk -F"." '{if ($$4) {print $$1"."$$2"-"$$4} else {print $$1"."$$2}}')
CDF_HTTP_PATH := $(CDF_HOME)/cdf/cdf-http/$(CDF_HTTP_VERSION)

CDF_DB_VERSION := $(shell jq -r '.dependencies[] | select(.group=="cdf") | select(.name=="cdf-db") | .version' $(PROJECT_ROOT)cdfmodule.json | sed 's|-|.|' | awk -F"." '{if ($$4) {print $$1"."$$2"-"$$4} else {print $$1"."$$2}}')
CDF_DB_PATH := $(CDF_HOME)/cdf/cdf-db/$(CDF_DB_VERSION)

CDF_DB_SQLITE_VERSION := $(shell jq -r '.dependencies[] | select(.group=="cdf") | select(.name=="cdf-db-sqlite") | .version' $(PROJECT_ROOT)cdfmodule.json | sed 's|-|.|' | awk -F"." '{if ($$4) {print $$1"."$$2"-"$$4} else {print $$1"."$$2}}')
CDF_DB_SQLITE_PATH := $(CDF_HOME)/cdf/cdf-db-sqlite/$(CDF_DB_SQLITE_VERSION)

CDF_DB_ENTITY_VERSION := $(shell jq -r '.dependencies[] | select(.group=="cdf") | select(.name=="cdf-db-entity") | .version' $(PROJECT_ROOT)cdfmodule.json | sed 's|-|.|' | awk -F"." '{if ($$4) {print $$1"."$$2"-"$$4} else {print $$1"."$$2}}')
CDF_DB_ENTITY_PATH := $(CDF_HOME)/cdf/cdf-db-entity/$(CDF_DB_ENTITY_VERSION)
