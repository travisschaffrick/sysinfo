pkgname=sysinfo
pkgver=0.1.0
pkgrel=1
pkgdesc="Simple ncurses-based system monitor"
arch=('x86_64')
url="https://github.com/travisschaffrick/sysinfo"
license=('MIT')
depends=('ncurses')
makedepends=('gcc' 'make')
source=("$pkgname-$pkgver.tar.gz::$url/archive/refs/tags/v$pkgver.tar.gz")
sha256sums=('SKIP')

build() {
    cd "$pkgname-$pkgver"
    make
}

package() {
    cd "$pkgname-$pkgver"
    install -Dm755 sysinfo "$pkgdir/usr/bin/sysinfo"
}
