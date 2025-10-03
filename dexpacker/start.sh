#!/bin/bash

# 检查参数
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <source.apk> <output_packed.apk>"
    exit 1
fi

SOURCE_APK=$1
OUTPUT_APK=$2
TEMP_DIR="assets"
ORIGINAL_DEX="$TEMP_DIR/classes.dex"
ENCRYPTED_DEX="$TEMP_DIR/encrypted_classes.dex"
STUB_DEX="stub.dex"
PACKER_EXEC="./packer"

# 检查必要文件是否存在
if [ ! -f "$SOURCE_APK" ]; then
    echo "Error: Source APK '$SOURCE_APK' not found."
    exit 1
fi
if [ ! -f "$STUB_DEX" ]; then
    echo "Error: Stub DEX '$STUB_DEX' not found."
    exit 1
fi
if [ ! -f "$PACKER_EXEC" ]; then
    echo "Error: Packer executable '$PACKER_EXEC' not found. Did you compile packer.c?"
    exit 1
fi

echo "--- Starting APK Packing Process ---"

# 1. 清理并创建临时目录
echo "Step 1: Unpacking APK to temporary directory..."
rm -rf "$TEMP_DIR"
mkdir -p "$TEMP_DIR"
unzip -q "$SOURCE_APK" -d "$TEMP_DIR"

if [ ! -f "$ORIGINAL_DEX" ]; then
    echo "Error: classes.dex not found in '$SOURCE_APK'. Is it a valid APK?"
    rm -rf "$TEMP_DIR"
    exit 1
fi

# 2. 使用 C 程序加密原始的 classes.dex
echo "Step 2: Encrypting classes.dex..."
$PACKER_EXEC "$ORIGINAL_DEX" "$ENCRYPTED_DEX"

# 3. 将加密后的 DEX 移动到 assets 目录
echo "Step 3: Moving encrypted DEX to assets folder..."
mkdir -p "$TEMP_DIR/assets"
mv "$ENCRYPTED_DEX" "$TEMP_DIR/assets/"

# 4. 用 stub.dex 替换原来的 classes.dex
echo "Step 4: Replacing original classes.dex with stub.dex..."
cp "$STUB_DEX" "$ORIGINAL_DEX"

# 5. 重新打包 APK
echo "Step 5: Repacking APK into '$OUTPUT_APK'..."
cd "$TEMP_DIR"
zip -r -q "../$OUTPUT_APK" .
cd ..

# 6. 清理临时文件
echo "Step 6: Cleaning up temporary files..."
rm -rf "$TEMP_DIR"

echo "--- Packing Complete! Packed APK saved as '$OUTPUT_APK' ---"
echo "NOTE: You still need to sign the APK before installing."