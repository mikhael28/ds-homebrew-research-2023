#pragma once

// ---------------------------------------------------

#define CODE_IN_ITCM __attribute__ ((section ("ITCM")))
#define CODE_IN_ITCM_MemTool __attribute__ ((section ("ITCM_MemTool")))
#define CODE_IN_ITCM_GLIB __attribute__ ((section ("ITCM_GLIB")))

#define DATA_IN_DTCM __attribute__ ((section (".dtcm")))

// ---------------------------------------------------

