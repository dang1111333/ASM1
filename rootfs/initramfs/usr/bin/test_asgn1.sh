cat > test_asgn1.sh << 'EOF'
#!/bin/sh
echo "=== TASK 1: ASGN1 DRIVER IOCTL TEST ==="

# Load module và tạo device node
insmod /lib/modules/5.15.0/asgn1_driver.ko
mknod /dev/asgn1 c 241 0 2>/dev/null
chmod 666 /dev/asgn1

echo ""
echo "[Step 1] Test Version:"
asgn1_user_test version

echo ""
echo "[Step 2] Test Mode 1 (Echo Mode):"
echo "Hello Echo" > /dev/asgn1
echo "Read back: $(cat /dev/asgn1)"

echo ""
echo "[Step 3] Test Mode 0 (Kernel Mode):"
asgn1_user_test setmode 0
echo "Hello Kernel" > /dev/asgn1
echo "Read back: $(cat /dev/asgn1)"

echo ""
echo "[Step 4] Test Stats:"
asgn1_user_test stats

echo ""
echo "[Step 5] Test Reset Buffer:"
asgn1_user_test reset
echo "Read back after reset: [$(cat /dev/asgn1)]"
asgn1_user_test stats

echo ""
echo "[Step 6] Kernel Messages (dmesg):"
dmesg | grep asgn1_driver | tail -10

# Gỡ bỏ module sau khi test xong
rmmod asgn1_driver
echo "=== TASK 1 TEST COMPLETED ==="
EOF
