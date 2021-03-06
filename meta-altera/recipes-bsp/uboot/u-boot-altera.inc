DESCRIPTION = "U-boot bootloader for Altera socfpga_cyclone5"
HOMEPAGE = "http://www.altera.com/"
SECTION = "bootloaders"
PROVIDES += " virtual/bootloader u-boot"

inherit deploy

PARALLEL_MAKE=""
PACKAGES=""

UBOOT_REPO ?= "git://git.rocketboards.org/u-boot-socfpga.git"
UBOOT_PROT ?= "http"
UBOOT_TAG ?= "ACDS14.0_REL_GSRD_PR"
SRC_URI = "${UBOOT_REPO};protocol=${UBOOT_PROT};tag=${UBOOT_TAG}"
LIC_FILES_CHKSUM = "file://${S}/COPYING;startline=26;md5=1707d6db1d42237583f50183a5651ecb"
SRCREV_pn-${PN} = "${AUTOREV}"

S = "${WORKDIR}/git"

# GCC 4.5.1 builds unusable binaries using -Os, remove it from OPTFLAGS
EXTRA_OEMAKE = "CROSS_COMPILE=${TARGET_PREFIX}"

python () {
	if not d.getVar("UBOOT_MACHINE", True):
		PN = d.getVar("PN", True)
		FILE = os.path.basename(d.getVar("FILE", True))
		bb.debug(1, "To build %s, see %s for instructions on \
			     setting up your machine config" % (PN, FILE))
		raise bb.parse.SkipPackage("because UBOOT_MACHINE is not set")
}

UBOOT_ELF ?= "u-boot-${MACHINE}-${PV}-${PR}"
UBOOT_IMAGE ?= "u-boot-${MACHINE}-${PV}-${PR}.bin"
UBOOT_IMAGE_UIMAGE ?= "u-boot-${MACHINE}-${PV}-${PR}.img"
UBOOT_BINARY ?= "u-boot.bin"
UBOOT_SYMLINK_ELF ?= "u-boot-${MACHINE}"
UBOOT_SYMLINK ?= "u-boot-${MACHINE}.bin"
UBOOT_SYMLINK_UIMAGE ?= "u-boot-${MACHINE}.img"
UBOOT_MAKE_TARGET ?= "u-boot.img"
INC_PR = "r0"

LICENSE = "GPLv2+"

do_compile () {
	unset LDFLAGS
	unset CFLAGS
	unset CPPFLAGS
	oe_runmake ${UBOOT_MACHINE}
	oe_runmake ${UBOOT_MAKE_TARGET}
}

do_install () {
    install -d ${D}/boot
    install ${S}/${UBOOT_BINARY} ${D}/boot/${UBOOT_IMAGE}
    install ${S}/u-boot ${D}/boot/${UBOOT_ELF}
    ln -sf ${UBOOT_IMAGE} ${D}/boot/${UBOOT_BINARY}
}

FILES_${PN} = "/boot"

do_deploy () {
	install ${S}/u-boot.bin ${DEPLOYDIR}/${UBOOT_IMAGE}
	install ${S}/u-boot.img ${DEPLOYDIR}/${UBOOT_IMAGE_UIMAGE}
	install ${S}/u-boot ${DEPLOYDIR}/${UBOOT_ELF}

	cd ${DEPLOYDIR}
	rm -f ${UBOOT_SYMLINK}
	ln -sf ${UBOOT_IMAGE} ${UBOOT_SYMLINK}
	rm -f ${UBOOT_SYMLINK_UIMAGE}
	ln -sf ${UBOOT_IMAGE_UIMAGE} ${UBOOT_SYMLINK_UIMAGE}
	rm -f ${UBOOT_SYMLINK_ELF}
	ln -sf ${UBOOT_ELF} ${UBOOT_SYMLINK_ELF}
}
addtask deploy before do_build after do_compile
