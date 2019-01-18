LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_VENDOR_MODULE := true

AMLOGIC_LIBPLAYER :=y

ifeq (1,$(strip $(shell expr $(PLATFORM_SDK_VERSION) \>= 25)))
AMADEC_C_INCLUDES:=hardware/amlogic/media/amcodec/include\
       hardware/amlogic/LibAudio/amadec/include
AMADEC_LIBS:=libamadec
else
AMADEC_C_INCLUDES:=vendor/amlogic/frameworks/av/LibPlayer/amcodec/include\
       vendor/amlogic/frameworks/av/LibPlayer/amadec/include
AMADEC_LIBS:=libamplayer
endif
ifeq ($(AMLOGIC_LIBPLAYER), y)
LOCAL_CFLAGS+=-DAMLOGIC_LIBPLAYER
endif

LOCAL_SRC_FILES:= am_userdata_test.c

LOCAL_MODULE:= am_userdata_test

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS+=-DANDROID -DAMLINUX -DCHIP_8226M -DLINUX_DVB_FEND
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../include/am_adp $(LOCAL_PATH)/../../android/ndk/include \
			$(LOCAL_PATH)/../../../../packages/amlogic/LibPlayer/amadec/include\
		    $(LOCAL_PATH)/../../../../packages/amlogic/LibPlayer/amcodec/include\
		    $(LOCAL_PATH)/../../../../packages/amlogic/LibPlayer/amffmpeg\
		    $(LOCAL_PATH)/../../../../packages/amlogic/LibPlayer/amplayer


LOCAL_SHARED_LIBRARIES := libam_adp $(AMADEC_LIBS) libcutils liblog libc

include $(BUILD_EXECUTABLE)
