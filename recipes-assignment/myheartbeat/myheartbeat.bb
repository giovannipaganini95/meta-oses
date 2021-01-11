DESCRIPTION = "Simple Heartbeat Monitor App"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

COMPATIBLE_MACHINE = "qemuarm|raspberrypi3"

SRC_URI = "file://main.cpp \
           file://circular_buffer.hpp \
           file://producer.hpp \
           file://consumer.hpp \
           file://fft_function.hpp \
          "
S = "${WORKDIR}"

CXXFLAGS = "-g -std=c++17"
SOURCES = "main.cpp circular_buffer.hpp producer.hpp consumer.hpp fft_function.hpp"
LDFLAGS = "-lstdc++ -lm -lpthread"
TARGET = "myheartbeat"

do_compile() {
            ${CC} ${CXXFLAGS} ${SOURCES} ${LDFLAGS} -o ${TARGET}
           
}
do_install() {
            install -d ${D}${bindir}
            install -m 0755 ${TARGET} ${D}${bindir}
}
