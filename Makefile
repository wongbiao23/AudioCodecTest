ANDROID_NDK:=/PATH-TO/Android/sdk/ndk-bundle/
OUT_DIR:=out

.PHONY:all prebuild

all:prebuild
	@cmake --build ${OUT_DIR}/build

NDK_ABI := armeabi-v7a

prebuild:
	@mkdir -p ${OUT_DIR}
	@mkdir -p ${OUT_DIR}/bin
	@mkdir -p ${OUT_DIR}/libs
	@mkdir -p ${OUT_DIR}/build
	@cmake \
	-DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
	-DCMAKE_BUILD_TYPE=Debug \
	-DANDROID_ABI=${NDK_ABI} \
	-DANDROID_PLATFORM=android-19 \
	-B ${OUT_DIR}/build

clean:
	@rm -rf ${OUT_DIR}