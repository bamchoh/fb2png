# fb2png

linux(raspberry pi zero 2w 用)のフレームバッファをpng画像にして保存するサンプル

# ビルド方法

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake -DCMAKE_SYSROOT=/home/bamchoh/rpi-sysroot
cmake --build .
```

# 感想

`-lpng` が通らなくてめちゃくちゃ悩んだんだけど`CMAKE_EXE_LINKER_FLAGS` 足したらいけた。

その悔しさを記録するためにアップした。
