#
# Copyright (c) Siemens AG, 2019-2020
#
# Authors:
#  Le Jin <le.jin@siemens.com>
#  Jan Kiszka <jan.kiszka@siemens.com>
#
# This file is subject to the terms and conditions of the MIT License.  See
# COPYING.MIT file in the top-level directory.
#

require recipes-bsp/u-boot/u-boot-custom.inc

SRC_URI += " \
    git://git.ti.com/processor-sdk/processor-sdk-u-boot.git;branch=${U_BOOT_BRANCH};rev=${U_BOOT_REV} \
    git://github.com/ARM-software/arm-trusted-firmware.git;rev=${ATF_REV};destsuffix=atf \
    git://github.com/OP-TEE/optee_os.git;rev=${OPTEE_REV};destsuffix=optee  \
    file://0001-feat-add-iot2050-platform-support.patch \
    file://0002-am654x-frequency-update-for-both-arm-clusters-and-bu.patch \
    file://0003-iot2050-turn-on-red-led-when-something-goes-wrong.patch \
    file://0004-am65x-fix-usb-hub-issue-for-disabling-charge-detect.patch \
    file://0005-am654x-remove-dependency-of-TI_SECURE_DEV_PKG.patch \
    file://0006-mmc-change-SD-to-mmc0-and-EMMC-Flash-to-mmc1.patch \
    file://0007-feat-add-scripts-to-select-fdt-name.patch \
    file://0008-feat-add-UUID-checking-before-run-distro_bootcmd.patch \
    file://0009-setting-the-rj45-port-led-behavior.patch \
    file://0010-fix-rproc-init-error-in-u-boot.patch \
    file://0011-feat-catch-up-100M-full-duplex-in-u-boot.patch \
    file://0012-feat-add-the-flash-protect-for-winbond-flash.patch \
    file://0013-bugfix-set-gpio-direction-output-show-error.patch \
    file://0014-feat-add-config-for-skipping-certificate-verificatio.patch \
    file://0015-feat-enable-verified-boot.patch \
    file://0016-feat-add-sysfw-19.12-support.patch \
    file://0017-add-the-signature-support-for-fit-image-add-the-supp.patch \
    file://0018-feat-set-sdhci0-clock-frequency-to-142.86MHz.patch \
    file://atf/0001-feat-add-atf-support-to-iot2050-platform.patch;patchdir=${WORKDIR}/atf \
    file://optee/0001-feat-add-optee-support-to-iot2050-platform.patch;patchdir=${WORKDIR}/optee \
    file://${U_BOOT_RULES} \
    file://keys \
    file://prebuild \
    "

U_BOOT_BRANCH = "processor-sdk-u-boot-2019.01"
U_BOOT_REV = "029e4c009aaeaee2d06aa8271dbd3a9e73a28aa7"
U_BOOT_BIN = "u-boot.itb"
U_BOOT_RULES = "iot2050-uboot-ti-sdk-rules"
ATF_REV   = "996d37930996c2fa39eb091508b5ad4e0d69ad35"
OPTEE_REV = "e260ea8dde9669308336d194abec8dcc18784216"

S = "${WORKDIR}/git"

SPI_FLASH_IMG = "iot2050-image-boot-${IOT2050_VARIANT}.bin"
SPI_FLASH_SIZE_KB = "8192"

# Build environment
BUILD_DEPENDS =. "openssl, libssl-dev:native, libssl-dev:arm64, python-crypto:native, python3-pyelftools,"

do_prepare_build_append() {
    ln -sf ../keys ${S}
    cp ${WORKDIR}/${U_BOOT_RULES} ${S}/debian/rules
    echo "${SPI_FLASH_IMG} /usr/lib/u-boot/${MACHINE}" > \
        ${S}/debian/u-boot-${MACHINE}.install
    echo -$(${ISAR_RELEASE_CMD}) > ${S}/.scmversion
}

do_build[dirs] += "${DEPLOY_DIR_IMAGE}"

dpkg_runbuild_prepend() {
    export IOT2050_VARIANT=${IOT2050_VARIANT}
    export SPI_FLASH_IMG=${SPI_FLASH_IMG}
    export SPI_FLASH_SIZE_KB=${SPI_FLASH_SIZE_KB}
}

do_deploy() {
    cp -f "${S}/${SPI_FLASH_IMG}" "${DEPLOY_DIR_IMAGE}"
}

addtask deploy after do_dpkg_build before do_build
do_deploy[dirs] = "${DEPLOY_DIR_IMAGE}"
