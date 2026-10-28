#!/bin/sh
echo "=== TASK 2: COUNTER MISC DRIVER TEST ==="
insmod /lib/modules/5.15.0/counter_driver.ko

echo ""
echo "[Step 1] Initial Read (expect 0):"
cat /dev/counter

echo ""
echo "[Step 2] Auto-increment Read (expect 1, 2):"
cat /dev/counter
cat /dev/counter

echo ""
echo "[Step 3] Stats before write/reset:"
cat /proc/counter_info

echo ""
echo "[Step 4] Write 100 to counter:"
echo "100" > /dev/counter
echo "Read back (expect 100):"
cat /dev/counter

echo ""
echo "[Step 5] Reset to 0:"
echo "0" > /dev/counter
echo "Read back (expect 0):"
cat /dev/counter

echo ""
echo "[Step 6] Final Stats:"
cat /proc/counter_info

echo ""
echo "[Step 7] Kernel Messages:"
dmesg | grep counter_driver | tail -10

rmmod counter_driver
echo "=== TASK 2 TEST COMPLETED ==="
