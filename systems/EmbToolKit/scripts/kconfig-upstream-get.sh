#!/bin/sh

files=$(cat scripts/kconfig-upstream.plist)
for f in $files; do
	cp ~/projects/linux/linux.git/$f $f;
done
