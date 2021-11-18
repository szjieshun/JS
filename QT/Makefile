#created by lgb 2014-11-24
#
#

TOPDIR=$(shell pwd)/..
include $(TOPDIR)/../common.mk

PARK_DIR      	:= $(TOPDIR)/park
QT4_DIR=$(TOPDIR)/QT/dm8148_gui_update
QT4_ENV_DIR=$(TOPDIR)/QT/qt4_7

target ?= $(QT4_DIR)/dm8148_gui

.PHONY: all qt_gui clean install 

all :qt4_env qt_gui 
qt4_env:qt4_7_1.tar.bz2	
	$(shell) rm  $(QT4_ENV_DIR) -rf
	$(shell) tar -jxf $<
	@echo =======done, tar[$^]compiled====== 
	touch $@

qt_gui:	
	$(MAKE) -C  $(QT4_DIR)

install:
	$(shell) mkdir -p $(PARK_DIR)
	$(shell) mkdir -p $(PARK_DIR)/app_target
	$(shell) cp $(target) $(TOPDIR)/app_target/ -rf
	$(shell) cp $(QT4_DIR)/sample_hifb $(TOPDIR)/app_target/ -rf
	
	
clean:
	$(MAKE) -C  $(QT4_DIR) clean
	$(shell) rm $(target) -rf
	$(shell) rm $(QT4_ENV_DIR) -rf
	$(shell) rm $(TOPDIR)/QT/qt4_env -f
	




