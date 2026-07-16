# Copyright 2026 Gentoo Authors
# Distributed under the terms of the GNU General Public License v2

EAPI=8

inherit cmake

DESCRIPTION="C Development Framework - Object Oriented libraries for C"
HOMEPAGE="https://github.com/kubawoo/cdf"


if [[ ${PV} == *9999* ]]; then
        inherit git-r3
        EGIT_REPO_URI="https://github.com/kubawoo/cdf.git"
        EGIT_BRANCH="main"
        KEYWORDS=""
else
        SRC_URI="https://github.com/kubawoo/cdf/archive/v${PV}.tar.gz -> ${P}.tar.gz"
        KEYWORDS="~amd64"
fi


LICENSE="MIT"
SLOT="0"


IUSE="+sqlite test examples static-libs"

RESTRICT="mirror"

DEPEND="
        sqlite? ( dev-db/sqlite )
"
BDEPEND="
        >=dev-util/cmake-3.20
        sys-devel/gcc
"

src_configure() {
        local mycmakeargs=(
                -DCDF_BUILD_DB_SQLITE=$(usex sqlite ON OFF)
                -DCDF_BUILD_STATIC=$(usex static-libs ON OFF)
                -DCDF_BUILD_EXAMPLES=$(usex examples ON OFF)
                -DBUILD_TESTING=$(usex test ON OFF)
        )
        cmake_src_configure
}

src_install() {
        cmake_src_install
}


