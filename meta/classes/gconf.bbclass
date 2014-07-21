DEPENDS += "gconf gconf-native"

# These are for when gconftool is used natively and the prefix isn't necessarily
# the sysroot.  TODO: replicate the postinst logic for -native packages going
# into sysroot as they won't be running their own install-time schema
# registration (disabled below) nor the postinst script (as they don't happen).
export GCONF_SCHEMA_INSTALL_SOURCE = "xml:merged:${STAGING_DIR_NATIVE}${sysconfdir}/gconf/gconf.xml.defaults"
export GCONF_BACKEND_DIR = "${STAGING_LIBDIR_NATIVE}/GConf/2"

# Disable install-time schema registration as we're a packaging system so this
# happens in the postinst script, not at install time.  Set both the configure
# script option and the traditional envionment variable just to make sure.
EXTRA_OECONF += "--disable-schemas-install"
export GCONF_DISABLE_MAKEFILE_SCHEMA_INSTALL = "1"

gconf_postinst() {
if [ "x$D" != "x" ]; then
	exit 1
fi
SCHEMA_LOCATION=/etc/gconf/schemas
for SCHEMA in ${SCHEMA_FILES}; do
	if [ -e $SCHEMA_LOCATION/$SCHEMA ]; then
		HOME=/root GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source` \
			gconftool-2 \
			--makefile-install-rule $SCHEMA_LOCATION/$SCHEMA > /dev/null
	fi
done
}

gconf_prerm() {
SCHEMA_LOCATION=/etc/gconf/schemas
for SCHEMA in ${SCHEMA_FILES}; do
	if [ -e $SCHEMA_LOCATION/$SCHEMA ]; then
		HOME=/root GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source` \
			gconftool-2 \
			--makefile-uninstall-rule $SCHEMA_LOCATION/$SCHEMA > /dev/null
	fi
done
}

python populate_packages_append () {
    import re
    packages = d.getVar('PACKAGES', True).split()
    pkgdest =  d.getVar('PKGDEST', True)
    
    for pkg in packages:
        schema_dir = '%s/%s/etc/gconf/schemas' % (pkgdest, pkg)
        schemas = []
        schema_re = re.compile(".*\.schemas$")
        if os.path.exists(schema_dir):
            for f in os.listdir(schema_dir):
                if schema_re.match(f):
                    schemas.append(f)
        if schemas != []:
            bb.note("adding gconf postinst and prerm scripts to %s" % pkg)
            d.setVar('SCHEMA_FILES', " ".join(schemas))
            postinst = d.getVar('pkg_postinst_%s' % pkg, True) or d.getVar('pkg_postinst', True)
            if not postinst:
                postinst = '#!/bin/sh\n'
            postinst += d.getVar('gconf_postinst', True)
            d.setVar('pkg_postinst_%s' % pkg, postinst)
            prerm = d.getVar('pkg_prerm_%s' % pkg, True) or d.getVar('pkg_prerm', True)
            if not prerm:
                prerm = '#!/bin/sh\n'
            prerm += d.getVar('gconf_prerm', True)
            d.setVar('pkg_prerm_%s' % pkg, prerm)
            rdepends = d.getVar("RDEPENDS_%s" % pkg, True) or ""
            rdepends += ' ' + d.getVar('MLPREFIX') + 'gconf'
            d.setVar("RDEPENDS_%s" % pkg, rdepends)

}
