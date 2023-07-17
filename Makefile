all: debug

include barebones.mk

arch?=x86_64
os?=linux
abi?=gnu
TRIPLET=$(arch)-$(os)-$(abi)

run-image:
	docker run --net=host -i -t -v ~/.vgazer:/root/.vgazer \
     -v `pwd`:/mnt/steroids --entrypoint sh steroids-deps-$(TRIPLET)

release:
	docker run --net=host -i -t \
     -v ~/.vgazer:/root/.vgazer -v `pwd`:/mnt/steroids \
     --entrypoint sh steroids-deps-$(TRIPLET) \
     -E -c "make bb_build_release toolchain=$(TRIPLET)-gcc \
     --include-dir=/usr/local/$(TRIPLET)/include" | tee build.log

debug:
	docker run --net=host -i -t \
     -v ~/.vgazer:/root/.vgazer -v `pwd`:/mnt/steroids \
     --entrypoint sh steroids-deps-$(TRIPLET) \
     -E -c "make bb_build_debug toolchain=$(TRIPLET)-gcc \
     --include-dir=/usr/local/$(TRIPLET)/include" | tee build.log

coverage:
	docker run --net=host -i -t \
     -v ~/.vgazer:/root/.vgazer -v `pwd`:/mnt/steroids \
     --entrypoint sh steroids-deps-$(TRIPLET) \
     -E -c "make bb_build_coverage toolchain=$(TRIPLET)-gcc \
     --include-dir=/usr/local/$(TRIPLET)/include" | tee build.log

lint:
	docker run --net=host -i -t \
     -v ~/.vgazer:/root/.vgazer -v `pwd`:/mnt/steroids \
     --entrypoint sh steroids-deps-$(TRIPLET) \
     -E -c "make bb_build_lint toolchain=$(TRIPLET)-gcc \
     --include-dir=/usr/local/$(TRIPLET)/include" | tee build.log

iwyu:
	docker run --net=host -i -t \
     -v ~/.vgazer:/root/.vgazer -v `pwd`:/mnt/steroids \
     --entrypoint sh steroids-deps-$(TRIPLET) \
     -E -c "make bb_build_iwyu toolchain=$(TRIPLET)-gcc \
     --include-dir=/usr/local/$(TRIPLET)/include" | tee build.log

lint_build: bb_lint_build

install: bb_install

clean: bb_clean

dockerfiles/deps-$(TRIPLET).dockerfile: deps/$(TRIPLET) scripts/build_dockerfile.py
	scripts/build_dockerfile.py --target=$(TRIPLET)

build-image: dockerfiles/deps-$(TRIPLET).dockerfile
	docker build --network=host --no-cache \
     -f dockerfiles/deps-$(TRIPLET).dockerfile \
     -t steroids-deps-$(TRIPLET) --build-arg USER_ID=$(SUDO_UID) \
     --build-arg GROUP_ID=$(SUDO_GID) .
