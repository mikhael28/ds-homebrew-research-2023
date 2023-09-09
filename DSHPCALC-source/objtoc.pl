#
# convert a .obj ROM file to our .c
#
die unless open(f,"11c.obj");
die unless open(o,">hp11crom.h");
print o <<EOF
/*
 * HP11C ROM
 */
static const rom_word_t hp11crom[] = {
EOF
;

while (<f>) {
	tr /a-z/A-Z/;
	($a,$d) = /^([0-9A-Z]+):([0-9A-Z]+)$/;
	next unless defined($a) && defined($d);
	printf o "/* %04x */\t0x%03x,\n", hex($a), hex($d);
}
print o <<EOF
};
EOF
;
close(f)
