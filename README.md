# Dumpfidomsg

This is a utility to convert the contents of a Fidonet message into a
RFC822-style message.  The Fidonet message format is a binary format of the
style used by the msged program (https://github.com/jrnutt/msged).

It is a stripped-down modification of iftoss from the ifmail *Internet to
FidoNet gateway and transport* package by Eugene G. Crosser. The delivery
functions were removed and the input format was changed to expect a single
message instead of a message packet.

This program can be useful to both read message files without requiring a
specialized message editor or viewer, and to convert existing messages into a
format for easier archiving or indexing.

## Installation

The source code can be downloaded from https://github.com/dfandrich/dumpfidomsg
Edit the CONFIG file to match your target system then build it with

    make

Install it to the right location with

    make install
    make man

The makefile uses certain GNU make extensions and will not work with other make
programs. Note that not all compile-time features mentioned in CONFIG have been
tested in this derivative.

## Usage

Pass a Fidonet message into stdin of the program and it will convert it to an
Internet message on stdout, like this:

    dumpfidomsg < 123.msg > 123.eml

## Author

The original author was Eugene G. Crosser. This version was created by Dan
Fandrich <dan@coneharvesters.com>.
