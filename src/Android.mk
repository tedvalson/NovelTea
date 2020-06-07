LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := noveltea
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libNovelTea.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := sfml-system sfml-window sfml-graphics
LOCAL_SHARED_LIBRARIES += sfml-audio sfml-network sfml-activity
LOCAL_WHOLE_STATIC_LIBRARIES := sfml-main

prebuilt_path := $(call local-prebuilt-path,$(LOCAL_SRC_FILES))
prebuilt := $(strip $(wildcard $(prebuilt_path)))

ifdef prebuilt
    include $(PREBUILT_STATIC_LIBRARY)
endif

$(call import-module,third_party/sfml)
