#!/bin/sh

x="127.0.0.1"
y=2012
z="ab\c\x2fp\x11\x2e\r\n"

$1 "$x" $2 "$y" $3 "$z"
echo ""
