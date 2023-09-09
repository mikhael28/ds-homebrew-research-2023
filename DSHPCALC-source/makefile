# ex:set ts=4 sw=4: <- for vi
#
# if we are invoked with no particular calculator, iterate around doing them all
#
ifeq ($(TARGET),)

export _MAKEDIR	:=	$(CURDIR)

default:	_hp11c _hp16c

_hp11c:
	@$(MAKE) -$(MAKEFLAGS) TARGET=HP11C TEXT2="Scientific Calculator" 

_hp16c:
	@$(MAKE) -$(MAKEFLAGS) TARGET=HP16C TEXT2="Programmers Calculator"

clean:
	@$(MAKE) -$(MAKEFLAGS) TARGET=HP11C TEXT2="Scientific Calculator" clean
	@$(MAKE) -$(MAKEFLAGS) TARGET=HP16C TEXT2="Programmers Calculator" clean
	@-rm -f ds-hpcalc-sources.zip hp11c.zip hp16c.zip

_release:	hp11c.zip hp16c.zip ds-hpcalc-sources.zip
	@echo Done

hp11c.zip: _hp11c
	@(cd Release;zip -uq ../hp11c.zip HP11C.*)
	@-zip -uq hp11c.zip LICENSE COPYING

hp16c.zip: _hp16c
	@(cd Release;zip -uq ../hp16c.zip HP16C.*)
	@-zip -uq hp16c.zip LICENSE COPYING

ds-hpcalc-sources.zip: contents
	@-zip -uq ds-hpcalc-sources.zip -@ <contents

contents:
	find common data HP11C HP16C makefile nonpareil objtoc.pl jl_rules -print >contents
	@echo remove undesirable files from contents before trying again
	exit 1

else

#-------------------------------------------------------------------------------
# TARGET is the name of the output file
# BUILD is the directory where object files & intermediate files will be placed
# RELEASE is where the binary files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#-------------------------------------------------------------------------------

SOURCES			:=	common nonpareil $(TARGET) 
INCLUDES		:=	common nonpareil

TEXT1 			:= $(TARGET) 
TEXT2 			?= Calculator
TEXT3 			:=
ICON 			:= $(TARGET)/logo.bmp

USE_WOOPSI		:= 1
USE_FAT			:= 1
NO_AUTO_BINARY	:= 1

include $(PWD)/jl_rules

endif
