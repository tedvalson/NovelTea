LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := noveltea
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libNovelTea.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

prebuilt_path := $(call local-prebuilt-path,$(LOCAL_SRC_FILES))
prebuilt := $(strip $(wildcard $(prebuilt_path)))

ifdef prebuilt
    include $(PREBUILT_STATIC_LIBRARY)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := noveltea-sfml
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libNovelTea-SFML.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_WHOLE_STATIC_LIBRARIES := sfml-main noveltea
LOCAL_STATIC_LIBRARIES := sfml-system-s sfml-window-s sfml-graphics-s
LOCAL_EXPORT_LDFLAGS := -Wl,--allow-multiple-definition

prebuilt_path := $(call local-prebuilt-path,$(LOCAL_SRC_FILES))
prebuilt := $(strip $(wildcard $(prebuilt_path)))

ifdef prebuilt
    include $(PREBUILT_STATIC_LIBRARY)
endif

$(call import-module,third_party/sfml)
