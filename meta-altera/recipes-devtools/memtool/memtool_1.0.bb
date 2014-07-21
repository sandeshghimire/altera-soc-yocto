DESCRIPTION = "tools to inspect or alter memory"
SECTION = "dev tools"
LICENSE = "GPLv2"

PV = "1.0"
PR = "r0"

SRC_URI = "file://memtool.c file://Makefile"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

#SRC_URI[md5sum] = ""
#SRC_URI[sha256sum] = ""

S = "${WORKDIR}"

do_install () {
   install -d ${D}${bindir}
   install -m 0755 memtool ${D}${bindir}
}

do_compile() {
   oe_runmake 
}
#inherit pkgconfig

FILES_${PN} = "${bindir}/memtool"

INHIBIT_PACKAGE_DEBUG_SPLIT_${PN} = "1"
#INSANE_SKIP_memtools += "installed_vs_shipped"
INSANE_SKIP_${PN} += "ldflags"

