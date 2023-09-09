
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <NDS.h>

#include "_const.h"
#include "_console.h"
#include "shell.h"
#include "strtool.h"
#include "procstate.h"
#include "unicode.h"

#include "memtool.h"
#include "arm9tcm.h"

#include "lang.h"

// -----------------------------------------------------

extern char Shell_FAT_fopen_fullfn[MaxFilenameLength+1];
char Shell_FAT_fopen_fullfn[MaxFilenameLength+1];

#include "shell_FAT_fopen.h"
#include "shell_FAT_Read.h"

// -----------------------------------------------------

#include "shell_ConvertFull.h"

#include "shell_Helpers.h"

// -----------------------------------------------------------------------

