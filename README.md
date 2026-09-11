# bmon - Bandwidth Monitor

[![Build](https://github.com/Jafaral/bmon/actions/workflows/build.yml/badge.svg)](https://github.com/Jafaral/bmon/actions/workflows/build.yml)
[![Coverity Status](https://scan.coverity.com/projects/2864/badge.svg)](https://scan.coverity.com/projects/2864)

bmon is a monitoring and debugging tool to capture networking related
statistics and prepare them visually in a human friendly way. It
features various output methods including an interactive curses user
interface and a programmable text output for scripting.

## Download

 * [Latest Release](https://github.com/Jafaral/bmon/releases/latest)
 * [Older Releases](https://github.com/Jafaral/bmon/releases)

## Debian/Ubuntu Installation

```
git clone https://github.com/Jafaral/bmon.git
cd bmon
apt-get install build-essential make libconfuse-dev libnl-3-dev libnl-route-3-dev libncurses-dev pkg-config dh-autoreconf
./autogen.sh
./configure
make
make install
bmon
```
## CentOS 6 Installation

```
git clone https://github.com/Jafaral/bmon.git
cd bmon
yum install make libconfuse-devel libnl3-devel libnl-route3-devel ncurses-devel
./autogen.sh
./configure
make
make install
bmon
```

## CentOS 7 Installation

```
yum install bmon
```

## Fedora Installation

```
dnf install bmon
```

## OSX Installation

### Brew
```
brew install bmon
```

### Compile yourself
Install libconfuse
```
wget https://github.com/martinh/libconfuse/releases/download/v2.8/confuse-2.8.zip
unzip confuse-2.8.zip && cd confuse-2.8
PATH=/usr/local/opt/gettext/bin:$PATH ./configure
make
make install
```

Install bmon
```
git clone https://github.com/Jafaral/bmon.git
cd bmon
./autogen.sh
./configure
make
make install
bmon
```

-------------
## New in 5.0
 * out_curses: use xcalloc instead of a fixed buffer by @NachiP in #54
 * Avoid trying to include <values.h> on Android by @fornwall in #55
 * Allow numeric colors in bmonrc by @afh in #62
 * Fix typographical errors by @afh in #63
 * curses: Fix seperator lines by @afh in #64
 * curses: Add CTRL-N/CTRL-P for next/previous element by @afh in #65
 * Add package installation instructions for Fedora/CentOS by @carlwgeorge in #78
 * Fix build in FreeBSD/DragonflyBSD by @farrokhi in #76
 * Update to autoconf of the common era by @Jafaral in #116
 * fix segfault on element removal by @dcaratti in #102
 * ci: add GitHub Actions builds for Linux, macOS, and FreeBSD by @Jafaral in #118
 * ci: enable -Werror for compiler warnings by @Jafaral in #119
 * feat: Add IPv4/IPv6 addresses to additional info by @csiltala in #117
 * feat: Add configure option to disable IPv6 in netlink module by @csiltala in #121
 * fix: Fix memory leaks in module_set() by @csiltala in #122
 * fix: Convert usage of strdup/free to xstrdup/xfree by @csiltala in #124
 * feat: Add ability to toggle TC elements by @csiltala in #120
 * curses: render graphs with Braille cells by @crgr in #114
 * display filters for TC ingress and egress by @dcaratti in #100
 * Fix compile issue in OpenBSD by @FlyingHacker in #92
 * Fix spelling and grammar in docs and comments by @Jafaral in #125
 * Add support for JSON output by @Jafaral in #126
 * Remove the 'Really quit' dialog and just exit cleanly by @graysky2 in #113

-------------
### Usage

To run bmon in the default curses mode:

> bmon

There are many other options available and full help is
provided via:

> bmon --help

## Screenshots

![Screenshot 1](https://github.com/Jafaral/bmon/raw/gh-pages/images/shot3.png)
![Screenshot 2](https://github.com/Jafaral/bmon/raw/gh-pages/images/shot1.png)
![Screenshot 3](https://github.com/Jafaral/bmon/raw/gh-pages/images/shot2.png)

## Copyright

Various authors, see git commit log.

> *Copyright (c) 2001-2016 Thomas Graf <tgraf@suug.ch>*
> 
> *Copyright (c) 2013 Red Hat, Inc.*
> 
> *Copyright (c) 2026 Architecture Technology Corporation*

Please see the [LICENSE.BSD](https://github.com/Jafaral/bmon/blob/master/LICENSE.BSD)
and [LICENSE.MIT](https://github.com/Jafaral/bmon/blob/master/LICENSE.MIT) files for
additional details.

