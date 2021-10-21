OUT_DIR:=out

.PHONY:all prebuild

all:prebuild
	@cmake --build ${OUT_DIR}/build

prebuild:
	@mkdir -p ${OUT_DIR}
	@mkdir -p ${OUT_DIR}/bin
	@mkdir -p ${OUT_DIR}/libs
	@mkdir -p ${OUT_DIR}/build
	@cmake \
	-B ${OUT_DIR}/build

clean:
	@rm -rf ${OUT_DIR}