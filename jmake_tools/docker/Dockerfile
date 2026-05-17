FROM gcc:16.1 AS base
LABEL Description="jmake development environment"

# target architecture: X86 or AArch64
ARG TARGET=AArch64
ARG TARGET_TRIPLE=aarch64-unknown-linux-gnu

# install directory
ARG INSTALL_DIR=/opt/clang-p2996

# number of processors for build
ARG NPROC="$(nproc)"

# builder stage for clang bloomberg p2996
FROM base AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates build-essential git cmake \
    ninja-build lld python3 libxml2-dev zlib1g-dev \
    libedit-dev libncurses-dev pkg-config lld \
    curl file

WORKDIR /tmp/src

# clone clang bloomberg p2996
RUN git clone --depth=1 --branch=p2996 https://github.com/bloomberg/clang-p2996.git

WORKDIR /tmp/src/clang-p2996

# generate build files
RUN cmake -G Ninja \ 
    -S llvm \
    -B build \
    -DLLVM_TARGETS_TO_BUILD=$TARGET \
    -DLLVM_DEFAULT_TARGET_TRIPLE=$TARGET_TRIPLE \
    -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
    -DLLVM_ENABLE_RUNTIMES="libunwind;libcxxabi;libcxx;compiler-rt" \
    -DCMAKE_BUILD_TYPE=Release \ 
    -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
    -DLLVM_USE_LINKER=lld \
    -DCLANG_DEFAULT_CXX_STDLIB=libc++ \
    -DCLANG_DEFAULT_LINKER=lld \
    -DCLANG_DEFAULT_RTLIB=compiler-rt

# build & install
WORKDIR /tmp/src/clang-p2996/build

RUN ninja -j"$NPROC"
RUN ninja install

FROM base AS runner

COPY --from=builder $INSTALL_DIR $INSTALL_DIR

RUN apt -y update
RUN apt install -y --no-install-recommends \
    git cmake

ENV PATH=$INSTALL_DIR/bin:$PATH

CMD ["sleep", "infinity"]