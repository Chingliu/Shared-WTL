## About

This repository contains C++ source code of many WTL based classes that I wrote and that are shared through many of my projects.

It also contains a copy of the WTL library (http://sourceforge.net/projects/wtl/), note that this copy is customized and subject to minor changes, it is not supposed to always be the original WTL source.

## Framework general concepts

To be written.

## Engine system

Three main cooperative concepts involved:

- Callbacks system to run function in an exception safe manner

- Looging chain per-thread, deferred for output in case of exceptions

- Exception thrown monitor to output the log chain