ifeq ($(shell uname -m),x86_64)
	ARCH=x86_64
endif
ifeq ($(shell uname),Linux)
	OS=linux
endif

ifeq ($(ARCH)-$(OS),x86_64-linux)
	TRIPLET=x86_64-linux-gnu
	CPU_CORES=$(shell nproc --all)
endif

rebuild: clean build

cmake_build:
	mkdir cmake_build

refresh: cmake_build
	cd cmake_build; cmake .. -DCMAKE_BUILD_TYPE=Debug -DST_MORE_WARNINGS=ON \
     -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/toolchain-cross-$(TRIPLET).cmake \
     -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON

build: refresh
	cmake --build cmake_build -- -j$(CPU_CORES)

clean:
	-rm -r -f ./cmake_build

build-in-docker:
ifeq ($(ARCH)-$(OS),x86_64-linux)
	docker run -i -t \
     -v `pwd`:/mnt/steroids --entrypoint sh steroids-deps-x86_64-linux-gnu \
     -E -c "make build" | tee build.log
else
	echo "Error: host $(ARCH) architecture or $(OS) os not implemented now"
endif

rebuild-in-docker:
ifeq ($(ARCH)-$(OS),x86_64-linux)
	docker run -i -t \
     -v `pwd`:/mnt/steroids --entrypoint sh steroids-deps-x86_64-linux-gnu \
     -E -c "make rebuild" | tee build.log
else
	echo "Error: host $(ARCH) architecture or $(OS) os not implemented now"
endif

build-deps-x86_64-linux-gnu: dockerfiles/deps-x86_64-linux-gnu.dockerfile

dockerfiles:
	mkdir dockerfiles

dockerfiles/deps-x86_64-linux-gnu.dockerfile: deps/x86_64-linux-gnu scripts/build_dockerfile.py dockerfiles
	scripts/build_dockerfile.py --target=x86_64-linux-gnu

build-image: dockerfiles/deps-x86_64-linux-gnu.dockerfile
	docker build \
     -f dockerfiles/deps-x86_64-linux-gnu.dockerfile \
     -t steroids-deps-x86_64-linux-gnu .
