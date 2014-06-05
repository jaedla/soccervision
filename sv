#!/system/bin/sh

gdb=
if [ x${1} = x--gdb ] ; then
  gdb="gdbserver :12345"
fi

export LD_PRELOAD=/system/lib/libasan_preload.so
${gdb} ./soccervision
