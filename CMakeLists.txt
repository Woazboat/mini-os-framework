cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0079 NEW)


set(OS_NAME "MiniOS")
set(KERNEL_BIN minioskernel)
set(DISK_IMAGE_FILE miniosdisk.img)


project(${OS_NAME}
  LANGUAGES C ASM)
set(CMAKE_SYSTEM_NAME Generic)

add_compile_definitions(OS_NAME="${OS_NAME}")

set(KERNEL_CMAKE_C_FLAGS -m32 -O0 -static -Wall -Wextra -Werror -Wno-nonnull-compare -nostdinc -nostdlib -nostartfiles -nodefaultlibs -fno-builtin -fno-exceptions -fno-stack-protector -ffreestanding -mno-red-zone -mno-mmx -mno-sse2 -mno-sse3 -mno-3dnow -fPIE)
set(KERNEL_CMAKE_ASM_FLAGS ${KERNEL_CMAKE_C_FLAGS})
set(KERNEL_LD_FLAGS -m32 -mcmodel=kernel -Wl,--build-id=none -Wl,-z,max-page-size=0x1000 -Wl,-melf_i386 -nostdinc -nostdlib -nodefaultlibs -pie)


add_library(kernelinterface INTERFACE)
target_compile_options(kernelinterface INTERFACE
  $<$<COMPILE_LANGUAGE:C>:${KERNEL_CMAKE_C_FLAGS}>
  $<$<COMPILE_LANGUAGE:ASM;>:${KERNEL_CMAKE_ASM_FLAGS}>
  )
target_link_options(kernelinterface INTERFACE
  ${KERNEL_LD_FLAGS}
  )




add_executable(${KERNEL_BIN})
target_link_libraries(${KERNEL_BIN} kernelinterface)
target_link_options(${KERNEL_BIN} PRIVATE
  -u entry -Wl,-T ${CMAKE_SOURCE_DIR}/kernel-ld-script.ld
  )
file(GLOB SOURCE_FILES *.c *.S)
target_sources(${KERNEL_BIN} PRIVATE ${SOURCE_FILES})


add_subdirectory(lib)





find_program(DD dd)
find_program(SED sed)
find_program(QEMU_I386 qemu-system-i386)
find_program(GRUB_MKIMAGE grub-mkimage)
find_program(MKFS_EXT2 mkfs.ext2
  PATHS /usr/sbin)



set(PRELOADED_GRUB2_MODULES pata part_msdos ext2 configfile search_label normal ls reboot serial echo cat)

add_custom_target(diskimage
  DEPENDS ${KERNEL_BIN}
  BYPRODUCTS ${PROJECT_BINARY_DIR}/${DISK_IMAGE_FILE}
  BYPRODUCTS bootpartition
  BYPRODUCTS bootpartition.img
  COMMENT "Creating bootable disk image ${PROJECT_BINARY_DIR}/${DISK_IMAGE_FILE}"

  # Create blank disk image
  COMMAND ${DD} if=/dev/zero of=${PROJECT_BINARY_DIR}/${DISK_IMAGE_FILE} bs=1M count=10

  # Prepare kernel binary and grub2 config file + modules
  COMMAND mkdir -p bootpartition/boot/grub/i386-pc/
  COMMAND cp ${PROJECT_BINARY_DIR}/${KERNEL_BIN} bootpartition/boot/kernel
  COMMAND ${SED} -e 's/@OS_NAME@/${OS_NAME}/g' ${CMAKE_SOURCE_DIR}/grub2/grub.cfg > bootpartition/boot/grub/grub.cfg
  COMMAND cp /usr/lib/grub/i386-pc/* bootpartition/boot/grub/i386-pc/

  # Create grub2 boot image, include preloaded modules and early config
  COMMAND ${GRUB_MKIMAGE} -p '\(ata0,mdsos1\)/boot/grub' -c ${CMAKE_SOURCE_DIR}/grub2/early-grub.cfg -O i386-pc -o bootpartition/boot/grub/i386-pc/core.img ${PRELOADED_GRUB2_MODULES}

  # Create boot partition on blank disk image
  COMMAND ${CMAKE_SOURCE_DIR}/partimage.sh ${PROJECT_BINARY_DIR}/${DISK_IMAGE_FILE}

  # Install grub on disk image, take care not to overwrite MBR partition table
  COMMAND ${DD} if=bootpartition/boot/grub/i386-pc/boot.img of=${PROJECT_BINARY_DIR}/${DISK_IMAGE_FILE} bs=446 count=1 conv=sparse,notrunc
  COMMAND ${DD} if=bootpartition/boot/grub/i386-pc/core.img of=${PROJECT_BINARY_DIR}/${DISK_IMAGE_FILE} bs=512 seek=1 conv=sparse,notrunc

  # Create ext2 file system on boot partition
  COMMAND ${DD} if=/dev/zero of=bootpartition.img bs=1M count=9
  COMMAND ${MKFS_EXT2} bootpartition.img -d bootpartition/ -L "boot"
  COMMAND ${DD} if=bootpartition.img of=${PROJECT_BINARY_DIR}/${DISK_IMAGE_FILE} bs=1M seek=1
  )



add_custom_target(qemu
  DEPENDS diskimage
  COMMAND ${QEMU_I386} -m 8M -drive file=${PROJECT_BINARY_DIR}/${DISK_IMAGE_FILE},index=0,media=disk,format=raw -debugcon stdio -no-reboot -d guest_errors -enable-kvm -cpu qemu32
  COMMENT "Executing `${QEMU_I386} -m 8M -drive file=${PROJECT_BINARY_DIR}/${DISK_IMAGE_FILE},index=0,media=disk,format=raw -debugcon stdio -no-reboot -d guest_errors -enable-kvm -cpu qemu32`"
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  COMMAND reset -I
  )
