[![Build Status](https://travis-ci.org/neochapay/QtInstagram.png)](https://travis-ci.org/neochapay/QtInstagram)
[![codecov.io](https://codecov.io/github/neochapay/QtInstagram/coverage.svg?branch=master)](https://codecov.io/github/neochapay/QtInstagram/branch/master)

# QtInstagram
Unofficial Instagram API and Client writen in Qt. Based on https://github.com/mgp25/Instagram-API

IMPORTANT: Your account may be blocked if you use this code. Or maybe not ^_^

In application you can:
* Login
* See your timeline
* Send like
* Send comment

In the API you can:
* Post image (not public yet - give me fiew weeks to optimize code)
* Post video (not public yet - give me fiew weeks to optimize code)
* Registration
* Set your profile public or private
* Edit your profile
* Change (not public yet) and remove profile picture
* Remove self tags
* Post comments
* Send Likes
* Get recent and follow activity
* Get user tags and tagfeed
* Get media likes and comments
* Get self and users timelines

# How to use QtInstagram

This project can be used in two ways: either as a shared library, or as a set
of C++ files to be directly link into your project.

## Building QtInstagram as a shared library

Just run `qmake` in the root directory of the project, and then build the library:

```
  qmake
  make
```
By default, the v2 version of the library is used. If you want to use the v1
version, you need to tell that to qmake:
```
  qmake INSTAGRAM_API_VERSION=v1
  make
```

## Using QtInstagram's source code as part of your project

Just copy the repository into a subdirectory in your project; we recommend
storing it as a git submodule, so getting updates will be easier. Then, in
order to add QtInstagram into your build, add this line into your qmake project
file:

```
  include(QtInstagram/QtInstagram.pri)
```
By default, the v2 version of the library will be used. In order to use the v1
version, you need to define the `INSTAGRAM_API_VERSION` variable _before_
including QtInstagram:
```
  INSTAGRAM_API_VERSION = "v1"
  include(QtInstagram/QtInstagram.pri)
```
