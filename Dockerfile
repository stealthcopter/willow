FROM espressif/idf:v5.0.2

ENV DEBIAN_FRONTEND="noninteractive"

RUN \
  --mount=type=cache,target=/var/cache/apt \
  --mount=type=cache,target=/var/lib/apt/lists \
  apt-get -qq update && \
  apt-get -qq install \
	git \
	libusb-1.0-0 \
	python3 \
	python3-pip \
	python-is-python3 \
	python3-venv \
	tio \
	sudo \
	nano

RUN useradd --create-home --uid 1000 build
COPY --chown=1000 container.gitconfig /home/build/.gitconfig

ENV PATH="$PATH:/sallow/.local/bin"
WORKDIR /sallow

ENV ADF_VER="c1e884871872bc748a0248746fcba4980170356f"
RUN \
  cd /opt/esp/idf && \
  curl https://raw.githubusercontent.com/espressif/esp-adf/$ADF_VER/idf_patches/idf_v5.0_freertos.patch | patch -p1

COPY gdma-logging.patch /tmp
RUN cd /opt/esp/idf && \
  patch -p1 < /tmp/gdma-logging.patch
